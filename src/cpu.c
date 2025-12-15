#include <cpu.h>
#include <emulator.h>

void initialize_cpu(Emulator* emu)
{
    emu->cpu.A = 0;
    emu->cpu.F = 0;
    emu->cpu.B = 0;
    emu->cpu.C = 0;
    emu->cpu.D = 0;
    emu->cpu.E = 0;
    emu->cpu.H = 0;
    emu->cpu.L = 0;
    emu->cpu.PC = 0x0000; // Program Counter starts at 0
    emu->cpu.SP = 0xFFFE; // Stack Pointer starts at top of memory

    emu->cpu.halted = false;
    emu->cpu.interrupts_enabled = false;
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

uint8_t cpu_read_u8(Emulator* emu, uint16_t address) {
    // Reads a byte from the given address in memory
    // Replace this with actual memory access in a real emulator
    return 0;
}

uint8_t cpu_read_next_u8(Emulator* emu) {
    uint8_t value = cpu_read_u8(emu, emu->cpu.PC);
    emu->cpu.PC++;
    return value;
}

uint16_t cpu_read_u16(Emulator* emu, uint16_t address) {
    // Little endian: low byte first, then high byte
    uint8_t low = cpu_read_u8(emu, address);
    uint8_t high = cpu_read_u8(emu, address + 1);
    return ((uint16_t)high << 8) | low;
}