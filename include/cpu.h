#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Estrutura que representa a CPU do emulador.
 */

typedef struct {
    // Registradores de 8 bits
    uint8_t A; // Accumulator
    uint8_t F; // Flags
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;

    // Registradores de 16 bits nativos
    uint16_t SP; // Stack Pointer
    uint16_t PC; // Program Counter
    
    // Estado do emulador
    bool interrupts_enabled;
    bool halted;
} CPU;

uint16_t get_af(const CPU* cpu);
uint16_t get_bc(const CPU* cpu);
uint16_t get_de(const CPU* cpu);
uint16_t get_hl(const CPU* cpu);

void set_af(CPU* cpu, uint16_t val);
void set_bc(CPU* cpu, uint16_t val);
void set_de(CPU* cpu, uint16_t val);
void set_hl(CPU* cpu, uint16_t val);


void initialize_cpu(CPU* cpu);