#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <emulator.h> // Para ter acesso a struct Emulator e ao typedef InstructionFunc

// Definição do tipo de função de instrução CB
typedef void (*InstructionFunc)(Emulator* emu, uint16_t opcode);

// Tabela de ponteiros para funções CB (mantida para compatibilidade)
extern InstructionFunc cb_instructions[256];

/**
 * Inicializa a tabela de instruções CB.
 * (No seu modelo, não faz nada, mas mantida para compatibilidade.)
 */
void initialize_cb_table(void);

/**
 * Handler principal do Prefixo CB.
 * Esta função deve ser associada ao opcode 0xCB na tabela principal.
 * Ela busca o próximo byte (o operando) e executa a função correspondente na tabela cb_instructions.
 */
void cb_prefix_handler(Emulator* emu);

/**
 * Helper para decodificar qual registrador a instrução CB deve afetar.
 * As instruções CB usam os 3 bits menos significativos para definir o alvo:
 * 000 = B, 001 = C, 010 = D, 011 = E, 100 = H, 101 = L, 110 = (HL), 111 = A
 */
typedef enum {
    REG_B = 0,
    REG_C = 1,
    REG_D = 2,
    REG_E = 3,
    REG_H = 4,
    REG_L = 5,
    REG_HL_PTR = 6, // Atenção: Este opera na memória apontada por HL, não no registro HL
    REG_A = 7
} CB_RegisterTarget;

// --- Helpers de acesso a registradores (usados internamente) ---
static uint8_t cb_read_operand(Emulator* emu, uint8_t reg_index);
static void cb_write_operand(Emulator* emu, uint8_t reg_index, uint8_t value);

// --- Helpers lógicos para operações CB (usados internamente) ---
static uint8_t helper_rlc(Emulator* emu, uint8_t value);
static uint8_t helper_rrc(Emulator* emu, uint8_t value);
static uint8_t helper_rl(Emulator* emu, uint8_t value);
static uint8_t helper_rr(Emulator* emu, uint8_t value);
static uint8_t helper_sla(Emulator* emu, uint8_t value);
static uint8_t helper_sra(Emulator* emu, uint8_t value);
static uint8_t helper_swap(Emulator* emu, uint8_t value);
static uint8_t helper_srl(Emulator* emu, uint8_t value);
static void helper_bit(Emulator* emu, uint8_t value, uint8_t bit);
static uint8_t helper_res(Emulator* emu, uint8_t value, uint8_t bit);
static uint8_t helper_set(Emulator* emu, uint8_t value, uint8_t bit);

// Protótipo da função de crash específica para CB (para debug mais preciso)
void cb_not_implemented(Emulator* emu);
