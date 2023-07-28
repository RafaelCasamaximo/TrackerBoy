#pragma once

#include <common.h>

/*
    Instruction Mnemonics/ Types
*/
typedef enum{
    IN_NONE,
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_RLCA,
    IN_ADD,
    IN_RRCA,
    IN_STOP,
    IN_RLA,
    IN_JR,
    IN_RRA,
    IN_DAA,
    IN_CPL,
    IN_SCF,
    IN_CCF,
    IN_HALT,
    IN_ADC,
    IN_SUB,
    IN_SBC,
    IN_AND,
    IN_XOR,
    IN_OR,
    IN_CP,
    IN_RET,
    IN_POP,
    IN_JP,
    IN_CALL,
    IN_PUSH,
    IN_RST,
    IN_CB, 
    IN_RETI,
    IN_LDH,
    IN_DI,
    IN_EI,
    // LD HL,SP+r8 has alternative mnemonic LDHL SP,r8
    IN_LDHL,
    //CB instructions
    IN_RLC,
    IN_RRC,
    IN_RL,
    IN_RR,
    IN_SLA,
    IN_SRA,
    IN_SWAP,
    IN_SRL,
    IN_BIT,
    IN_RES,
    IN_SET,
    // Error instruction
    IN_ERR,
} Instruction_type;

/*
    IMP:    Implict
    R       Register
    MR      Memory Location pointed by a Register
    D8      8-bit Immediate data
    D16     16-bit Immediate data
    A8      8-bit Address/ Used as offset to page #FF00 from RAM ($FF00 + A8)
    A16     16-bit Address
    HL      Memory Location pointed by the HL Register (Special Register on the DMG)
    HLI     HL + Post-increment
    HLD     HL + Post-decrement
    SPR     Mnemonic to LD HL,SP+r8 as LDHL SP,r8/ Stack Pointer Offset (SP + r8)

    Style:
    AM_<Address_Type-1>_<Address_Type-2>
*/

typedef enum {
    AM_IMP, // OK
    AM_R_D16, // OK
    AM_R_R, // OK
    AM_MR_R, // OK
    AM_R, // OK
    AM_R_D8, // OK
    AM_R_MR, // OK
    AM_R_HLI, // OK
    AM_R_HLD, // OK
    AM_HLI_R, // OK
    AM_HLD_R, // OK
    AM_R_A8, // OK
    AM_A8_R, // OK
    AM_HL_SPR, // OK
    AM_D16, // OK
    AM_D8, // OK
    AM_A16, // OK
    AM_A8, // OK
    AM_D16_R, // OK
    AM_MR_D8, // OK
    AM_MR, // OK
    AM_A16_R, // OK
    AM_R_A16, // OK
} Address_mode;

/*
    Register type:
    RT_None - No register
    
    A,F B,C D,E H,L
    8b-Register    8b-Register      16b-Register    
    <A-Register>,   <B-Register>,     <ABRegister>

    SP, PC
    16b-Register
*/

typedef enum{
    RT_NONE,
    RT_A, RT_F,     RT_AF,
    RT_B, RT_C,     RT_BC,
    RT_D, RT_E,     RT_DE,
    RT_H, RT_L,     RT_HL,
    RT_SP,
    RT_PC,
} Register_type;

/*
    Condition types:
    check if condition of flag is satisfied before executing
*/

typedef enum{
    CT_NONE,    // No condition
    CT_NZ,      // Not Zero
    CT_Z,       // Zero
    CT_NC,      // Not Carry
    CT_C,       // Carry
} Condition_type;

typedef struct{
    Instruction_type instruction_type;
    Address_mode address_mode;
    Register_type register_1;
    Register_type register_2;
    Condition_type condition;
    u8 param; // Exclusively to Prefix CB instructions
} Instruction;

Instruction* intruction_by_opcode(u8 opcode);
char* inst_name_by_type(Instruction_type type);