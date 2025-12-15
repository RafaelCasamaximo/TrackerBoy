#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct Emulator Emulator;

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

uint16_t get_af(Emulator* emu);
uint16_t get_bc(Emulator* emu);
uint16_t get_de(Emulator* emu);
uint16_t get_hl(Emulator* emu);

void set_af(Emulator* emu, uint16_t val);
void set_bc(Emulator* emu, uint16_t val);
void set_de(Emulator* emu, uint16_t val);
void set_hl(Emulator* emu, uint16_t val);


void initialize_cpu(Emulator* emu);

uint8_t cpu_read_u8(Emulator* emu, uint16_t address);
uint16_t cpu_read_u16(Emulator* emu, uint16_t address);