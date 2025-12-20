#include "instructions.h"
#include "cpu.h"
#include "mmu.h"
#include "emulator.h"
#include <stdlib.h>
#include <stdio.h>

// --- HELPERS INTERNOS (Para encapsular lógica de Flags) ---

// Helper: Empilha 16 bits na Stack
static void push_u16(Emulator* emu, uint16_t value) {
    uint8_t hi = (value >> 8) & 0xFF;
    uint8_t lo = value & 0xFF;
    
    // Decrementa SP e escreve High
    emu->cpu.SP--;
    mmu_write_byte(emu, emu->cpu.SP, hi);
    
    // Decrementa SP e escreve Low
    emu->cpu.SP--;
    mmu_write_byte(emu, emu->cpu.SP, lo);
}

// Helper: Desempilha 16 bits da Stack
static uint16_t pop_u16(Emulator* emu) {
    uint8_t lo = mmu_read_byte(emu, emu->cpu.SP);
    emu->cpu.SP++;
    
    uint8_t hi = mmu_read_byte(emu, emu->cpu.SP);
    emu->cpu.SP++;
    
    return ((uint16_t)hi << 8) | lo;
}

// Helper: OR A, n8
static void or_n8(Emulator* emu, uint8_t value) {
    emu->cpu.A |= value;
    
    flag_Z_set(emu, emu->cpu.A == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, false);
}

// Helper: CP A, n8 (Compare)
// Basicamente um SUB que não salva o resultado em A
static void cp_n8(Emulator* emu, uint8_t value) {
    uint8_t a = emu->cpu.A;
    int result = a - value;

    flag_Z_set(emu, (result & 0xFF) == 0);
    flag_N_set(emu, true); // CP é uma subtração, então N=1
    // Half Carry (Borrow do bit 4)
    flag_H_set(emu, (a & 0x0F) < (value & 0x0F));
    // Carry (Borrow do bit 8 / Underflow)
    flag_C_set(emu, a < value);
    
    // NOTA: Não atualizamos emu->cpu.A!
}

// Helper: AND A, n8
static void and_n8(Emulator* emu, uint8_t value) {
    emu->cpu.A &= value;
    
    flag_Z_set(emu, emu->cpu.A == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, true);  // No Game Boy, AND sempre define H = 1
    flag_C_set(emu, false);
}

// Helper: XOR A, n8
static void xor_n8(Emulator* emu, uint8_t value) {
    emu->cpu.A ^= value;
    
    flag_Z_set(emu, emu->cpu.A == 0);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, false);
}

// Helper: SUB n8 (Subtração simples)
static void sub_n8(Emulator* emu, uint8_t value) {
    uint8_t a = emu->cpu.A;
    int result = a - value;

    flag_Z_set(emu, (result & 0xFF) == 0);
    flag_N_set(emu, true); // Subtração sempre seta N
    // Half Carry: Se o nibble de A for menor que o nibble do valor, houve borrow do bit 4
    flag_H_set(emu, (a & 0x0F) < (value & 0x0F));
    // Carry: Se A for menor que o valor, houve underflow
    flag_C_set(emu, a < value);

    emu->cpu.A = (uint8_t)result;
}

// Helper: SBC A, n8 (Subtração com Carry)
static void sbc_a_n8(Emulator* emu, uint8_t value) {
    uint8_t a = emu->cpu.A;
    uint8_t carry = flag_C_get(emu) ? 1 : 0;
    int result = a - value - carry;

    flag_Z_set(emu, (result & 0xFF) == 0);
    flag_N_set(emu, true);
    
    // Cálculo preciso de Borrow para H e C em SBC:
    // H: ((A & 0xF) - (val & 0xF) - c) < 0
    flag_H_set(emu, ((a & 0x0F) - (value & 0x0F) - carry) < 0);
    // C: Resultado menor que zero (Underflow)
    flag_C_set(emu, result < 0);

    emu->cpu.A = (uint8_t)result;
}

// Helper: ADD A, n8 (Soma simples)
static void add_a_n8(Emulator* emu, uint8_t value) {
    uint16_t result = emu->cpu.A + value;

    flag_Z_set(emu, (result & 0xFF) == 0);
    flag_N_set(emu, false);
    // Half Carry: Verifica se houve overflow do bit 3 para o 4
    flag_H_set(emu, ((emu->cpu.A & 0x0F) + (value & 0x0F)) > 0x0F);
    // Carry: Verifica se houve overflow do bit 7 (resultado > 255)
    flag_C_set(emu, result > 0xFF);

    emu->cpu.A = (uint8_t)result;
}

// Helper: ADC A, n8 (Soma com Carry)
static void adc_a_n8(Emulator* emu, uint8_t value) {
    uint8_t carry_in = flag_C_get(emu) ? 1 : 0;
    uint16_t result = emu->cpu.A + value + carry_in;

    flag_Z_set(emu, (result & 0xFF) == 0);
    flag_N_set(emu, false);
    
    // Half Carry: Soma dos nibbles inferiores + carry > 0xF ?
    flag_H_set(emu, ((emu->cpu.A & 0x0F) + (value & 0x0F) + carry_in) > 0x0F);
    // Carry: Soma total > 0xFF ?
    flag_C_set(emu, result > 0xFF);

    emu->cpu.A = (uint8_t)result;
}

// Incrementa valor de 8 bits e ajusta Z, N, H
static uint8_t inc_n8(Emulator* emu, uint8_t value) {
    uint8_t result = value + 1;
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, false);
    // Half Carry: Overflow do bit 3 para 4
    flag_H_set(emu, (value & 0x0F) == 0x0F);
    return result;
}

// Decrementa valor de 8 bits e ajusta Z, N, H
static uint8_t dec_n8(Emulator* emu, uint8_t value) {
    uint8_t result = value - 1;
    flag_Z_set(emu, result == 0);
    flag_N_set(emu, true);
    // Half Carry: Borrow do bit 4
    flag_H_set(emu, (value & 0x0F) == 0x00);
    return result;
}

// Soma 16 bits ao HL e ajusta N, H, C (Z preservado)
static void add_hl_n16(Emulator* emu, uint16_t value) {
    uint16_t hl = get_hl(emu);
    uint32_t result = hl + value;
    
    flag_N_set(emu, false);
    // Half Carry em 16 bits acontece no bit 11 -> 12
    flag_H_set(emu, ((hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF);
    // Carry no bit 15 -> 16
    flag_C_set(emu, result > 0xFFFF);
    
    set_hl(emu, (uint16_t)result);
}

// --- IMPLEMENTAÇÃO DAS INSTRUÇÕES ---

void op_notImplemented(Emulator* emu, uint16_t opcode) {
    // PC - 1 porque o fetch já avançou
    printf("CRASH: Opcode nao implementado 0x%02X em PC: 0x%04X\n", opcode, emu->cpu.PC - 1);
    exit(1);
}

// 0x00: NOP
void opcode_00(Emulator* emu, uint16_t opcode) {
    (void)emu; (void)opcode; // Evita warnings de unused parameter
}

// 0x01: LD BC, n16
void opcode_01(Emulator* emu, uint16_t opcode) {
    uint16_t value = cpu_next_u16(emu);
    set_bc(emu, value);
}

// 0x02: LD (BC), A
void opcode_02(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_bc(emu), emu->cpu.A);
}

// 0x03: INC BC (16-bit não afeta flags)
void opcode_03(Emulator* emu, uint16_t opcode) {
    set_bc(emu, get_bc(emu) + 1);
}

// 0x04: INC B
void opcode_04(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = inc_n8(emu, emu->cpu.B);
}

// 0x05: DEC B
void opcode_05(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = dec_n8(emu, emu->cpu.B);
}

// 0x06: LD B, n8
void opcode_06(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = cpu_next_u8(emu);
}

// 0x07: RLCA (Rotate Left A). Bit 7 -> Carry e Bit 0. ZERA Z.
void opcode_07(Emulator* emu, uint16_t opcode) {
    uint8_t old_bit7 = (emu->cpu.A & 0x80) >> 7;
    emu->cpu.A = (emu->cpu.A << 1) | old_bit7;
    
    flag_Z_set(emu, false); // Diferente do Z80, no GB é sempre 0 aqui
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, old_bit7);
}

// 0x08: LD (a16), SP
void opcode_08(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    // Grava Little Endian manualmente para clareza
    mmu_write_byte(emu, addr, emu->cpu.SP & 0xFF);
    mmu_write_byte(emu, addr + 1, (emu->cpu.SP >> 8) & 0xFF);
}

// 0x09: ADD HL, BC
void opcode_09(Emulator* emu, uint16_t opcode) {
    add_hl_n16(emu, get_bc(emu));
}

// 0x0A: LD A, (BC)
void opcode_0A(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = mmu_read_byte(emu, get_bc(emu));
}

// 0x0B: DEC BC
void opcode_0B(Emulator* emu, uint16_t opcode) {
    set_bc(emu, get_bc(emu) - 1);
}

// 0x0C: INC C
void opcode_0C(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = inc_n8(emu, emu->cpu.C);
}

// 0x0D: DEC C
void opcode_0D(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = dec_n8(emu, emu->cpu.C);
}

// 0x0E: LD C, n8
void opcode_0E(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = cpu_next_u8(emu);
}

// 0x0F: RRCA (Rotate Right A). Bit 0 -> Carry e Bit 7. ZERA Z.
void opcode_0F(Emulator* emu, uint16_t opcode) {
    uint8_t old_bit0 = emu->cpu.A & 0x01;
    emu->cpu.A = (emu->cpu.A >> 1) | (old_bit0 << 7);
    
    flag_Z_set(emu, false);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, old_bit0);
}

// 0x10: STOP
void opcode_10(Emulator* emu, uint16_t opcode) {
    cpu_next_u8(emu); // Consome o byte extra
    // TODO: Implementar pausa ou speed switch (GBC)
}

// 0x11: LD DE, n16
void opcode_11(Emulator* emu, uint16_t opcode) {
    uint16_t val = cpu_next_u16(emu);
    set_de(emu, val);
}

// 0x12: LD (DE), A
void opcode_12(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_de(emu), emu->cpu.A);
}

// 0x13: INC DE
void opcode_13(Emulator* emu, uint16_t opcode) {
    set_de(emu, get_de(emu) + 1);
}

// 0x14: INC D
void opcode_14(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = inc_n8(emu, emu->cpu.D);
}

// 0x15: DEC D
void opcode_15(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = dec_n8(emu, emu->cpu.D);
}

// 0x16: LD D, n8
void opcode_16(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = cpu_next_u8(emu);
}

// 0x17: RLA (Rotate Left Through Carry). Carry -> Bit 0, Bit 7 -> Carry.
void opcode_17(Emulator* emu, uint16_t opcode) {
    uint8_t old_carry = flag_C_get(emu) ? 1 : 0;
    uint8_t old_bit7 = (emu->cpu.A & 0x80) >> 7;
    
    emu->cpu.A = (emu->cpu.A << 1) | old_carry;
    
    flag_Z_set(emu, false);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, old_bit7);
}

// 0x18: JR n8 (Jump Relative)
void opcode_18(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    emu->cpu.PC += offset;
}

// 0x19: ADD HL, DE
void opcode_19(Emulator* emu, uint16_t opcode) {
    add_hl_n16(emu, get_de(emu));
}

// 0x1A: LD A, (DE)
void opcode_1A(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = mmu_read_byte(emu, get_de(emu));
}

// 0x1B: DEC DE
void opcode_1B(Emulator* emu, uint16_t opcode) {
    set_de(emu, get_de(emu) - 1);
}

// 0x1C: INC E
void opcode_1C(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = inc_n8(emu, emu->cpu.E);
}

// 0x1D: DEC E
void opcode_1D(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = dec_n8(emu, emu->cpu.E);
}

// 0x1E: LD E, n8
void opcode_1E(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = cpu_next_u8(emu);
}

// 0x1F: RRA (Rotate Right Through Carry)
void opcode_1F(Emulator* emu, uint16_t opcode) {
    uint8_t old_carry = flag_C_get(emu) ? 0x80 : 0;
    uint8_t old_bit0 = emu->cpu.A & 0x01;
    
    emu->cpu.A = (emu->cpu.A >> 1) | old_carry;
    
    flag_Z_set(emu, false);
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, old_bit0);
}

// 0x20: JR NZ, n8
void opcode_20(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if (!flag_Z_get(emu)) {
        emu->cpu.PC += offset;
    }
}

// 0x21: LD HL, n16
void opcode_21(Emulator* emu, uint16_t opcode) {
    uint16_t val = cpu_next_u16(emu);
    set_hl(emu, val);
}

// 0x22: LD (HL+), A
void opcode_22(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    mmu_write_byte(emu, hl, emu->cpu.A);
    set_hl(emu, hl + 1);
}

// 0x23: INC HL
void opcode_23(Emulator* emu, uint16_t opcode) {
    set_hl(emu, get_hl(emu) + 1);
}

// 0x24: INC H
void opcode_24(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = inc_n8(emu, emu->cpu.H);
}

// 0x25: DEC H
void opcode_25(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = dec_n8(emu, emu->cpu.H);
}

// 0x26: LD H, n8
void opcode_26(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = cpu_next_u8(emu);
}

// 0x27: DAA (Decimal Adjust Accumulator) - Essencial para contagem BCD
void opcode_27(Emulator* emu, uint16_t opcode) {
    int a = emu->cpu.A;
    if (!flag_N_get(emu)) {
        if (flag_H_get(emu) || (a & 0x0F) > 9) a += 0x06;
        if (flag_C_get(emu) || a > 0x9F) a += 0x60;
    } else {
        if (flag_H_get(emu)) a -= 0x06;
        if (flag_C_get(emu)) a -= 0x60;
    }
    
    flag_H_set(emu, false);
    flag_C_set(emu, (a & 0x100) != 0 || flag_C_get(emu)); // Carry persiste se já estava setado? (verificar specs, mas geralmente é OR)
    a &= 0xFF;
    flag_Z_set(emu, a == 0);
    emu->cpu.A = (uint8_t)a;
}

// 0x28: JR Z, n8
void opcode_28(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if (flag_Z_get(emu)) {
        emu->cpu.PC += offset;
    }
}

// 0x29: ADD HL, HL
void opcode_29(Emulator* emu, uint16_t opcode) {
    add_hl_n16(emu, get_hl(emu));
}

// 0x2A: LD A, (HL+)
void opcode_2A(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    emu->cpu.A = mmu_read_byte(emu, hl);
    set_hl(emu, hl + 1);
}

// 0x2B: DEC HL
void opcode_2B(Emulator* emu, uint16_t opcode) {
    set_hl(emu, get_hl(emu) - 1);
}

// 0x2C: INC L
void opcode_2C(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = inc_n8(emu, emu->cpu.L);
}

// 0x2D: DEC L
void opcode_2D(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = dec_n8(emu, emu->cpu.L);
}

// 0x2E: LD L, n8
void opcode_2E(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = cpu_next_u8(emu);
}

// 0x2F: CPL (Inverte bits de A)
void opcode_2F(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = ~emu->cpu.A;
    flag_N_set(emu, true);
    flag_H_set(emu, true);
}


// 0x30: JR NC, n8 (Jump Relative if Not Carry)
void opcode_30(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if (!flag_C_get(emu)) {
        emu->cpu.PC += offset;
    }
}

// 0x31: LD SP, n16 (Inicializa o Stack Pointer)
void opcode_31(Emulator* emu, uint16_t opcode) {
    emu->cpu.SP = cpu_next_u16(emu);
}

// 0x32: LD (HL-), A (Escreve A em (HL) e decrementa HL)
void opcode_32(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    mmu_write_byte(emu, hl, emu->cpu.A);
    set_hl(emu, hl - 1);
}

// 0x33: INC SP (16-bit, sem flags)
void opcode_33(Emulator* emu, uint16_t opcode) {
    emu->cpu.SP++;
}

// 0x34: INC (HL) - Incrementa valor NA MEMÓRIA
// Cuidado: Read -> Modify -> Write
void opcode_34(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    uint8_t val = mmu_read_byte(emu, hl);
    val = inc_n8(emu, val); // Usa seu helper existente para flags (Z, N, H)
    mmu_write_byte(emu, hl, val);
}

// 0x35: DEC (HL) - Decrementa valor NA MEMÓRIA
void opcode_35(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    uint8_t val = mmu_read_byte(emu, hl);
    val = dec_n8(emu, val); // Usa seu helper existente
    mmu_write_byte(emu, hl, val);
}

// 0x36: LD (HL), n8 (Carrega imediato para memória)
void opcode_36(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    uint8_t val = cpu_next_u8(emu);
    mmu_write_byte(emu, hl, val);
}

// 0x37: SCF (Set Carry Flag)
void opcode_37(Emulator* emu, uint16_t opcode) {
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, true);
    // Z é preservado
}

// 0x38: JR C, n8 (Jump Relative if Carry)
void opcode_38(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if (flag_C_get(emu)) {
        emu->cpu.PC += offset;
    }
}

// 0x39: ADD HL, SP (Adiciona SP ao HL, 16-bit)
void opcode_39(Emulator* emu, uint16_t opcode) {
    add_hl_n16(emu, emu->cpu.SP); // Usa seu helper existente
}

// 0x3A: LD A, (HL-) (Lê de (HL) para A e decrementa HL)
void opcode_3A(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    emu->cpu.A = mmu_read_byte(emu, hl);
    set_hl(emu, hl - 1);
}

// 0x3B: DEC SP (16-bit, sem flags)
void opcode_3B(Emulator* emu, uint16_t opcode) {
    emu->cpu.SP--;
}

// 0x3C: INC A
void opcode_3C(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = inc_n8(emu, emu->cpu.A);
}

// 0x3D: DEC A
void opcode_3D(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = dec_n8(emu, emu->cpu.A);
}

// 0x3E: LD A, n8
void opcode_3E(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = cpu_next_u8(emu);
}

// 0x3F: CCF (Complement Carry Flag - Inverte C)
void opcode_3F(Emulator* emu, uint16_t opcode) {
    flag_N_set(emu, false);
    flag_H_set(emu, false);
    flag_C_set(emu, !flag_C_get(emu)); // Inverte
    // Z é preservado
}

// --- BLOCO 0x40 - 0x4F: Loads para B e C ---

// 0x40: LD B, B (Nop efetivo)
void opcode_40(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.B;
}

// 0x41: LD B, C
void opcode_41(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.C;
}

// 0x42: LD B, D
void opcode_42(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.D;
}

// 0x43: LD B, E
void opcode_43(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.E;
}

// 0x44: LD B, H
void opcode_44(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.H;
}

// 0x45: LD B, L
void opcode_45(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.L;
}

// 0x46: LD B, (HL) - Lê da memória
void opcode_46(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = mmu_read_byte(emu, get_hl(emu));
}

// 0x47: LD B, A
void opcode_47(Emulator* emu, uint16_t opcode) {
    emu->cpu.B = emu->cpu.A;
}

// 0x48: LD C, B
void opcode_48(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.B;
}

// 0x49: LD C, C (Nop efetivo)
void opcode_49(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.C;
}

// 0x4A: LD C, D
void opcode_4A(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.D;
}

// 0x4B: LD C, E
void opcode_4B(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.E;
}

// 0x4C: LD C, H
void opcode_4C(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.H;
}

// 0x4D: LD C, L
void opcode_4D(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.L;
}

// 0x4E: LD C, (HL) - Lê da memória
void opcode_4E(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = mmu_read_byte(emu, get_hl(emu));
}

// 0x4F: LD C, A
void opcode_4F(Emulator* emu, uint16_t opcode) {
    emu->cpu.C = emu->cpu.A;
}

// --- BLOCO 0x50 - 0x5F: Loads para D e E ---

// 0x50: LD D, B
void opcode_50(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.B;
}

// 0x51: LD D, C
void opcode_51(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.C;
}

// 0x52: LD D, D (Nop efetivo)
void opcode_52(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.D;
}

// 0x53: LD D, E
void opcode_53(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.E;
}

// 0x54: LD D, H
void opcode_54(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.H;
}

// 0x55: LD D, L
void opcode_55(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.L;
}

// 0x56: LD D, (HL) - Lê da memória
void opcode_56(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = mmu_read_byte(emu, get_hl(emu));
}

// 0x57: LD D, A
void opcode_57(Emulator* emu, uint16_t opcode) {
    emu->cpu.D = emu->cpu.A;
}

// 0x58: LD E, B
void opcode_58(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.B;
}

// 0x59: LD E, C
void opcode_59(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.C;
}

// 0x5A: LD E, D
void opcode_5A(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.D;
}

// 0x5B: LD E, E (Nop efetivo)
void opcode_5B(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.E;
}

// 0x5C: LD E, H
void opcode_5C(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.H;
}

// 0x5D: LD E, L
void opcode_5D(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.L;
}

// 0x5E: LD E, (HL) - Lê da memória
void opcode_5E(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = mmu_read_byte(emu, get_hl(emu));
}

// 0x5F: LD E, A
void opcode_5F(Emulator* emu, uint16_t opcode) {
    emu->cpu.E = emu->cpu.A;
}

// --- BLOCO 0x60 - 0x6F: Loads para H e L ---

// 0x60: LD H, B
void opcode_60(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.B;
}

// 0x61: LD H, C
void opcode_61(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.C;
}

// 0x62: LD H, D
void opcode_62(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.D;
}

// 0x63: LD H, E
void opcode_63(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.E;
}

// 0x64: LD H, H (Nop efetivo)
void opcode_64(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.H;
}

// 0x65: LD H, L
void opcode_65(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.L;
}

// 0x66: LD H, (HL) - Lê da memória
void opcode_66(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = mmu_read_byte(emu, get_hl(emu));
}

// 0x67: LD H, A
void opcode_67(Emulator* emu, uint16_t opcode) {
    emu->cpu.H = emu->cpu.A;
}

// 0x68: LD L, B
void opcode_68(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.B;
}

// 0x69: LD L, C
void opcode_69(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.C;
}

// 0x6A: LD L, D
void opcode_6A(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.D;
}

// 0x6B: LD L, E
void opcode_6B(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.E;
}

// 0x6C: LD L, H
void opcode_6C(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.H;
}

// 0x6D: LD L, L (Nop efetivo)
void opcode_6D(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.L;
}

// 0x6E: LD L, (HL) - Lê da memória
void opcode_6E(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = mmu_read_byte(emu, get_hl(emu));
}

// 0x6F: LD L, A
void opcode_6F(Emulator* emu, uint16_t opcode) {
    emu->cpu.L = emu->cpu.A;
}

// --- BLOCO 0x70 - 0x7F: Loads (Escrita em Memória) e HALT ---

// 0x70: LD (HL), B
void opcode_70(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.B);
}

// 0x71: LD (HL), C
void opcode_71(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.C);
}

// 0x72: LD (HL), D
void opcode_72(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.D);
}

// 0x73: LD (HL), E
void opcode_73(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.E);
}

// 0x74: LD (HL), H
void opcode_74(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.H);
}

// 0x75: LD (HL), L
void opcode_75(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.L);
}

// 0x76: HALT - Pausa a CPU até ocorrer uma interrupção
void opcode_76(Emulator* emu, uint16_t opcode) {
    emu->cpu.halted = true;
    
    // Nota Técnica: Existe um "HALT Bug" no hardware original se 
    // interrupções estiverem desabilitadas (IME=0) mas houver flags pendentes.
    // Para um emulador inicial, apenas setar halted = true é suficiente.
}

// 0x77: LD (HL), A
void opcode_77(Emulator* emu, uint16_t opcode) {
    mmu_write_byte(emu, get_hl(emu), emu->cpu.A);
}

// 0x78: LD A, B
void opcode_78(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.B;
}

// 0x79: LD A, C
void opcode_79(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.C;
}

// 0x7A: LD A, D
void opcode_7A(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.D;
}

// 0x7B: LD A, E
void opcode_7B(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.E;
}

// 0x7C: LD A, H
void opcode_7C(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.H;
}

// 0x7D: LD A, L
void opcode_7D(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.L;
}

// 0x7E: LD A, (HL) - Lê da memória
void opcode_7E(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = mmu_read_byte(emu, get_hl(emu));
}

// 0x7F: LD A, A (Nop efetivo)
void opcode_7F(Emulator* emu, uint16_t opcode) {
    emu->cpu.A = emu->cpu.A;
}

// --- BLOCO 0x80 - 0x87: ADD A, r ---

// 0x80: ADD A, B
void opcode_80(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.B);
}

// 0x81: ADD A, C
void opcode_81(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.C);
}

// 0x82: ADD A, D
void opcode_82(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.D);
}

// 0x83: ADD A, E
void opcode_83(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.E);
}

// 0x84: ADD A, H
void opcode_84(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.H);
}

// 0x85: ADD A, L
void opcode_85(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.L);
}

// 0x86: ADD A, (HL) - Lê da memória e soma
void opcode_86(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    add_a_n8(emu, val);
}

// 0x87: ADD A, A
void opcode_87(Emulator* emu, uint16_t opcode) {
    add_a_n8(emu, emu->cpu.A);
}

// --- BLOCO 0x88 - 0x8F: ADC A, r (Add with Carry) ---

// 0x88: ADC A, B
void opcode_88(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.B);
}

// 0x89: ADC A, C
void opcode_89(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.C);
}

// 0x8A: ADC A, D
void opcode_8A(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.D);
}

// 0x8B: ADC A, E
void opcode_8B(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.E);
}

// 0x8C: ADC A, H
void opcode_8C(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.H);
}

// 0x8D: ADC A, L
void opcode_8D(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.L);
}

// 0x8E: ADC A, (HL)
void opcode_8E(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    adc_a_n8(emu, val);
}

// 0x8F: ADC A, A
void opcode_8F(Emulator* emu, uint16_t opcode) {
    adc_a_n8(emu, emu->cpu.A);
}

// --- BLOCO 0x90 - 0x97: SUB r (A = A - r) ---

// 0x90: SUB B
void opcode_90(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.B);
}

// 0x91: SUB C
void opcode_91(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.C);
}

// 0x92: SUB D
void opcode_92(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.D);
}

// 0x93: SUB E
void opcode_93(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.E);
}

// 0x94: SUB H
void opcode_94(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.H);
}

// 0x95: SUB L
void opcode_95(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.L);
}

// 0x96: SUB (HL)
void opcode_96(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    sub_n8(emu, val);
}

// 0x97: SUB A (Resultado sempre 0, Z=1)
void opcode_97(Emulator* emu, uint16_t opcode) {
    sub_n8(emu, emu->cpu.A);
}

// --- BLOCO 0x98 - 0x9F: SBC A, r (A = A - r - C) ---

// 0x98: SBC A, B
void opcode_98(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.B);
}

// 0x99: SBC A, C
void opcode_99(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.C);
}

// 0x9A: SBC A, D
void opcode_9A(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.D);
}

// 0x9B: SBC A, E
void opcode_9B(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.E);
}

// 0x9C: SBC A, H
void opcode_9C(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.H);
}

// 0x9D: SBC A, L
void opcode_9D(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.L);
}

// 0x9E: SBC A, (HL)
void opcode_9E(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    sbc_a_n8(emu, val);
}

// 0x9F: SBC A, A (Resultado será 0 ou -1 dependendo do Carry)
void opcode_9F(Emulator* emu, uint16_t opcode) {
    sbc_a_n8(emu, emu->cpu.A);
}

// --- BLOCO 0xA0 - 0xA7: AND r (A = A & r) ---

// 0xA0: AND B
void opcode_A0(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.B);
}

// 0xA1: AND C
void opcode_A1(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.C);
}

// 0xA2: AND D
void opcode_A2(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.D);
}

// 0xA3: AND E
void opcode_A3(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.E);
}

// 0xA4: AND H
void opcode_A4(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.H);
}

// 0xA5: AND L
void opcode_A5(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.L);
}

// 0xA6: AND (HL)
void opcode_A6(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    and_n8(emu, val);
}

// 0xA7: AND A
void opcode_A7(Emulator* emu, uint16_t opcode) {
    and_n8(emu, emu->cpu.A);
}

// --- BLOCO 0xA8 - 0xAF: XOR r (A = A ^ r) ---

// 0xA8: XOR B
void opcode_A8(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.B);
}

// 0xA9: XOR C
void opcode_A9(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.C);
}

// 0xAA: XOR D
void opcode_AA(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.D);
}

// 0xAB: XOR E
void opcode_AB(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.E);
}

// 0xAC: XOR H
void opcode_AC(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.H);
}

// 0xAD: XOR L
void opcode_AD(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.L);
}

// 0xAE: XOR (HL)
void opcode_AE(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    xor_n8(emu, val);
}

// 0xAF: XOR A (Resultado é sempre 0, Z=1. Muito usado para limpar A)
void opcode_AF(Emulator* emu, uint16_t opcode) {
    xor_n8(emu, emu->cpu.A);
}

// --- BLOCO 0xB0 - 0xB7: OR r (A = A | r) ---

// 0xB0: OR B
void opcode_B0(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.B);
}

// 0xB1: OR C
void opcode_B1(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.C);
}

// 0xB2: OR D
void opcode_B2(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.D);
}

// 0xB3: OR E
void opcode_B3(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.E);
}

// 0xB4: OR H
void opcode_B4(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.H);
}

// 0xB5: OR L
void opcode_B5(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.L);
}

// 0xB6: OR (HL)
void opcode_B6(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    or_n8(emu, val);
}

// 0xB7: OR A
void opcode_B7(Emulator* emu, uint16_t opcode) {
    or_n8(emu, emu->cpu.A);
}

// --- BLOCO 0xB8 - 0xBF: CP r (Flags = A - r) ---

// 0xB8: CP B
void opcode_B8(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.B);
}

// 0xB9: CP C
void opcode_B9(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.C);
}

// 0xBA: CP D
void opcode_BA(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.D);
}

// 0xBB: CP E
void opcode_BB(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.E);
}

// 0xBC: CP H
void opcode_BC(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.H);
}

// 0xBD: CP L
void opcode_BD(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.L);
}

// 0xBE: CP (HL)
void opcode_BE(Emulator* emu, uint16_t opcode) {
    uint8_t val = mmu_read_byte(emu, get_hl(emu));
    cp_n8(emu, val);
}

// 0xBF: CP A (Sempre Z=1, N=1, H=0, C=0)
void opcode_BF(Emulator* emu, uint16_t opcode) {
    cp_n8(emu, emu->cpu.A);
}

// --- BLOCO 0xC0 - 0xCF: Stack, Calls e Flow Control ---

// 0xC0: RET NZ (Retorna se Z == 0)
void opcode_C0(Emulator* emu, uint16_t opcode) {
    if (!flag_Z_get(emu)) {
        emu->cpu.PC = pop_u16(emu);
        // Ciclos extras são gastos aqui no hardware real
    }
}

// 0xC1: POP BC
void opcode_C1(Emulator* emu, uint16_t opcode) {
    set_bc(emu, pop_u16(emu));
}

// 0xC2: JP NZ, a16 (Pula se Z == 0)
void opcode_C2(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (!flag_Z_get(emu)) {
        emu->cpu.PC = addr;
    }
}

// 0xC3: JP a16 (Jump Incondicional)
void opcode_C3(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu); // Lê o endereço (PC já andou 2)
    emu->cpu.PC = addr;                 // Pula
}

// 0xC4: CALL NZ, a16 (Chama função se Z == 0)
void opcode_C4(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu); // Lê o destino (PC já andou 2)
    if (!flag_Z_get(emu)) {
        push_u16(emu, emu->cpu.PC); // Salva o endereço de retorno (próxima instr)
        emu->cpu.PC = addr;         // Pula
    }
}

// 0xC5: PUSH BC
void opcode_C5(Emulator* emu, uint16_t opcode) {
    push_u16(emu, get_bc(emu));
}

// 0xC6: ADD A, n8 (Imediato)
void opcode_C6(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    add_a_n8(emu, val); // Usa seu helper existente
}

// 0xC7: RST 00H (Call para endereço fixo 0x0000)
void opcode_C7(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0000;
}

// 0xC8: RET Z (Retorna se Z == 1)
void opcode_C8(Emulator* emu, uint16_t opcode) {
    if (flag_Z_get(emu)) {
        emu->cpu.PC = pop_u16(emu);
    }
}

// 0xC9: RET (Retorno Incondicional)
void opcode_C9(Emulator* emu, uint16_t opcode) {
    emu->cpu.PC = pop_u16(emu);
}

// 0xCA: JP Z, a16
void opcode_CA(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (flag_Z_get(emu)) {
        emu->cpu.PC = addr;
    }
}

// 0xCB: PREFIX CB (Bit Operations) - CRÍTICO
void opcode_CB(Emulator* emu, uint16_t opcode) {
    // O byte seguinte determina a operação na tabela estendida.
    uint8_t cb_op = cpu_next_u8(emu);
    
    // TODO: Implementar a tabela de instruções CB (RLC, BIT, RES, SET)
    printf("ERRO CRITICO: Instrucao PREFIX CB 0x%02X ainda nao implementada!\n", cb_op);
    exit(1);
}

// 0xCC: CALL Z, a16
void opcode_CC(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (flag_Z_get(emu)) {
        push_u16(emu, emu->cpu.PC);
        emu->cpu.PC = addr;
    }
}

// 0xCD: CALL a16 (Call Incondicional)
void opcode_CD(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = addr;
}

// 0xCE: ADC A, n8 (Imediato)
void opcode_CE(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    adc_a_n8(emu, val); // Usa helper existente
}

// 0xCF: RST 08H
void opcode_CF(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0008;
}

// --- BLOCO 0xD0 - 0xDF: Control Flow (Carry), Stack e RETI ---

// 0xD0: RET NC (Retorna se Carry == 0)
void opcode_D0(Emulator* emu, uint16_t opcode) {
    if (!flag_C_get(emu)) {
        emu->cpu.PC = pop_u16(emu);
        // Ciclos extras (condicional satisfeita)
    }
}

// 0xD1: POP DE
void opcode_D1(Emulator* emu, uint16_t opcode) {
    set_de(emu, pop_u16(emu));
}

// 0xD2: JP NC, a16 (Pula se Carry == 0)
void opcode_D2(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (!flag_C_get(emu)) {
        emu->cpu.PC = addr;
    }
}

// 0xD3: ILLEGAL / UNUSED
// No Z80 seria OUT (n), A. No Game Boy não existe.
// Mantemos como op_notImplemented na tabela.

// 0xD4: CALL NC, a16
void opcode_D4(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (!flag_C_get(emu)) {
        push_u16(emu, emu->cpu.PC);
        emu->cpu.PC = addr;
    }
}

// 0xD5: PUSH DE
void opcode_D5(Emulator* emu, uint16_t opcode) {
    push_u16(emu, get_de(emu));
}

// 0xD6: SUB n8 (Subtrai imediato de A)
void opcode_D6(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    sub_n8(emu, val); // Usa helper existente
}

// 0xD7: RST 10H (Call para 0x0010)
void opcode_D7(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0010;
}

// 0xD8: RET C (Retorna se Carry == 1)
void opcode_D8(Emulator* emu, uint16_t opcode) {
    if (flag_C_get(emu)) {
        emu->cpu.PC = pop_u16(emu);
    }
}

// 0xD9: RETI (Return from Interrupt) - CRÍTICO
void opcode_D9(Emulator* emu, uint16_t opcode) {
    emu->cpu.PC = pop_u16(emu);
    emu->cpu.interrupts_enabled = true; // Reabilita a Master Interrupt Flag (IME)
}

// 0xDA: JP C, a16
void opcode_DA(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (flag_C_get(emu)) {
        emu->cpu.PC = addr;
    }
}

// 0xDB: ILLEGAL / UNUSED

// 0xDC: CALL C, a16
void opcode_DC(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    if (flag_C_get(emu)) {
        push_u16(emu, emu->cpu.PC);
        emu->cpu.PC = addr;
    }
}

// 0xDD: ILLEGAL / UNUSED

// 0xDE: SBC A, n8 (Subtrai imediato com carry)
void opcode_DE(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    sbc_a_n8(emu, val); // Usa helper existente
}

// 0xDF: RST 18H
void opcode_DF(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0018;
}

// --- BLOCO 0xE0 - 0xEF: High Ram, Stack Math e Jumps ---

// 0xE0: LDH (n8), A -> Escreve A em (0xFF00 + n8)
// Usado para acesso rápido a I/O Ports e HRAM
void opcode_E0(Emulator* emu, uint16_t opcode) {
    uint8_t offset = cpu_next_u8(emu);
    uint16_t addr = 0xFF00 + offset;
    mmu_write_byte(emu, addr, emu->cpu.A);
}

// 0xE1: POP HL
void opcode_E1(Emulator* emu, uint16_t opcode) {
    set_hl(emu, pop_u16(emu));
}

// 0xE2: LD (C), A -> Escreve A em (0xFF00 + C)
// Muito usado para I/O quando o offset está no registrador C
void opcode_E2(Emulator* emu, uint16_t opcode) {
    uint16_t addr = 0xFF00 + emu->cpu.C;
    mmu_write_byte(emu, addr, emu->cpu.A);
}

// 0xE3: ILLEGAL

// 0xE4: ILLEGAL

// 0xE5: PUSH HL
void opcode_E5(Emulator* emu, uint16_t opcode) {
    push_u16(emu, get_hl(emu));
}

// 0xE6: AND n8 (Imediato)
void opcode_E6(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    and_n8(emu, val); // Usa helper existente
}

// 0xE7: RST 20H
void opcode_E7(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0020;
}

// 0xE8: ADD SP, e8 (Soma imediato com sinal ao SP) - CHATO DE IMPLEMENTAR
// Flags: Z=0, N=0. H e C são baseados no overflow do BYTE BAIXO (0xFF)
void opcode_E8(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu); // Valor com sinal!
    uint16_t sp = emu->cpu.SP;
    
    // Cálculo das Flags (baseado em unsigned 8-bit overflow)
    // H: Overflow do bit 3 para 4
    flag_H_set(emu, ((sp & 0x0F) + (offset & 0x0F)) > 0x0F);
    // C: Overflow do bit 7 para 8
    flag_C_set(emu, ((sp & 0xFF) + (offset & 0xFF)) > 0xFF);
    
    flag_Z_set(emu, false); // Z é sempre 0 aqui (estranho, mas é spec do GB)
    flag_N_set(emu, false);

    emu->cpu.SP = sp + offset;
}

// 0xE9: JP (HL) -> PC = HL
// Atenção: Não lê da memória, apenas copia o valor de HL para PC
void opcode_E9(Emulator* emu, uint16_t opcode) {
    emu->cpu.PC = get_hl(emu);
}

// 0xEA: LD (a16), A -> Escreve A em endereço absoluto de 16 bits
void opcode_EA(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    mmu_write_byte(emu, addr, emu->cpu.A);
}

// 0xEB: ILLEGAL

// 0xEC: ILLEGAL

// 0xED: ILLEGAL

// 0xEE: XOR n8 (Imediato)
void opcode_EE(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    xor_n8(emu, val); // Usa helper existente
}

// 0xEF: RST 28H
void opcode_EF(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0028;
}

// --- BLOCO 0xF0 - 0xFF: HRAM, Interrupts e Stack Complexo ---

// 0xF0: LDH A, (n8) -> Lê de (0xFF00 + n8) para A
// Semelhante ao E0, mas a direção é Memória -> Registrador
void opcode_F0(Emulator* emu, uint16_t opcode) {
    uint8_t offset = cpu_next_u8(emu);
    uint16_t addr = 0xFF00 + offset;
    emu->cpu.A = mmu_read_byte(emu, addr);
}

// 0xF1: POP AF
// CRÍTICO: Os bits 0-3 do registrador F devem ser SEMPRE zero.
void opcode_F1(Emulator* emu, uint16_t opcode) {
    uint16_t af = pop_u16(emu);
    emu->cpu.A = (af >> 8) & 0xFF;
    // Máscara 0xF0 para garantir que bits baixos de F sejam 0
    emu->cpu.F = (af & 0xF0); 
}

// 0xF2: LD A, (C) -> Lê de (0xFF00 + C) para A
// Endereçamento baseado no registrador C (High RAM)
void opcode_F2(Emulator* emu, uint16_t opcode) {
    uint16_t addr = 0xFF00 + emu->cpu.C;
    emu->cpu.A = mmu_read_byte(emu, addr);
}

// 0xF3: DI (Disable Interrupts)
// Desabilita a flag IME (Interrupt Master Enable) imediatamente.
void opcode_F3(Emulator* emu, uint16_t opcode) {
    emu->cpu.interrupts_enabled = false;
}

// 0xF4: ILLEGAL / UNUSED

// 0xF5: PUSH AF
void opcode_F5(Emulator* emu, uint16_t opcode) {
    // Garante máscara no F antes de empilhar, por segurança
    uint16_t af = (emu->cpu.A << 8) | (emu->cpu.F & 0xF0);
    push_u16(emu, af);
}

// 0xF6: OR n8 (Imediato)
void opcode_F6(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    or_n8(emu, val); // Usa helper existente
}

// 0xF7: RST 30H
void opcode_F7(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0030;
}

// 0xF8: LD HL, SP+e8
// Comportamento IDÊNTICO ao 0xE8 (ADD SP, e8) nas flags,
// mas o resultado vai para HL e SP permanece inalterado.
void opcode_F8(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu); // Signed!
    uint16_t sp = emu->cpu.SP;
    int result = sp + offset;

    flag_Z_set(emu, false);
    flag_N_set(emu, false);
    
    // Cálculo de H e C baseados no byte baixo (unsigned), igual ao opcode 0xE8
    flag_H_set(emu, ((sp & 0x0F) + (offset & 0x0F)) > 0x0F);
    flag_C_set(emu, ((sp & 0xFF) + (offset & 0xFF)) > 0xFF);

    set_hl(emu, (uint16_t)result);
}

// 0xF9: LD SP, HL
// Copia HL para SP. Não afeta flags.
void opcode_F9(Emulator* emu, uint16_t opcode) {
    emu->cpu.SP = get_hl(emu);
}

// 0xFA: LD A, (a16) -> Lê byte de endereço absoluto para A
// Diferente de F0/F2, este acessa toda a memória (0x0000-0xFFFF)
void opcode_FA(Emulator* emu, uint16_t opcode) {
    uint16_t addr = cpu_next_u16(emu);
    emu->cpu.A = mmu_read_byte(emu, addr);
}

// 0xFB: EI (Enable Interrupts)
// Habilita a flag IME.
// NOTA TÉCNICA: No hardware real, há um delay de 1 instrução antes
// de as interrupções serem efetivamente ativadas.
// Para este estágio do emulador, ativação imediata costuma funcionar,
// mas tenha isso em mente para precisão futura (bug do "EI instruction delay").
void opcode_FB(Emulator* emu, uint16_t opcode) {
    emu->cpu.interrupts_enabled = true;
}

// 0xFC: ILLEGAL / UNUSED

// 0xFD: ILLEGAL / UNUSED

// 0xFE: CP n8 (Imediato)
void opcode_FE(Emulator* emu, uint16_t opcode) {
    uint8_t val = cpu_next_u8(emu);
    cp_n8(emu, val); // Usa helper existente
}

// 0xFF: RST 38H
// Muito comum! Frequentemente usado como handler de interrupção 0xFF
// (se houver padding de código) ou chamadas de sistema em alguns jogos.
void opcode_FF(Emulator* emu, uint16_t opcode) {
    push_u16(emu, emu->cpu.PC);
    emu->cpu.PC = 0x0038;
}

// --- TABELA DE INSTRUÇÕES ---
// Inicializa tudo com op_notImplemented usando designador de intervalo do GCC (GNU Extension)
// Se não estiver usando GCC/Clang, terá que inicializar manualmente ou num loop.

InstructionFunc instructions[256] = {
    [0x00] = opcode_00, [0x01] = opcode_01, [0x02] = opcode_02, [0x03] = opcode_03,
    [0x04] = opcode_04, [0x05] = opcode_05, [0x06] = opcode_06, [0x07] = opcode_07,
    [0x08] = opcode_08, [0x09] = opcode_09, [0x0A] = opcode_0A, [0x0B] = opcode_0B,
    [0x0C] = opcode_0C, [0x0D] = opcode_0D, [0x0E] = opcode_0E, [0x0F] = opcode_0F,

    [0x10] = opcode_10, [0x11] = opcode_11, [0x12] = opcode_12, [0x13] = opcode_13,
    [0x14] = opcode_14, [0x15] = opcode_15, [0x16] = opcode_16, [0x17] = opcode_17,
    [0x18] = opcode_18, [0x19] = opcode_19, [0x1A] = opcode_1A, [0x1B] = opcode_1B,
    [0x1C] = opcode_1C, [0x1D] = opcode_1D, [0x1E] = opcode_1E, [0x1F] = opcode_1F,

    [0x20] = opcode_20, [0x21] = opcode_21, [0x22] = opcode_22, [0x23] = opcode_23,
    [0x24] = opcode_24, [0x25] = opcode_25, [0x26] = opcode_26, [0x27] = opcode_27,
    [0x28] = opcode_28, [0x29] = opcode_29, [0x2A] = opcode_2A, [0x2B] = opcode_2B,
    [0x2C] = opcode_2C, [0x2D] = opcode_2D, [0x2E] = opcode_2E, [0x2F] = opcode_2F,

    [0x30] = opcode_30, [0x31] = opcode_31, [0x32] = opcode_32, [0x33] = opcode_33,
    [0x34] = opcode_34, [0x35] = opcode_35, [0x36] = opcode_36, [0x37] = opcode_37,
    [0x38] = opcode_38, [0x39] = opcode_39, [0x3A] = opcode_3A, [0x3B] = opcode_3B,
    [0x3C] = opcode_3C, [0x3D] = opcode_3D, [0x3E] = opcode_3E, [0x3F] = opcode_3F,

    [0x40] = opcode_40, [0x41] = opcode_41, [0x42] = opcode_42, [0x43] = opcode_43,
    [0x44] = opcode_44, [0x45] = opcode_45, [0x46] = opcode_46, [0x47] = opcode_47,
    [0x48] = opcode_48, [0x49] = opcode_49, [0x4A] = opcode_4A, [0x4B] = opcode_4B,
    [0x4C] = opcode_4C, [0x4D] = opcode_4D, [0x4E] = opcode_4E, [0x4F] = opcode_4F,

    [0x50] = opcode_50, [0x51] = opcode_51, [0x52] = opcode_52, [0x53] = opcode_53,
    [0x54] = opcode_54, [0x55] = opcode_55, [0x56] = opcode_56, [0x57] = opcode_57,
    [0x58] = opcode_58, [0x59] = opcode_59, [0x5A] = opcode_5A, [0x5B] = opcode_5B,
    [0x5C] = opcode_5C, [0x5D] = opcode_5D, [0x5E] = opcode_5E, [0x5F] = opcode_5F,

    [0x60] = opcode_60, [0x61] = opcode_61, [0x62] = opcode_62, [0x63] = opcode_63,
    [0x64] = opcode_64, [0x65] = opcode_65, [0x66] = opcode_66, [0x67] = opcode_67,
    [0x68] = opcode_68, [0x69] = opcode_69, [0x6A] = opcode_6A, [0x6B] = opcode_6B,
    [0x6C] = opcode_6C, [0x6D] = opcode_6D, [0x6E] = opcode_6E, [0x6F] = opcode_6F,

    [0x70] = opcode_70, [0x71] = opcode_71, [0x72] = opcode_72, [0x73] = opcode_73,
    [0x74] = opcode_74, [0x75] = opcode_75, [0x76] = opcode_76, [0x77] = opcode_77,
    [0x78] = opcode_78, [0x79] = opcode_79, [0x7A] = opcode_7A, [0x7B] = opcode_7B,
    [0x7C] = opcode_7C, [0x7D] = opcode_7D, [0x7E] = opcode_7E, [0x7F] = opcode_7F,

    [0x80] = opcode_80, [0x81] = opcode_81, [0x82] = opcode_82, [0x83] = opcode_83,
    [0x84] = opcode_84, [0x85] = opcode_85, [0x86] = opcode_86, [0x87] = opcode_87,
    [0x88] = opcode_88, [0x89] = opcode_89, [0x8A] = opcode_8A, [0x8B] = opcode_8B,
    [0x8C] = opcode_8C, [0x8D] = opcode_8D, [0x8E] = opcode_8E, [0x8F] = opcode_8F,

    [0x90] = opcode_90, [0x91] = opcode_91, [0x92] = opcode_92, [0x93] = opcode_93,
    [0x94] = opcode_94, [0x95] = opcode_95, [0x96] = opcode_96, [0x97] = opcode_97,
    [0x98] = opcode_98, [0x99] = opcode_99, [0x9A] = opcode_9A, [0x9B] = opcode_9B,
    [0x9C] = opcode_9C, [0x9D] = opcode_9D, [0x9E] = opcode_9E, [0x9F] = opcode_9F,

    [0xA0] = opcode_A0, [0xA1] = opcode_A1, [0xA2] = opcode_A2, [0xA3] = opcode_A3,
    [0xA4] = opcode_A4, [0xA5] = opcode_A5, [0xA6] = opcode_A6, [0xA7] = opcode_A7,
    [0xA8] = opcode_A8, [0xA9] = opcode_A9, [0xAA] = opcode_AA, [0xAB] = opcode_AB,
    [0xAC] = opcode_AC, [0xAD] = opcode_AD, [0xAE] = opcode_AE, [0xAF] = opcode_AF,

    [0xB0] = opcode_B0, [0xB1] = opcode_B1, [0xB2] = opcode_B2, [0xB3] = opcode_B3,
    [0xB4] = opcode_B4, [0xB5] = opcode_B5, [0xB6] = opcode_B6, [0xB7] = opcode_B7,
    [0xB8] = opcode_B8, [0xB9] = opcode_B9, [0xBA] = opcode_BA, [0xBB] = opcode_BB,
    [0xBC] = opcode_BC, [0xBD] = opcode_BD, [0xBE] = opcode_BE, [0xBF] = opcode_BF,

    [0xC0] = opcode_C0, [0xC1] = opcode_C1, [0xC2] = opcode_C2, [0xC3] = opcode_C3,
    [0xC4] = opcode_C4, [0xC5] = opcode_C5, [0xC6] = opcode_C6, [0xC7] = opcode_C7,
    [0xC8] = opcode_C8, [0xC9] = opcode_C9, [0xCA] = opcode_CA, [0xCB] = opcode_CB,
    [0xCC] = opcode_CC, [0xCD] = opcode_CD, [0xCE] = opcode_CE, [0xCF] = opcode_CF,

    [0xD0] = opcode_D0, [0xD1] = opcode_D1, [0xD2] = opcode_D2, /* [0xD3] = op_notImplemented, */
    [0xD4] = opcode_D4, [0xD5] = opcode_D5, [0xD6] = opcode_D6, [0xD7] = opcode_D7,
    [0xD8] = opcode_D8, [0xD9] = opcode_D9, [0xDA] = opcode_DA, /* [0xDB] = op_notImplemented, */
    [0xDC] = opcode_DC, /* [0xDD] = op_notImplemented, */ [0xDE] = opcode_DE, [0xDF] = opcode_DF,

    [0xE0] = opcode_E0, [0xE1] = opcode_E1, [0xE2] = opcode_E2, /* [0xE3] = op_notImplemented, */
    /* [0xE4] = op_notImplemented, */ [0xE5] = opcode_E5, [0xE6] = opcode_E6, [0xE7] = opcode_E7,
    [0xE8] = opcode_E8, [0xE9] = opcode_E9, [0xEA] = opcode_EA, /* [0xEB] = op_notImplemented, */
    /* [0xEC] = op_notImplemented, */ /* [0xED] = op_notImplemented, */ [0xEE] = opcode_EE, [0xEF] = opcode_EF,

    [0xF0] = opcode_F0, [0xF1] = opcode_F1, [0xF2] = opcode_F2, [0xF3] = opcode_F3,
    /* [0xF4] = op_notImplemented, */ [0xF5] = opcode_F5, [0xF6] = opcode_F6, [0xF7] = opcode_F7,
    [0xF8] = opcode_F8, [0xF9] = opcode_F9, [0xFA] = opcode_FA, [0xFB] = opcode_FB,
    /* [0xFC] = op_notImplemented, */ /* [0xFD] = op_notImplemented, */ [0xFE] = opcode_FE, [0xFF] = opcode_FF
};