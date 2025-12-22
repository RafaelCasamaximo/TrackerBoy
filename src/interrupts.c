#include <interrupts.h>
#include <emulator.h>
#include <mmu.h>
#include <stdio.h>

// Helper interno para empilhar o PC (Simula a instrução CALL/PUSH)
static void service_interrupt_push(Emulator* emu, uint16_t value) {
    emu->cpu.SP--;
    mmu_write_byte(emu, emu->cpu.SP, (value >> 8) & 0xFF); // High byte
    emu->cpu.SP--;
    mmu_write_byte(emu, emu->cpu.SP, value & 0xFF);        // Low byte
}

void request_interrupt(Emulator* emu, uint8_t interrupt_mask) {
    // Lê o valor atual de IF (0xFF0F)
    uint8_t if_reg = mmu_read_byte(emu, 0xFF0F);
    
    // Liga o bit correspondente (OR)
    if_reg |= interrupt_mask;
    
    // Escreve de volta
    mmu_write_byte(emu, 0xFF0F, if_reg);
}

void handle_interrupts(Emulator* emu) {
    // 1. Ler IE (Enable - 0xFFFF) e IF (Flag - 0xFF0F)
    uint8_t ie_reg = mmu_read_byte(emu, 0xFFFF);
    uint8_t if_reg = mmu_read_byte(emu, 0xFF0F);

    // 2. Acordar do HALT
    // Se houver qualquer interrupção pendente E habilitada no IE, a CPU acorda.
    // Isso acontece MESMO se o IME (Master Enable) estiver desligado.
    if (emu->cpu.halted && (ie_reg & if_reg)) {
        emu->cpu.halted = false;
    }

    // 3. Verificar Master Interrupt Enable (IME)
    // Se o IME estiver desligado (DI), não executamos o pulo da interrupção.
    if (!emu->cpu.interrupts_enabled) {
        return;
    }

    // 4. Verificar qual interrupção disparar (com prioridade)
    // Só processamos UMA interrupção por vez. A ordem dos IFs importa.
    
    uint8_t fired = ie_reg & if_reg;

    if (fired == 0) return; // Nenhuma interrupção habilitada disparou

    // --- V-BLANK (Bit 0) ---
    if (fired & INT_VBLANK) {
        // 1. Desabilita IME (para não ser interrompido por outra coisa agora)
        emu->cpu.interrupts_enabled = false;
        
        // 2. Limpa o bit em IF
        mmu_write_byte(emu, 0xFF0F, if_reg & ~INT_VBLANK);
        
        // 3. Salva onde a CPU estava (PC) na Stack
        service_interrupt_push(emu, emu->cpu.PC);
        
        // 4. Pula para o vetor
        emu->cpu.PC = VEC_VBLANK;
        return; // Sai da função
    }

    // --- LCD STAT (Bit 1) ---
    if (fired & INT_LCD_STAT) {
        emu->cpu.interrupts_enabled = false;
        mmu_write_byte(emu, 0xFF0F, if_reg & ~INT_LCD_STAT);
        service_interrupt_push(emu, emu->cpu.PC);
        emu->cpu.PC = VEC_LCD_STAT;
        return;
    }

    // --- TIMER (Bit 2) ---
    if (fired & INT_TIMER) {
        emu->cpu.interrupts_enabled = false;
        mmu_write_byte(emu, 0xFF0F, if_reg & ~INT_TIMER);
        service_interrupt_push(emu, emu->cpu.PC);
        emu->cpu.PC = VEC_TIMER;
        return;
    }

    // --- SERIAL (Bit 3) ---
    if (fired & INT_SERIAL) {
        emu->cpu.interrupts_enabled = false;
        mmu_write_byte(emu, 0xFF0F, if_reg & ~INT_SERIAL);
        service_interrupt_push(emu, emu->cpu.PC);
        emu->cpu.PC = VEC_SERIAL;
        return;
    }

    // --- JOYPAD (Bit 4) ---
    if (fired & INT_JOYPAD) {
        emu->cpu.interrupts_enabled = false;
        mmu_write_byte(emu, 0xFF0F, if_reg & ~INT_JOYPAD);
        service_interrupt_push(emu, emu->cpu.PC);
        emu->cpu.PC = VEC_JOYPAD;
        return;
    }
}