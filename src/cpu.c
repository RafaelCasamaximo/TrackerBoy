#include <stdlib.h>
#include <stdio.h>


#include <cpu.h>
#include <emulator.h>
#include <instructions.h>

void initialize_cpu(Emulator* emu)
{
    // Registradores Principais (Valores Pós-BIOS)
    emu->cpu.A = 0x01;
    emu->cpu.F = 0xB0; // Z=1, N=0, H=1, C=1 (aproximadamente)
    emu->cpu.B = 0x00;
    emu->cpu.C = 0x13;
    emu->cpu.D = 0x00;
    emu->cpu.E = 0xD8;
    emu->cpu.H = 0x01;
    emu->cpu.L = 0x4D;
    // Ponteiros Críticos
    emu->cpu.SP = 0xFFFE; // Stack Pointer no topo da memória
    emu->cpu.PC = 0x0100; // Ponto de entrada do Cartucho (Pula a BIOS)

    // Estado Interno
    emu->cpu.halted = false;
    emu->cpu.interrupts_enabled = false; // A BIOS desliga interrupções antes de passar o controle
}

uint16_t get_af(Emulator* emu) {
    return ((uint16_t)emu->cpu.A << 8) | (emu->cpu.F & 0xF0);
}

uint16_t get_bc(Emulator* emu) {
    return ((uint16_t)emu->cpu.B << 8) | emu->cpu.C;
}

uint16_t get_de(Emulator* emu) {
    return ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
}

uint16_t get_hl(Emulator* emu) {
    return ((uint16_t)emu->cpu.H << 8) | emu->cpu.L;
}

void set_af(Emulator* emu, uint16_t val) {
    emu->cpu.A = (val >> 8) & 0xFF;
    emu->cpu.F = val & 0xF0; // Apenas os 4 bits superiores de F são usados
}

void set_bc(Emulator* emu, uint16_t val) {
    emu->cpu.B = (val >> 8) & 0xFF;
    emu->cpu.C = val & 0xFF;
}

void set_de(Emulator* emu, uint16_t val) {
    emu->cpu.D = (val >> 8) & 0xFF;
    emu->cpu.E = val & 0xFF;
}

void set_hl(Emulator* emu, uint16_t val) {
    emu->cpu.H = (val >> 8) & 0xFF;
    emu->cpu.L = val & 0xFF;
}
void flag_Z_set(Emulator* emu, bool value)
{
    if (value) {
        emu->cpu.F |= 0b10000000; // Set bit 7
    } else {
        emu->cpu.F &= ~0b10000000; // Clear bit 7
    }
}

void flag_N_set(Emulator* emu, bool value)
{
    if (value) {
        emu->cpu.F |= 0b01000000; // Set bit 6
    } else {
        emu->cpu.F &= ~0b01000000; // Clear bit 6
    }
}

void flag_H_set(Emulator* emu, bool value)
{
    if (value) {
        emu->cpu.F |= 0b00100000; // Set bit 5
    } else {
        emu->cpu.F &= ~0b00100000; // Clear bit 5
    }
}

void flag_C_set(Emulator* emu, bool value)
{
    if (value) {
        emu->cpu.F |= 0b00010000; // Set bit 4
    } else {
        emu->cpu.F &= ~0b00010000; // Clear bit 4
    }
}

void flag_Z_reset(Emulator* emu)
{
    emu->cpu.F &= ~0b10000000; // Clear bit 7
}

void flag_N_reset(Emulator* emu)
{
    emu->cpu.F &= ~0b01000000; // Clear bit 6
}

void flag_H_reset(Emulator* emu)
{
    emu->cpu.F &= ~0b00100000; // Clear bit 5
}

void flag_C_reset(Emulator* emu)
{
    emu->cpu.F &= ~0b00010000; // Clear bit 4
}

bool flag_Z_get(Emulator* emu)
{
    return (emu->cpu.F & 0b10000000) != 0;
}

bool flag_N_get(Emulator* emu)
{
    return (emu->cpu.F & 0b01000000) != 0;
}

bool flag_H_get(Emulator* emu)
{
    return (emu->cpu.F & 0b00100000) != 0;
}

bool flag_C_get(Emulator* emu)
{
    return (emu->cpu.F & 0b00010000) != 0;
}



uint8_t cpu_read_u8(Emulator* emu, uint16_t address) {
    return mmu_read_byte(emu, address);
}

uint16_t cpu_read_u16(Emulator* emu, uint16_t address) {
    // Little endian: low byte first, then high byte
    uint8_t low = cpu_read_u8(emu, address);
    uint8_t high = cpu_read_u8(emu, address + 1);
    return ((uint16_t)high << 8) | low;
}

uint8_t cpu_next_u8(Emulator* emu) {
    uint8_t value = cpu_read_u8(emu, emu->cpu.PC);
    emu->cpu.PC++;
    return value;
}

uint16_t cpu_next_u16(Emulator* emu) {
    // Little endian: low byte first, then high byte
    uint8_t low = cpu_next_u8(emu);
    uint8_t high = cpu_next_u8(emu);
    return ((uint16_t)high << 8) | low;
}

void cpu_step(Emulator* emu) {
    // 1. Verificar interrupções (Futuramente aqui você checa se deve acordar)
    // handle_interrupts(emu); 

    // 2. Se a CPU estiver em HALT, não fazemos fetch de nova instrução!
    if (emu->cpu.halted) {
        // A CPU "roda" ciclos internos, mas não avança o PC.
        // Aqui futuramente você somaria ciclos ao relógio do sistema.
        
        // Se você tiver interrupções implementadas:
        // if (interrupcao_pendente) { emu->cpu.halted = false; }
        
        return; // Sai da função sem executar nada novo
    }

    // --- Daqui para baixo é o código normal de execução ---

    // 3. LOG ANTES DE EXECUTAR
    if (emu->cpu.PC >= 0x0100 && emu->cpu.PC <= 0x0250) { // Aumentei o range para você ver o loop
        uint8_t opcode_peek = mmu_read_byte(emu, emu->cpu.PC);
        printf("PC:%04X | Op:%02X | SP:%04X\n", emu->cpu.PC, opcode_peek, emu->cpu.SP);
    }

    // 4. Fetch
    uint8_t opcode = cpu_next_u8(emu);

    // 5. Execute
    if (instructions[opcode] != NULL) {
        instructions[opcode](emu, opcode);
    } else {
        printf("CRITICAL: Opcode NULL %02X\n", opcode);
        exit(1);
    }
}