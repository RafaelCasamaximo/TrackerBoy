#include <cpu.h>

void initialize_cpu(CPU* cpu)
{
    cpu->A = 0;
    cpu->F = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->E = 0;
    cpu->H = 0;
    cpu->L = 0;

    cpu->PC = 0x0000; // Program Counter starts at 0
    cpu->SP = 0xFFFE; // Stack Pointer starts at top of memory

    cpu->halted = false;
    cpu->interrupts_enabled = false;
}

uint16_t get_af(const CPU* cpu) {
    return ((uint16_t)cpu->A << 8) | (cpu->F & 0xF0);
}

uint16_t get_bc(const CPU* cpu) {
    return ((uint16_t)cpu->B << 8) | cpu->C;
}

uint16_t get_de(const CPU* cpu) {
    return ((uint16_t)cpu->D << 8) | cpu->E;
}

uint16_t get_hl(const CPU* cpu) {
    return ((uint16_t)cpu->H << 8) | cpu->L;
}

void set_af(CPU* cpu, uint16_t val) {
    cpu->A = (val >> 8) & 0xFF;
    cpu->F = val & 0xF0; // Apenas os 4 bits superiores de F são usados
}

void set_bc(CPU* cpu, uint16_t val) {
    cpu->B = (val >> 8) & 0xFF;
    cpu->C = val & 0xFF;
}

void set_de(CPU* cpu, uint16_t val) {
    cpu->D = (val >> 8) & 0xFF;
    cpu->E = val & 0xFF;
}

void set_hl(CPU* cpu, uint16_t val) {
    cpu->H = (val >> 8) & 0xFF;
    cpu->L = val & 0xFF;
}