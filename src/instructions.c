#include <instructions.h>

Instruction instructions[0x100] = {

    // 0x0X
    [0x00] = {IN_NOP, AM_IMP},                              // Implemented
    [0x01] = {IN_LD, AM_R_D16, RT_BC},                      // Implemented
    [0x02] = {IN_LD, AM_MR_R, RT_BC, RT_A},                 // Implemented
    [0x03] = {IN_INC, AM_R, RT_BC},                         // Implemented
    [0x04] = {IN_INC, AM_R, RT_B},                          // Implemented
    [0x05] = {IN_DEC, AM_R, RT_B},                          // Implemented
    [0x06] = {IN_LD, AM_R_D8, RT_B},                        // Implemented
    [0x07] = {IN_RLCA},
    [0x08] = {IN_LD, AM_A16_R, RT_NONE, RT_SP},             // Implemented
    [0x09] = {IN_ADD, AM_R_R, RT_HL, RT_BC},                // Implemented
    [0x0A] = {IN_LD, AM_R_MR, RT_A, RT_BC},                 // Implemented
    [0x0B] = {IN_DEC, AM_R, RT_BC},                         // Implemented
    [0x0C] = {IN_INC, AM_R, RT_C},                          // Implemented
    [0x0D] = {IN_DEC, AM_R, RT_D},                          // Implemented
    [0x0E] = {IN_LD, AM_R_D8, RT_C},                        // Implemented
    [0x0F] = {IN_RRCA},

    // 0x1X
    [0x10] = {IN_STOP, AM_D8},
    [0x11] = {IN_LD, AM_R_D16, RT_DE},                      // Implemented
    [0x12] = {IN_LD, AM_MR_R, RT_DE, RT_A},                 // Implemented
    [0x13] = {IN_INC, AM_R, RT_DE},                         // Implemented
    [0x14] = {IN_INC, AM_R, RT_D},                          // Implemented
    [0x15] = {IN_DEC, AM_R, RT_D},                          // Implemented
    [0x16] = {IN_LD, AM_R_D8, RT_D},                        // Implemented
    [0x17] = {IN_RLA},
    [0x18] = {IN_JR, AM_D8},                                // Implemented
    [0x19] = {IN_ADD, AM_R_R, RT_HL, RT_DE},                // Implemented
    [0x1A] = {IN_LD, AM_R_MR, RT_A, RT_DE},                 // Implemented
    [0x1B] = {IN_DEC, AM_R, RT_DE},                         // Implemented
    [0x1C] = {IN_INC, AM_R, RT_E},                          // Implemented
    [0x1D] = {IN_DEC, AM_R, RT_E},                          // Implemented
    [0x1E] = {IN_LD, AM_R_D8, RT_E},                        // Implemented
    [0x1F] = {IN_RRA},

    // 0x2X
    [0x20] = {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ},       // Implemented
    [0x21] = {IN_LD, AM_R_D16, RT_HL},                      // Implemented
    [0x22] = {IN_LD, AM_HLI_R, RT_HL, RT_A},                // Implemented
    [0x23] = {IN_INC, AM_R, RT_HL},                         // Implemented
    [0x24] = {IN_INC, AM_R, RT_H},                          // Implemented
    [0x25] = {IN_DEC, AM_R, RT_H},                          // Implemented
    [0x26] = {IN_LD, AM_R_D8, RT_H},                        // Implemented
    [0x27] = {IN_DAA},
    [0x28] = {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_Z},        // Implemented
    [0x29] = {IN_ADD, AM_R_R, RT_HL, RT_HL},                // Implemented
    [0x2A] = {IN_LD, AM_R_HLI, RT_A, RT_HL},                // Implemented
    [0x2B] = {IN_DEC, AM_R, RT_HL},                         // Implemented
    [0x2C] = {IN_INC, AM_R, RT_L},                          // Implemented
    [0x2D] = {IN_DEC, AM_R, RT_L},                          // Implemented
    [0x2E] = {IN_LD, AM_R_D8, RT_L},                        // Implemented
    [0x2F] = {IN_CPL},

    // 0x3X
    [0x30] = {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NC},       // Implemented
    [0x31] = {IN_LD, AM_R_D16, RT_SP},                      // Implemented
    [0x32] = {IN_LD, AM_HLD_R, RT_HL, RT_A},                // Implemented
    [0x33] = {IN_INC, AM_R, RT_SP},                         // Implemented
    [0x34] = {IN_INC, AM_MR, RT_HL},                        // Implemented
    [0x35] = {IN_DEC, AM_MR, RT_HL},                        // Implemented
    [0x36] = {IN_LD, AM_MR_D8, RT_HL},                      // Implemented
    [0x37] = {IN_SCF},
    [0x38] = {IN_JR, AM_R_D8, RT_C},                        // Implemented
    [0x39] = {IN_ADD, AM_R_R, RT_HL, RT_SP},                // Implemented
    [0x3A] = {IN_LD, AM_R_HLD, RT_A, RT_HL},                // Implemented
    [0x3B] = {IN_DEC, AM_R, RT_SP},                         // Implemented
    [0x3C] = {IN_INC, AM_R, RT_A},                          // Implemented
    [0x3D] = {IN_DEC, AM_R, RT_A},                          // Implemented
    [0x3E] = {IN_LD, AM_R_D8, RT_A},                        // Implemented
    [0x3F] = {IN_CCF},

    // 0x4X
    [0x40] = {IN_LD, AM_R_R, RT_B, RT_B},                   // Implemented
    [0x41] = {IN_LD, AM_R_R, RT_B, RT_C},                   // Implemented
    [0x42] = {IN_LD, AM_R_R, RT_B, RT_D},                   // Implemented
    [0x43] = {IN_LD, AM_R_R, RT_B, RT_E},                   // Implemented
    [0x44] = {IN_LD, AM_R_R, RT_B, RT_H},                   // Implemented
    [0x45] = {IN_LD, AM_R_R, RT_B, RT_L},                   // Implemented
    [0x46] = {IN_LD, AM_R_MR, RT_B, RT_HL},                 // Implemented
    [0x47] = {IN_LD, AM_R_R, RT_B, RT_A},                   // Implemented
    [0x48] = {IN_LD, AM_R_R, RT_C, RT_B},                   // Implemented
    [0x49] = {IN_LD, AM_R_R, RT_C, RT_C},                   // Implemented
    [0x4A] = {IN_LD, AM_R_R, RT_C, RT_D},                   // Implemented
    [0x4B] = {IN_LD, AM_R_R, RT_C, RT_E},                   // Implemented
    [0x4C] = {IN_LD, AM_R_R, RT_C, RT_H},                   // Implemented
    [0x4D] = {IN_LD, AM_R_R, RT_C, RT_L},                   // Implemented
    [0x4E] = {IN_LD, AM_R_MR, RT_C, RT_HL},                 // Implemented
    [0x4F] = {IN_LD, AM_R_R, RT_C, RT_A},                   // Implemented

    // 0x5X
    [0x50] = {IN_LD, AM_R_R, RT_D, RT_B},                   // Implemented
    [0x51] = {IN_LD, AM_R_R, RT_D, RT_C},                   // Implemented
    [0x52] = {IN_LD, AM_R_R, RT_D, RT_D},                   // Implemented
    [0x53] = {IN_LD, AM_R_R, RT_D, RT_E},                   // Implemented
    [0x54] = {IN_LD, AM_R_R, RT_D, RT_H},                   // Implemented
    [0x55] = {IN_LD, AM_R_R, RT_D, RT_L},                   // Implemented
    [0x56] = {IN_LD, AM_R_MR, RT_D, RT_HL},                 // Implemented
    [0x57] = {IN_LD, AM_R_R, RT_D, RT_A},                   // Implemented
    [0x58] = {IN_LD, AM_R_R, RT_E, RT_B},                   // Implemented
    [0x59] = {IN_LD, AM_R_R, RT_E, RT_C},                   // Implemented
    [0x5A] = {IN_LD, AM_R_R, RT_E, RT_D},                   // Implemented
    [0x5B] = {IN_LD, AM_R_R, RT_E, RT_E},                   // Implemented
    [0x5C] = {IN_LD, AM_R_R, RT_E, RT_H},                   // Implemented
    [0x5D] = {IN_LD, AM_R_R, RT_E, RT_L},                   // Implemented
    [0x5E] = {IN_LD, AM_R_MR, RT_E, RT_HL},                 // Implemented
    [0x5F] = {IN_LD, AM_R_R, RT_E, RT_A},                   // Implemented

    // 0x6X
    [0x60] = {IN_LD, AM_R_R, RT_H, RT_B},                   // Implemented
    [0x61] = {IN_LD, AM_R_R, RT_H, RT_C},                   // Implemented
    [0x62] = {IN_LD, AM_R_R, RT_H, RT_D},                   // Implemented
    [0x63] = {IN_LD, AM_R_R, RT_H, RT_E},                   // Implemented
    [0x64] = {IN_LD, AM_R_R, RT_H, RT_H},                   // Implemented
    [0x65] = {IN_LD, AM_R_R, RT_H, RT_L},                   // Implemented
    [0x66] = {IN_LD, AM_R_MR, RT_H, RT_HL},                 // Implemented
    [0x67] = {IN_LD, AM_R_R, RT_H, RT_A},                   // Implemented
    [0x68] = {IN_LD, AM_R_R, RT_L, RT_B},                   // Implemented
    [0x69] = {IN_LD, AM_R_R, RT_L, RT_C},                   // Implemented
    [0x6A] = {IN_LD, AM_R_R, RT_L, RT_D},                   // Implemented
    [0x6B] = {IN_LD, AM_R_R, RT_L, RT_E},                   // Implemented
    [0x6C] = {IN_LD, AM_R_R, RT_L, RT_H},                   // Implemented
    [0x6D] = {IN_LD, AM_R_R, RT_L, RT_L},                   // Implemented
    [0x6E] = {IN_LD, AM_R_MR, RT_L, RT_HL},                 // Implemented
    [0x6F] = {IN_LD, AM_R_R, RT_L, RT_A},                   // Implemented

    // 0x7X
    [0x70] = {IN_LD, AM_MR_R, RT_HL, RT_B},                 // Implemented
    [0x71] = {IN_LD, AM_MR_R, RT_HL, RT_C},                 // Implemented
    [0x72] = {IN_LD, AM_MR_R, RT_HL, RT_D},                 // Implemented
    [0x73] = {IN_LD, AM_MR_R, RT_HL, RT_E},                 // Implemented
    [0x74] = {IN_LD, AM_MR_R, RT_HL, RT_H},                 // Implemented
    [0x75] = {IN_LD, AM_MR_R, RT_HL, RT_L},                 // Implemented
    [0x76] = {IN_HALT},
    [0x77] = {IN_LD, AM_MR_R, RT_HL, RT_A},                 // Implemented
    [0x78] = {IN_LD, AM_R_R, RT_A, RT_B},                   // Implemented
    [0x79] = {IN_LD, AM_R_R, RT_A, RT_C},                   // Implemented
    [0x7A] = {IN_LD, AM_R_R, RT_A, RT_D},                   // Implemented
    [0x7B] = {IN_LD, AM_R_R, RT_A, RT_E},                   // Implemented
    [0x7C] = {IN_LD, AM_R_R, RT_A, RT_H},                   // Implemented
    [0x7D] = {IN_LD, AM_R_R, RT_A, RT_L},                   // Implemented
    [0x7E] = {IN_LD, AM_R_MR, RT_A, RT_HL},                 // Implemented
    [0x7F] = {IN_LD, AM_R_R, RT_A, RT_A},                   // Implemented

    // 0x8X
    [0x80] = {IN_ADD, AM_R_R, RT_A, RT_B},                  // Implemented
    [0x81] = {IN_ADD, AM_R_R, RT_A, RT_C},                  // Implemented
    [0x82] = {IN_ADD, AM_R_R, RT_A, RT_D},                  // Implemented
    [0x83] = {IN_ADD, AM_R_R, RT_A, RT_E},                  // Implemented
    [0x84] = {IN_ADD, AM_R_R, RT_A, RT_H},                  // Implemented
    [0x85] = {IN_ADD, AM_R_R, RT_A, RT_L},                  // Implemented
    [0x86] = {IN_ADD, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0x87] = {IN_ADD, AM_R_R, RT_A, RT_A},                  // Implemented
    [0x88] = {IN_ADC, AM_R_R, RT_A, RT_B},                  // Implemented
    [0x89] = {IN_ADC, AM_R_R, RT_A, RT_C},                  // Implemented
    [0x8A] = {IN_ADC, AM_R_R, RT_A, RT_D},                  // Implemented
    [0x8B] = {IN_ADC, AM_R_R, RT_A, RT_E},                  // Implemented
    [0x8C] = {IN_ADC, AM_R_R, RT_A, RT_H},                  // Implemented
    [0x8D] = {IN_ADC, AM_R_R, RT_A, RT_L},                  // Implemented
    [0x8E] = {IN_ADC, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0x8F] = {IN_ADC, AM_R_R, RT_A, RT_A},                  // Implemented

    // 0x9X
    [0x90] = {IN_SUB, AM_R_R, RT_A, RT_B},                  // Implemented
    [0x91] = {IN_SUB, AM_R_R, RT_A, RT_C},                  // Implemented
    [0x92] = {IN_SUB, AM_R_R, RT_A, RT_D},                  // Implemented
    [0x93] = {IN_SUB, AM_R_R, RT_A, RT_E},                  // Implemented
    [0x94] = {IN_SUB, AM_R_R, RT_A, RT_H},                  // Implemented
    [0x95] = {IN_SUB, AM_R_R, RT_A, RT_L},                  // Implemented
    [0x96] = {IN_SUB, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0x97] = {IN_SUB, AM_R_R, RT_A, RT_A},                  // Implemented
    [0x98] = {IN_SBC, AM_R_R, RT_A, RT_B},                  // Implemented
    [0x99] = {IN_SBC, AM_R_R, RT_A, RT_C},                  // Implemented
    [0x9A] = {IN_SBC, AM_R_R, RT_A, RT_D},                  // Implemented
    [0x9B] = {IN_SBC, AM_R_R, RT_A, RT_E},                  // Implemented
    [0x9C] = {IN_SBC, AM_R_R, RT_A, RT_H},                  // Implemented
    [0x9D] = {IN_SBC, AM_R_R, RT_A, RT_L},                  // Implemented
    [0x9E] = {IN_SBC, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0x9F] = {IN_SBC, AM_R_R, RT_A, RT_A},                  // Implemented

    // 0xAX
    [0xA0] = {IN_AND, AM_R_R, RT_A, RT_B},                  // Implemented
    [0xA1] = {IN_AND, AM_R_R, RT_A, RT_C},                  // Implemented
    [0xA2] = {IN_AND, AM_R_R, RT_A, RT_D},                  // Implemented
    [0xA3] = {IN_AND, AM_R_R, RT_A, RT_E},                  // Implemented
    [0xA4] = {IN_AND, AM_R_R, RT_A, RT_H},                  // Implemented
    [0xA5] = {IN_AND, AM_R_R, RT_A, RT_L},                  // Implemented
    [0xA6] = {IN_AND, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0xA7] = {IN_AND, AM_R_R, RT_A, RT_A},                  // Implemented
    [0xA8] = {IN_XOR, AM_R_R, RT_A, RT_B},                  // Implemented
    [0xA9] = {IN_XOR, AM_R_R, RT_A, RT_C},                  // Implemented
    [0xAA] = {IN_XOR, AM_R_R, RT_A, RT_D},                  // Implemented
    [0xAB] = {IN_XOR, AM_R_R, RT_A, RT_E},                  // Implemented
    [0xAC] = {IN_XOR, AM_R_R, RT_A, RT_H},                  // Implemented
    [0xAD] = {IN_XOR, AM_R_R, RT_A, RT_L},                  // Implemented
    [0xAE] = {IN_XOR, AM_R_MR, RT_A, RT_HL},                // Implemented
    [0xAF] = {IN_XOR, AM_R_R, RT_A, RT_A},                  // Implemented

    // 0xBX
    [0xB0] = {IN_OR, AM_R_R, RT_A, RT_B},                   // Implemented
    [0xB1] = {IN_OR, AM_R_R, RT_A, RT_C},                   // Implemented
    [0xB2] = {IN_OR, AM_R_R, RT_A, RT_D},                   // Implemented
    [0xB3] = {IN_OR, AM_R_R, RT_A, RT_E},                   // Implemented
    [0xB4] = {IN_OR, AM_R_R, RT_A, RT_H},                   // Implemented
    [0xB5] = {IN_OR, AM_R_R, RT_A, RT_L},                   // Implemented
    [0xB6] = {IN_OR, AM_R_MR, RT_A, RT_HL},                 // Implemented
    [0xB7] = {IN_OR, AM_R_R, RT_A, RT_A},                   // Implemented
    [0xB8] = {IN_CP, AM_R_R, RT_A, RT_B},                   // Implemented
    [0xB9] = {IN_CP, AM_R_R, RT_A, RT_C},                   // Implemented
    [0xBA] = {IN_CP, AM_R_R, RT_A, RT_D},                   // Implemented
    [0xBB] = {IN_CP, AM_R_R, RT_A, RT_E},                   // Implemented
    [0xBC] = {IN_CP, AM_R_R, RT_A, RT_H},                   // Implemented
    [0xBD] = {IN_CP, AM_R_R, RT_A, RT_L},                   // Implemented
    [0xBE] = {IN_CP, AM_R_MR, RT_A, RT_HL},                 // Implemented
    [0xBF] = {IN_CP, AM_R_R, RT_A, RT_A},                   // Implemented

    // 0xCX
    [0xC0] = {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NZ},     // Implemented
    [0xC1] = {IN_POP, AM_R, RT_BC},                         // Implemented
    [0xC2] = {IN_JP, AM_A16, RT_NONE, RT_NONE, CT_NZ},      // Implemented
    [0xC3] = {IN_JP, AM_A16},                               // Implemented
    [0xC4] = {IN_CALL, AM_A16, RT_NONE, RT_NONE, CT_NZ},    // Implemented
    [0xC5] = {IN_PUSH, AM_R, RT_BC},                        // Implemented
    [0xC6] = {IN_ADD, AM_R_D8, RT_A},                       // Implemented
    [0xC7] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00}, //Implemented
    [0xC8] = {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_Z},      // Implemented
    [0xC9] = {IN_RET},                                      // Implemented
    [0xCA] = {IN_JP, AM_A16, RT_NONE, RT_NONE, CT_Z},       // Implemented
    [0xCB] = {IN_CB, AM_D8},
    [0xCC] = {IN_CALL, AM_A16, RT_NONE, RT_NONE, CT_Z},     // Implemented
    [0xCD] = {IN_CALL, AM_A16, RT_NONE, RT_NONE},           // Implemented
    [0xCE] = {IN_ADC, AM_R_D8, RT_A},                       // Implemented
    [0xCF] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08}, // Implemented

    // 0xDX
    [0xD0] = {IN_RET, AM_IMP, RT_NONE, RT_NONE, CT_NC},     // Implemented
    [0xD1] = {IN_POP, AM_R, RT_DE},                         // Implemented
    [0xD2] = {IN_JP, AM_A16, RT_NONE, RT_NONE, CT_NC},      // Implemented
    [0xD3] = {IN_ERR},
    [0xD4] = {IN_CALL, AM_A16, RT_NONE, RT_NONE, CT_NC},    // Implemented
    [0xD5] = {IN_PUSH, AM_R, RT_DE},                        // Implemented
    [0xD6] = {IN_SUB, AM_R_D8, RT_A},                       // Implemented
    [0xD7] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10}, // Implemented
    [0xD8] = {IN_RET, AM_R, RT_C},                          // Implemented
    [0xD9] = {IN_RETI},                                     // Implemented
    [0xDA] = {IN_JP, AM_A16, RT_NONE, RT_NONE, CT_C},       // Implemented
    [0xDB] = {IN_ERR},
    [0xDC] = {IN_CALL, AM_A16, RT_NONE, RT_NONE, CT_C},     // Implemented
    [0xDD] = {IN_ERR},
    [0xDE] = {IN_SBC, AM_R_D8, RT_A},                       // Implemented
    [0xDF] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18}, // Implemented

    // 0xEX
    [0xE0] = {IN_LDH, AM_A8_R, RT_NONE, RT_A},              // Implemented
    [0xE1] = {IN_POP, AM_R, RT_HL},                         // Implemented
    [0xE2] = {IN_LD, AM_MR_R, RT_C, RT_A},                  // Implemented
    [0xE3] = {IN_ERR},
    [0xE4] = {IN_ERR},
    [0xE5] = {IN_PUSH, AM_R, RT_HL},                        // Implemented
    [0xE6] = {IN_AND, AM_R_D8, RT_A},                       // Implemented
    [0xE7] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20}, // Implemented
    [0xE8] = {IN_ADD, AM_R_A8, RT_SP},                      // Implemented
    [0xE9] = {IN_JP, AM_R, RT_HL},                          // Implemented
    [0xEA] = {IN_LD, AM_A16_R, RT_NONE, RT_A},              // Implemented
    [0xEB] = {IN_ERR},
    [0xEC] = {IN_ERR},
    [0xED] = {IN_ERR},
    [0xEE] = {IN_XOR, AM_R_D8, RT_A},                       // Implemented
    [0xEF] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28}, // Implemented

    // 0xFX
    [0xF0] = {IN_LDH, AM_R_A8, RT_A},                       // Implemented
    [0xF1] = {IN_POP, AM_R, RT_AF},                         // Implemented
    [0xF2] = {IN_LD, AM_R_MR, RT_A, RT_C},                  // Implemented
    [0xF3] = {IN_DI},                                       // Implemented
    [0xF4] = {IN_ERR},
    [0xF5] = {IN_PUSH, AM_R, RT_AF},                        // Implemented
    [0xF6] = {IN_OR, AM_R_D8, RT_A},                        // Implemented
    [0xF7] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30}, // Implemented
    [0xF8] = {IN_LDHL, AM_HL_SPR, RT_HL, RT_SP},            // Implemented
    [0xF9] = {IN_LD, AM_R_R, RT_SP, RT_HL},                 // Implemented
    [0xFA] = {IN_LD, AM_R_A16, RT_A},                       // Implemented
    [0xFB] = {IN_EI},                                       // Implemented
    [0xFC] = {IN_ERR},
    [0xFD] = {IN_ERR},
    [0xFE] = {IN_CP, AM_R_D8, RT_A},                        // Implemented
    [0xFF] = {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38}, // Implemented
};


Instruction* intruction_by_opcode(u8 opcode)
{
    if(instructions[opcode].instruction_type == IN_NONE)
    {
        return NULL;
    }

    return &instructions[opcode];
}

char* inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "RET",
    "POP",
    "JP",
    "CALL",
    "PUSH",
    "RST",
    "CB", 
    "RETI",
    "LDH",
    "DI",
    "EI",
    // LD HL,SP+r8 has alternative mnemonic LDHL SP,r8
    "LDHL",
    //CB instructions
    "CB_RLC",
    "CB_RRC",
    "CB_RL",
    "CB_RR",
    "CB_SLA",
    "CB_SRA",
    "CB_SWAP",
    "CB_SRL",
    "CB_BIT",
    "CB_RES",
    "CB_SET",
};

char* inst_name_by_type(Instruction_type type)
{
    return inst_lookup[type];
}