#include <ppu.h>
#include <emulator.h>
#include <mmu.h>       // Para atualizar LY (0xFF44)
#include <interrupts.h> // Para pedir VBLANK

// Constantes de Tempo do Game Boy (DMG)
#define CYCLES_PER_SCANLINE 456  // 114 M-Cycles * 4
#define SCREEN_HEIGHT 144        // Altura visível
#define SCANLINES_PER_FRAME 154  // 144 visíveis + 10 de VBlank

void ppu_init(Emulator* emu) {
    emu->ppu.cycle_counter = 0;
    emu->ppu.current_line = 0;
    emu->ppu.mode = PPU_MODE_OAM_SEARCH; // O hardware começa geralmente no modo 2
}

void ppu_step(Emulator* emu, int cpu_cycles) {
    // 1. Acumula os ciclos que a CPU gastou na última instrução
    emu->ppu.cycle_counter += cpu_cycles;

    // 2. Verifica se é hora de passar para a próxima linha (Scanline)
    if (emu->ppu.cycle_counter >= CYCLES_PER_SCANLINE) {
        
        // Reseta o contador (mantendo a sobra para precisão)
        emu->ppu.cycle_counter -= CYCLES_PER_SCANLINE;

        // Avança para a próxima linha
        emu->ppu.current_line++;

        // Atualiza o registrador LY na memória (0xFF44)
        // Isso é crucial: muitos jogos ficam lendo esse endereço em loop
        mmu_write_byte(emu, 0xFF44, emu->ppu.current_line);

        // --- Lógica de VBLANK ---
        
        // Se chegamos na linha 144, acabou a parte visível da tela
        if (emu->ppu.current_line == SCREEN_HEIGHT) {
            
            emu->ppu.mode = PPU_MODE_VBLANK;
            
            // DISPARA A INTERRUPÇÃO!
            // Isso vai tirar sua CPU do HALT
            request_interrupt(emu, INT_VBLANK);
            
            // TODO: Aqui é um bom lugar para avisar sua interface gráfica (SDL/OpenGL) 
            // que o quadro terminou e pode ser renderizado na tela do PC.
        }

        // Se passamos da linha 153, o frame acabou completamente
        else if (emu->ppu.current_line >= SCANLINES_PER_FRAME) {
            // Reinicia para o topo da tela
            emu->ppu.current_line = 0;
            emu->ppu.mode = PPU_MODE_OAM_SEARCH;
            
            // Atualiza LY para 0
            mmu_write_byte(emu, 0xFF44, 0);
        }
    }

    // TODO: Implementar a máquina de estados detalhada dos Modos 0, 2 e 3
    // para gerenciar acesso à memória e desenhar pixels no meio da linha.
    // Por enquanto, tratamos a linha como uma coisa única.
}