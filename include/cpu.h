#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * 
 * Operaçoes com registradores da CPU como 16 bits.
 */ 
// LENDO pares de 16 bits
// Pega o High, move 8 bits pra esquerda e soma com o Low
#define GET_AF(cpu) (((uint16_t)(cpu)->A << 8) | (cpu)->F)
#define GET_BC(cpu) (((uint16_t)(cpu)->B << 8) | (cpu)->C)
#define GET_DE(cpu) (((uint16_t)(cpu)->D << 8) | (cpu)->E)
#define GET_HL(cpu) (((uint16_t)(cpu)->H << 8) | (cpu)->L)

// ESCREVENDO em pares de 16 bits (recebe 'val' uint16_t)
// Pega os 8 bits superiores de 'val' para o High
// Pega os 8 bits inferiores de 'val' para o Low (usando máscara 0xFF)
#define SET_AF(cpu, val) do { \
    (cpu)->A = (uint8_t)((val) >> 8); \
    (cpu)->F = (uint8_t)((val) & 0xFF0); \ 
} while(0) 
// OBS: F tem uma pegadinha, os 4 bits inferiores são sempre zero no hardware real (0xF0).

#define SET_BC(cpu, val) do { \
    (cpu)->B = (uint8_t)((val) >> 8); \
    (cpu)->C = (uint8_t)((val) & 0xFF); \
} while(0)

#define SET_DE(cpu, val) do { \
    (cpu)->D = (uint8_t)((val) >> 8); \
    (cpu)->E = (uint8_t)((val) & 0xFF); \
} while(0)

#define SET_HL(cpu, val) do { \
    (cpu)->H = (uint8_t)((val) >> 8); \
    (cpu)->L = (uint8_t)((val) & 0xFF); \
} while(0)

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