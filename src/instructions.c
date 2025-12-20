#include <instructions.h>
#include <cpu.h>
#include <stdlib.h>

void op_notImplemented(Emulator* emu, uint16_t opcode) {
    printf("Opcode not implemented at PC(0x%04X): 0x%02X\n", emu->cpu.PC - 1, opcode);
    exit(1);
}

void opcode_00(Emulator* emu, uint16_t opcode) {
    // NOP: No Operation
    // Simply do nothing
}

// LD BC, n16: Load 16-bit immediate into BC
void opcode_01(Emulator* emu, uint16_t opcode) {
    uint16_t value = cpu_next_u16(emu);
    emu->cpu.B = (value >> 8) & 0xFF;
    emu->cpu.C = value & 0xFF;
}

// LD (BC), A: Store A into address pointed by BC
void opcode_02(Emulator* emu, uint16_t opcode) {
    uint16_t address = get_bc(emu);
    mmu_write_byte(emu, address, emu->cpu.A);
}

// INC BC: Increment BC register pair
void opcode_03(Emulator* emu, uint16_t opcode) {
    uint16_t bc = get_bc(emu);
    bc++;
    emu->cpu.B = (bc >> 8) & 0xFF;
    emu->cpu.C = bc & 0xFF;
}

// INC B: Increment register B
void opcode_04(Emulator* emu, uint16_t opcode) {
    emu->cpu.B++;
    // Set flags accordingly (not implemented here)
}

// DEC B: Decrement register B
void opcode_05(Emulator* emu, uint16_t opcode) {
    emu->cpu.B--;
    // Set flags accordingly (not implemented here)
}

// LD B, n8: Load 8-bit immediate into B
void opcode_06(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.B = value;
}

// RLCA: Rotate A left with carry
void opcode_07(Emulator* emu, uint16_t opcode) {
    uint8_t carry = (emu->cpu.A & 0x80) >> 7;
    emu->cpu.A = (emu->cpu.A << 1) | carry;
    // Set flags accordingly (not implemented here)
}

// LD (a16), SP: Store SP at address a16
void opcode_08(Emulator* emu, uint16_t opcode) {
    uint16_t address = cpu_next_u16(emu);
    mmu_write_byte(emu, address, emu->cpu.SP & 0xFF);         // Low byte
    mmu_write_byte(emu, address + 1, (emu->cpu.SP >> 8) & 0xFF); // High byte
}

// ADD HL, BC: Add BC to HL
void opcode_09(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    uint16_t bc = get_bc(emu);
    hl += bc;
    set_hl(emu, hl);
    // Set flags accordingly (not implemented here)
}

// LD A, (BC): Load value at address BC into A
void opcode_0A(Emulator* emu, uint16_t opcode) {
    uint16_t address = get_bc(emu);
    emu->cpu.A = cpu_read_u8(emu, address);
}

// DEC BC: Decrement BC register pair
void opcode_0B(Emulator* emu, uint16_t opcode) {
    uint16_t bc = get_bc(emu);
    bc--;
    emu->cpu.B = (bc >> 8) & 0xFF;
    emu->cpu.C = bc & 0xFF;
}

// INC C: Increment register C
void opcode_0C(Emulator* emu, uint16_t opcode) {
    emu->cpu.C++;
    // Set flags accordingly (not implemented here)
}

// DEC C: Decrement register C
void opcode_0D(Emulator* emu, uint16_t opcode) {
    emu->cpu.C--;
    // Set flags accordingly (not implemented here)
}

// LD C, n8: Load 8-bit immediate into C
void opcode_0E(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.C = value;
}

// RRCA: Rotate A right with carry
void opcode_0F(Emulator* emu, uint16_t opcode) {
    uint8_t carry = emu->cpu.A & 0x01;
    emu->cpu.A = (emu->cpu.A >> 1) | (carry << 7);
    // Set flags accordingly (not implemented here)
}

// STOP: Stop the CPU until a button is pressed
void opcode_10(Emulator* emu, uint16_t opcode) {
    // Implementation depends on the emulator's architecture
}

// LD DE, n16: Load 16-bit immediate into DE
void opcode_11(Emulator* emu, uint16_t opcode) {
    uint16_t value = cpu_next_u16(emu);
    emu->cpu.D = (value >> 8) & 0xFF;
    emu->cpu.E = value & 0xFF;
}

// LD (DE), A: Store A into address pointed by DE
void opcode_12(Emulator* emu, uint16_t opcode) {
    uint16_t address = ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
    mmu_write_byte(emu, address, emu->cpu.A);
}

// INC DE: Increment DE register pair
void opcode_13(Emulator* emu, uint16_t opcode) {
    uint16_t de = ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
    de++;
    emu->cpu.D = (de >> 8) & 0xFF;
    emu->cpu.E = de & 0xFF;
}

// INC D: Increment register D
void opcode_14(Emulator* emu, uint16_t opcode) {
    emu->cpu.D++;
    // Set flags accordingly (not implemented here)
}

// DEC D: Decrement register D
void opcode_15(Emulator* emu, uint16_t opcode) {
    emu->cpu.D--;
    // Set flags accordingly (not implemented here)
}

// LD D, n8: Load 8-bit immediate into D
void opcode_16(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.D = value;
}

// RLA: Rotate A left through carry
void opcode_17(Emulator* emu, uint16_t opcode) {
    uint8_t carry_in = (emu->cpu.F & 0x10) ? 1 : 0; // Assuming bit 4 of F is Carry flag
    uint8_t carry_out = (emu->cpu.A & 0x80) >> 7;
    emu->cpu.A = (emu->cpu.A << 1) | carry_in;
    // Update flags
    emu->cpu.F &= 0xEF; // Clear Carry flag
    if (carry_out) {
        emu->cpu.F |= 0x10; // Set Carry flag
    }
}

// JR r8: Jump relative by signed immediate
void opcode_18(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    emu->cpu.PC += offset;
}

// ADD HL, DE: Add DE to HL
void opcode_19(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    uint16_t de = ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
    hl += de;
    set_hl(emu, hl);
    // Set flags accordingly (not implemented here)
}

// LD A, (DE): Load value at address DE into A
void opcode_1A(Emulator* emu, uint16_t opcode) {
    uint16_t address = ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
    emu->cpu.A = cpu_read_u8(emu, address);
}

// DEC DE: Decrement DE register pair
void opcode_1B(Emulator* emu, uint16_t opcode) {
    uint16_t de = ((uint16_t)emu->cpu.D << 8) | emu->cpu.E;
    de--;
    emu->cpu.D = (de >> 8) & 0xFF;
    emu->cpu.E = de & 0xFF;
}

// INC E: Increment register E
void opcode_1C(Emulator* emu, uint16_t opcode) {
    emu->cpu.E++;
    // Set flags accordingly (not implemented here)
}

// DEC E: Decrement register E
void opcode_1D(Emulator* emu, uint16_t opcode) {
    emu->cpu.E--;
    // Set flags accordingly (not implemented here)
}

// LD E, n8: Load 8-bit immediate into E
void opcode_1E(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.E = value;
}

// RRA: Rotate A right through carry
void opcode_1F(Emulator* emu, uint16_t opcode) {
    uint8_t carry_in = (emu->cpu.F & 0x10) ? 0x80 : 0; // Assuming bit 4 of F is Carry flag
    uint8_t carry_out = emu->cpu.A & 0x01;
    emu->cpu.A = (emu->cpu.A >> 1) | carry_in;
    // Update flags
    emu->cpu.F &= 0xEF; // Clear Carry flag
    if (carry_out) {
        emu->cpu.F |= 0x10; // Set Carry flag
    }
}

// JR NZ, r8: Jump relative by signed immediate if Z flag is not set
void opcode_20(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if ((emu->cpu.F & 0x80) == 0) { // Assuming bit 7 of F is Zero flag
        emu->cpu.PC += offset;
    }
}

// LD HL, n16: Load 16-bit immediate into HL
void opcode_21(Emulator* emu, uint16_t opcode) {
    uint16_t value = cpu_next_u16(emu);
    emu->cpu.H = (value >> 8) & 0xFF;
    emu->cpu.L = value & 0xFF;
}

// LD (HL+), A: Store A into address pointed by HL, then increment HL
void opcode_22(Emulator* emu, uint16_t opcode) {
    uint16_t address = get_hl(emu);
    mmu_write_byte(emu, address, emu->cpu.A);
    set_hl(emu, address + 1);
}

// INC HL: Increment HL register pair
void opcode_23(Emulator* emu, uint16_t opcode) {
    uint16_t hl = get_hl(emu);
    hl++;
    set_hl(emu, hl);    
}

// INC H: Increment register H
void opcode_24(Emulator* emu, uint16_t opcode) {
    emu->cpu.H++;
    // Set flags accordingly (not implemented here)
}

// DEC H: Decrement register H
void opcode_25(Emulator* emu, uint16_t opcode) {
    emu->cpu.H--;
    // Set flags accordingly (not implemented here)
}   

// LD H, n8: Load 8-bit immediate into H
void opcode_26(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.H = value;
}

// DAA: Decimal Adjust Accumulator
void opcode_27(Emulator* emu, uint16_t opcode) {
    // Implementation of DAA instruction (not implemented here)
}

// JR Z, r8: Jump relative by signed immediate if Z flag is set
void opcode_28(Emulator* emu, uint16_t opcode) {
    int8_t offset = (int8_t)cpu_next_u8(emu);
    if (emu->cpu.F & 0x80) { // Assuming bit 7 of F is Zero flag
        emu->cpu.PC += offset;
    }
}

// LD L, n8: Load 8-bit immediate into L
void opcode_2E(Emulator* emu, uint16_t opcode) {
    uint8_t value = cpu_next_u8(emu);
    emu->cpu.L = value;
}






// JP nn: Jump to address nn
void opcode_C3(Emulator* emu, uint16_t opcode) {
    uint16_t address = cpu_next_u16(emu);
    emu->cpu.PC = address;
}





InstructionFunc instructions[256] = {
    opcode_00,    // 0x00
    opcode_01,    // 0x01
    opcode_02,    // 0x02
    opcode_03,    // 0x03
    opcode_04,    // 0x04
    opcode_05,    // 0x05
    opcode_06,    // 0x06
    opcode_07,    // 0x07
    opcode_08,    // 0x08
    opcode_09,    // 0x09
    opcode_0A,    // 0x0A
    opcode_0B,    // 0x0B
    opcode_0C,    // 0x0C
    opcode_0D,    // 0x0D
    opcode_0E,    // 0x0E
    opcode_0F,    // 0x0F
    op_notImplemented,    // 0x10
    op_notImplemented,    // 0x11
    op_notImplemented,    // 0x12
    op_notImplemented,    // 0x13
    op_notImplemented,    // 0x14
    op_notImplemented,    // 0x15
    op_notImplemented,    // 0x16
    op_notImplemented,    // 0x17
    op_notImplemented,    // 0x18
    op_notImplemented,    // 0x19
    op_notImplemented,    // 0x1A
    op_notImplemented,    // 0x1B
    op_notImplemented,    // 0x1C
    op_notImplemented,    // 0x1D
    op_notImplemented,    // 0x1E
    op_notImplemented,    // 0x1F
    op_notImplemented,    // 0x20
    op_notImplemented,    // 0x21
    op_notImplemented,    // 0x22
    op_notImplemented,    // 0x23
    op_notImplemented,    // 0x24
    op_notImplemented,    // 0x25
    op_notImplemented,    // 0x26
    op_notImplemented,    // 0x27
    op_notImplemented,    // 0x28
    op_notImplemented,    // 0x29
    op_notImplemented,    // 0x2A
    op_notImplemented,    // 0x2B
    op_notImplemented,    // 0x2C
    op_notImplemented,    // 0x2D
    op_notImplemented,    // 0x2E
    op_notImplemented,    // 0x2F
    op_notImplemented,    // 0x30
    op_notImplemented,    // 0x31
    op_notImplemented,    // 0x32
    op_notImplemented,    // 0x33
    op_notImplemented,    // 0x34
    op_notImplemented,    // 0x35
    op_notImplemented,    // 0x36
    op_notImplemented,    // 0x37
    op_notImplemented,    // 0x38
    op_notImplemented,    // 0x39
    op_notImplemented,    // 0x3A
    op_notImplemented,    // 0x3B
    op_notImplemented,    // 0x3C
    op_notImplemented,    // 0x3D
    op_notImplemented,    // 0x3E
    op_notImplemented,    // 0x3F
    op_notImplemented,    // 0x40
    op_notImplemented,    // 0x41
    op_notImplemented,    // 0x42
    op_notImplemented,    // 0x43
    op_notImplemented,    // 0x44
    op_notImplemented,    // 0x45
    op_notImplemented,    // 0x46
    op_notImplemented,    // 0x47
    op_notImplemented,    // 0x48
    op_notImplemented,    // 0x49
    op_notImplemented,    // 0x4A
    op_notImplemented,    // 0x4B
    op_notImplemented,    // 0x4C
    op_notImplemented,    // 0x4D
    op_notImplemented,    // 0x4E
    op_notImplemented,    // 0x4F
    op_notImplemented,    // 0x50
    op_notImplemented,    // 0x51
    op_notImplemented,    // 0x52
    op_notImplemented,    // 0x53
    op_notImplemented,    // 0x54
    op_notImplemented,    // 0x55
    op_notImplemented,    // 0x56
    op_notImplemented,    // 0x57
    op_notImplemented,    // 0x58
    op_notImplemented,    // 0x59
    op_notImplemented,    // 0x5A
    op_notImplemented,    // 0x5B
    op_notImplemented,    // 0x5C
    op_notImplemented,    // 0x5D
    op_notImplemented,    // 0x5E
    op_notImplemented,    // 0x5F
    op_notImplemented,    // 0x60
    op_notImplemented,    // 0x61
    op_notImplemented,    // 0x62
    op_notImplemented,    // 0x63
    op_notImplemented,    // 0x64
    op_notImplemented,    // 0x65
    op_notImplemented,    // 0x66
    op_notImplemented,    // 0x67
    op_notImplemented,    // 0x68
    op_notImplemented,    // 0x69
    op_notImplemented,    // 0x6A
    op_notImplemented,    // 0x6B
    op_notImplemented,    // 0x6C
    op_notImplemented,    // 0x6D
    op_notImplemented,    // 0x6E
    op_notImplemented,    // 0x6F
    op_notImplemented,    // 0x70
    op_notImplemented,    // 0x71
    op_notImplemented,    // 0x72
    op_notImplemented,    // 0x73
    op_notImplemented,    // 0x74
    op_notImplemented,    // 0x75
    op_notImplemented,    // 0x76
    op_notImplemented,    // 0x77
    op_notImplemented,    // 0x78
    op_notImplemented,    // 0x79
    op_notImplemented,    // 0x7A
    op_notImplemented,    // 0x7B
    op_notImplemented,    // 0x7C
    op_notImplemented,    // 0x7D
    op_notImplemented,    // 0x7E
    op_notImplemented,    // 0x7F
    op_notImplemented,    // 0x80
    op_notImplemented,    // 0x81
    op_notImplemented,    // 0x82
    op_notImplemented,    // 0x83
    op_notImplemented,    // 0x84
    op_notImplemented,    // 0x85
    op_notImplemented,    // 0x86
    op_notImplemented,    // 0x87
    op_notImplemented,    // 0x88
    op_notImplemented,    // 0x89
    op_notImplemented,    // 0x8A
    op_notImplemented,    // 0x8B
    op_notImplemented,    // 0x8C
    op_notImplemented,    // 0x8D
    op_notImplemented,    // 0x8E
    op_notImplemented,    // 0x8F
    op_notImplemented,    // 0x90
    op_notImplemented,    // 0x91
    op_notImplemented,    // 0x92
    op_notImplemented,    // 0x93
    op_notImplemented,    // 0x94
    op_notImplemented,    // 0x95
    op_notImplemented,    // 0x96
    op_notImplemented,    // 0x97
    op_notImplemented,    // 0x98
    op_notImplemented,    // 0x99
    op_notImplemented,    // 0x9A
    op_notImplemented,    // 0x9B
    op_notImplemented,    // 0x9C
    op_notImplemented,    // 0x9D
    op_notImplemented,    // 0x9E
    op_notImplemented,    // 0x9F
    op_notImplemented,    // 0xA0
    op_notImplemented,    // 0xA1
    op_notImplemented,    // 0xA2
    op_notImplemented,    // 0xA3
    op_notImplemented,    // 0xA4
    op_notImplemented,    // 0xA5
    op_notImplemented,    // 0xA6
    op_notImplemented,    // 0xA7
    op_notImplemented,    // 0xA8
    op_notImplemented,    // 0xA9
    op_notImplemented,    // 0xAA
    op_notImplemented,    // 0xAB
    op_notImplemented,    // 0xAC
    op_notImplemented,    // 0xAD
    op_notImplemented,    // 0xAE
    op_notImplemented,    // 0xAF
    op_notImplemented,    // 0xB0
    op_notImplemented,    // 0xB1
    op_notImplemented,    // 0xB2
    op_notImplemented,    // 0xB3
    op_notImplemented,    // 0xB4
    op_notImplemented,    // 0xB5
    op_notImplemented,    // 0xB6
    op_notImplemented,    // 0xB7
    op_notImplemented,    // 0xB8
    op_notImplemented,    // 0xB9
    op_notImplemented,    // 0xBA
    op_notImplemented,    // 0xBB
    op_notImplemented,    // 0xBC
    op_notImplemented,    // 0xBD
    op_notImplemented,    // 0xBE
    op_notImplemented,    // 0xBF
    op_notImplemented,    // 0xC0
    op_notImplemented,    // 0xC1
    op_notImplemented,    // 0xC2
    opcode_C3,            // 0xC3
    op_notImplemented,    // 0xC4
    op_notImplemented,    // 0xC5
    op_notImplemented,    // 0xC6
    op_notImplemented,    // 0xC7
    op_notImplemented,    // 0xC8
    op_notImplemented,    // 0xC9
    op_notImplemented,    // 0xCA
    op_notImplemented,    // 0xCB
    op_notImplemented,    // 0xCC
    op_notImplemented,    // 0xCD
    op_notImplemented,    // 0xCE
    op_notImplemented,    // 0xCF
    op_notImplemented,    // 0xD0
    op_notImplemented,    // 0xD1
    op_notImplemented,    // 0xD2
    op_notImplemented,    // 0xD3
    op_notImplemented,    // 0xD4
    op_notImplemented,    // 0xD5
    op_notImplemented,    // 0xD6
    op_notImplemented,    // 0xD7
    op_notImplemented,    // 0xD8
    op_notImplemented,    // 0xD9
    op_notImplemented,    // 0xDA
    op_notImplemented,    // 0xDB
    op_notImplemented,    // 0xDC
    op_notImplemented,    // 0xDD
    op_notImplemented,    // 0xDE
    op_notImplemented,    // 0xDF
    op_notImplemented,    // 0xE0
    op_notImplemented,    // 0xE1
    op_notImplemented,    // 0xE2
    op_notImplemented,    // 0xE3
    op_notImplemented,    // 0xE4
    op_notImplemented,    // 0xE5
    op_notImplemented,    // 0xE6
    op_notImplemented,    // 0xE7
    op_notImplemented,    // 0xE8
    op_notImplemented,    // 0xE9
    op_notImplemented,    // 0xEA
    op_notImplemented,    // 0xEB
    op_notImplemented,    // 0xEC
    op_notImplemented,    // 0xED
    op_notImplemented,    // 0xEE
    op_notImplemented,    // 0xEF
    op_notImplemented,    // 0xF0
    op_notImplemented,    // 0xF1
    op_notImplemented,    // 0xF2
    op_notImplemented,    // 0xF3
    op_notImplemented,    // 0xF4
    op_notImplemented,    // 0xF5
    op_notImplemented,    // 0xF6
    op_notImplemented,    // 0xF7
    op_notImplemented,    // 0xF8
    op_notImplemented,    // 0xF9
    op_notImplemented,    // 0xFA
    op_notImplemented,    // 0xFB
    op_notImplemented,    // 0xFC
    op_notImplemented,    // 0xFD
    op_notImplemented,    // 0xFE
    op_notImplemented     // 0xFF
};