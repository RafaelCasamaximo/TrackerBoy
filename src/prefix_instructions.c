#include "prefix_instructions.h"
#include "cpu.h"
#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>

// Como estamos usando decodificação dinâmica, a tabela de ponteiros
// torna-se desnecessária, mas mantemos a declaração para compatibilidade.
InstructionFunc cb_instructions[256];

// --- 1. HELPERS DE ACESSO A REGISTRADORES (Abstração) ---

/**
 * Lê o valor do operando baseado nos 3 últimos bits do opcode.
 * 0=B, 1=C, 2=D, 3=E, 4=H, 5=L, 6=(HL), 7=A
 */
static uint8_t cb_read_operand(Emulator* emu, uint8_t reg_index) {
    switch (reg_index) {
        case 0: return emu->cpu.B;
        case 1: return emu->cpu.C;
        case 2: return emu->cpu.D;
        case 3: return emu->cpu.E;
        case 4: return emu->cpu.H;
        case 5: return emu->cpu.L;
        case 6: return mmu_read_byte(emu, get_hl(emu)); // Acesso à memória (HL)
        case 7: return emu->cpu.A;
        default: return 0; // Unreachable
    }
}

/**
 * Escreve o valor de volta no registrador ou memória.
 * Ignora escrita se for operação BIT (tratado no handler).
 */
static void cb_write_operand(Emulator* emu, uint8_t reg_index, uint8_t value) {
    switch (reg_index) {
        case 0: emu->cpu.B = value; break;
        case 1: emu->cpu.C = value; break;
        case 2: emu->cpu.D = value; break;
        case 3: emu->cpu.E = value; break;
        case 4: emu->cpu.H = value; break;
        case 5: emu->cpu.L = value; break;
        case 6: mmu_write_byte(emu, get_hl(emu), value); break; // Escrita em memória
        case 7: emu->cpu.A = value; break;
    }
}

// --- 2. HELPERS LÓGICOS (Matemática Pura) ---

// RLC: Rotate Left Circular (Bit 7 -> Carry e Bit 0)
static uint8_t helper_rlc(Emulator* emu, uint8_t value) {
    uint8_t bit7 = (value & 0x80) >> 7;
    uint8_t result = (value << 1) | bit7;
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit7);
    return result;
}

// RRC: Rotate Right Circular (Bit 0 -> Carry e Bit 7)
static uint8_t helper_rrc(Emulator* emu, uint8_t value) {
    uint8_t bit0 = value & 0x01;
    uint8_t result = (value >> 1) | (bit0 << 7);
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit0);
    return result;
}

// RL: Rotate Left through Carry (Bit 7 -> Carry, Old Carry -> Bit 0)
static uint8_t helper_rl(Emulator* emu, uint8_t value) {
    uint8_t old_carry = flag_C_get(emu) ? 1 : 0;
    uint8_t bit7 = (value & 0x80) >> 7;
    uint8_t result = (value << 1) | old_carry;
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit7);
    return result;
}

// RR: Rotate Right through Carry (Bit 0 -> Carry, Old Carry -> Bit 7)
static uint8_t helper_rr(Emulator* emu, uint8_t value) {
    uint8_t old_carry = flag_C_get(emu) ? 1 : 0;
    uint8_t bit0 = value & 0x01;
    uint8_t result = (value >> 1) | (old_carry << 7);
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit0);
    return result;
}

// SLA: Shift Left Arithmetic (Bit 7 -> Carry, 0 -> Bit 0)
static uint8_t helper_sla(Emulator* emu, uint8_t value) {
    uint8_t bit7 = (value & 0x80) >> 7;
    uint8_t result = value << 1;
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit7);
    return result;
}

// SRA: Shift Right Arithmetic (Bit 0 -> Carry, Mantém Bit 7)
static uint8_t helper_sra(Emulator* emu, uint8_t value) {
    uint8_t bit0 = value & 0x01;
    uint8_t bit7 = value & 0x80; // Sinal
    uint8_t result = (value >> 1) | bit7;
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit0);
    return result;
}

// SWAP: Troca nibbles (High <-> Low)
static uint8_t helper_swap(Emulator* emu, uint8_t value) {
    uint8_t low = value & 0x0F;
    uint8_t high = value & 0xF0;
    uint8_t result = (low << 4) | (high >> 4);
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, false);
    return result;
}

// SRL: Shift Right Logical (Bit 0 -> Carry, 0 -> Bit 7)
static uint8_t helper_srl(Emulator* emu, uint8_t value) {
    uint8_t bit0 = value & 0x01;
    uint8_t result = value >> 1;
    
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, bit0);
    return result;
}

// BIT: Testa bit N (Z setado se bit for 0. N=0, H=1)
static void helper_bit(Emulator* emu, uint8_t value, uint8_t bit) {
    bool is_zero = (value & (1 << bit)) == 0;
    flag_Z_set(emu, is_zero);
    flag_N_set(emu, false);
    flag_H_set(emu, true); // BIT sempre seta H=1 no Game Boy
}

// RES: Reseta bit N para 0
static uint8_t helper_res(Emulator* emu, uint8_t value, uint8_t bit) {
    return value & ~(1 << bit);
}

// SET: Seta bit N para 1
static uint8_t helper_set(Emulator* emu, uint8_t value, uint8_t bit) {
    return value | (1 << bit);
}

// --- 3. INICIALIZAÇÃO E HANDLER PRINCIPAL ---

void initialize_cb_table(void) {
    // Com a abordagem de decodificação dinâmica abaixo, 
    // não precisamos preencher o array de 256 funções.
    // O handler principal fará o roteamento sozinho.
}

void cb_prefix_handler(Emulator* emu) {
    // 1. Lê o opcode CB
    uint8_t cb_opcode = cpu_next_u8(emu);
    
    // 2. Decodifica os componentes do opcode (xxyyyzzz)
    // x (bits 6-7): Grupo de operação (Rotates, BIT, RES, SET)
    // y (bits 3-5): Opcode Específico ou Índice do Bit (0-7)
    // z (bits 0-2): Índice do Registrador (B, C, D, E, H, L, HL, A)
    
    uint8_t x = (cb_opcode >> 6) & 0x03;
    uint8_t y = (cb_opcode >> 3) & 0x07;
    uint8_t z = cb_opcode & 0x07;
    
    // 3. Lê o valor atual do registrador alvo
    uint8_t val = cb_read_operand(emu, z);
    uint8_t result = val; // Default
    
    // 4. Executa a operação baseada no Grupo (x)
    switch (x) {
        case 0: // Bloco 0x00 - 0x3F: Rotates e Shifts
            switch (y) {
                case 0: result = helper_rlc(emu, val); break;  // RLC
                case 1: result = helper_rrc(emu, val); break;  // RRC
                case 2: result = helper_rl(emu, val); break;   // RL
                case 3: result = helper_rr(emu, val); break;   // RR
                case 4: result = helper_sla(emu, val); break;  // SLA
                case 5: result = helper_sra(emu, val); break;  // SRA
                case 6: result = helper_swap(emu, val); break; // SWAP
                case 7: result = helper_srl(emu, val); break;  // SRL
            }
            cb_write_operand(emu, z, result);
            break;
            
        case 1: // Bloco 0x40 - 0x7F: BIT (Teste de bit)
            // 'y' representa qual bit testar (0-7)
            helper_bit(emu, val, y);
            // NOTA: BIT apenas afeta flags, não escreve de volta no registrador
            break;
            
        case 2: // Bloco 0x80 - 0xBF: RES (Reset bit)
            // 'y' representa qual bit resetar (0-7)
            result = helper_res(emu, val, y);
            cb_write_operand(emu, z, result);
            break;
            
        case 3: // Bloco 0xC0 - 0xFF: SET (Set bit)
            // 'y' representa qual bit setar (0-7)
            result = helper_set(emu, val, y);
            cb_write_operand(emu, z, result);
            break;
    }
    
    // Opcional: Adicionar penalidade de ciclos para operações HL aqui
    // Se z == 6 (HL), operações costumam levar mais ciclos.
}