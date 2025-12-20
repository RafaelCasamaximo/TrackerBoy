// include/instructions.h
#pragma once
#include <stdint.h>
#include "emulator.h"

typedef void (*InstructionFunc)(Emulator* emu, uint16_t opcode);

extern InstructionFunc instructions[256];

// Função de fallback
void op_notImplemented(Emulator* emu, uint16_t opcode);

// Declaração de todas as funções de opcode (0x00 até 0xFF)
void opcode_00(Emulator*, uint16_t);
void opcode_01(Emulator*, uint16_t);
void opcode_02(Emulator*, uint16_t);
void opcode_03(Emulator*, uint16_t);
void opcode_04(Emulator*, uint16_t);
void opcode_05(Emulator*, uint16_t);
void opcode_06(Emulator*, uint16_t);
void opcode_07(Emulator*, uint16_t);
void opcode_08(Emulator*, uint16_t);
void opcode_09(Emulator*, uint16_t);
void opcode_0A(Emulator*, uint16_t);
void opcode_0B(Emulator*, uint16_t);
void opcode_0C(Emulator*, uint16_t);
void opcode_0D(Emulator*, uint16_t);
void opcode_0E(Emulator*, uint16_t);
void opcode_0F(Emulator*, uint16_t);

void opcode_10(Emulator*, uint16_t);
void opcode_11(Emulator*, uint16_t);
void opcode_12(Emulator*, uint16_t);
void opcode_13(Emulator*, uint16_t);
void opcode_14(Emulator*, uint16_t);
void opcode_15(Emulator*, uint16_t);
void opcode_16(Emulator*, uint16_t);
void opcode_17(Emulator*, uint16_t);
void opcode_18(Emulator*, uint16_t);
void opcode_19(Emulator*, uint16_t);
void opcode_1A(Emulator*, uint16_t);
void opcode_1B(Emulator*, uint16_t);
void opcode_1C(Emulator*, uint16_t);
void opcode_1D(Emulator*, uint16_t);
void opcode_1E(Emulator*, uint16_t);
void opcode_1F(Emulator*, uint16_t);

void opcode_20(Emulator*, uint16_t);
void opcode_21(Emulator*, uint16_t);
void opcode_22(Emulator*, uint16_t);
void opcode_23(Emulator*, uint16_t);
void opcode_24(Emulator*, uint16_t);
void opcode_25(Emulator*, uint16_t);
void opcode_26(Emulator*, uint16_t);
void opcode_27(Emulator*, uint16_t);
void opcode_28(Emulator*, uint16_t);
void opcode_29(Emulator*, uint16_t);
void opcode_2A(Emulator*, uint16_t);
void opcode_2B(Emulator*, uint16_t);
void opcode_2C(Emulator*, uint16_t);
void opcode_2D(Emulator*, uint16_t);
void opcode_2E(Emulator*, uint16_t);
void opcode_2F(Emulator*, uint16_t);

void opcode_30(Emulator*, uint16_t);
void opcode_31(Emulator*, uint16_t);
void opcode_32(Emulator*, uint16_t);
void opcode_33(Emulator*, uint16_t);
void opcode_34(Emulator*, uint16_t);
void opcode_35(Emulator*, uint16_t);
void opcode_36(Emulator*, uint16_t);
void opcode_37(Emulator*, uint16_t);
void opcode_38(Emulator*, uint16_t);
void opcode_39(Emulator*, uint16_t);
void opcode_3A(Emulator*, uint16_t);
void opcode_3B(Emulator*, uint16_t);
void opcode_3C(Emulator*, uint16_t);
void opcode_3D(Emulator*, uint16_t);
void opcode_3E(Emulator*, uint16_t);
void opcode_3F(Emulator*, uint16_t);

void opcode_40(Emulator*, uint16_t);
void opcode_41(Emulator*, uint16_t);
void opcode_42(Emulator*, uint16_t);
void opcode_43(Emulator*, uint16_t);
void opcode_44(Emulator*, uint16_t);
void opcode_45(Emulator*, uint16_t);
void opcode_46(Emulator*, uint16_t);
void opcode_47(Emulator*, uint16_t);
void opcode_48(Emulator*, uint16_t);
void opcode_49(Emulator*, uint16_t);
void opcode_4A(Emulator*, uint16_t);
void opcode_4B(Emulator*, uint16_t);
void opcode_4C(Emulator*, uint16_t);
void opcode_4D(Emulator*, uint16_t);
void opcode_4E(Emulator*, uint16_t);
void opcode_4F(Emulator*, uint16_t);

void opcode_50(Emulator*, uint16_t);
void opcode_51(Emulator*, uint16_t);
void opcode_52(Emulator*, uint16_t);
void opcode_53(Emulator*, uint16_t);
void opcode_54(Emulator*, uint16_t);
void opcode_55(Emulator*, uint16_t);
void opcode_56(Emulator*, uint16_t);
void opcode_57(Emulator*, uint16_t);
void opcode_58(Emulator*, uint16_t);
void opcode_59(Emulator*, uint16_t);
void opcode_5A(Emulator*, uint16_t);
void opcode_5B(Emulator*, uint16_t);
void opcode_5C(Emulator*, uint16_t);
void opcode_5D(Emulator*, uint16_t);
void opcode_5E(Emulator*, uint16_t);
void opcode_5F(Emulator*, uint16_t);

void opcode_60(Emulator*, uint16_t);
void opcode_61(Emulator*, uint16_t);
void opcode_62(Emulator*, uint16_t);
void opcode_63(Emulator*, uint16_t);
void opcode_64(Emulator*, uint16_t);
void opcode_65(Emulator*, uint16_t);
void opcode_66(Emulator*, uint16_t);
void opcode_67(Emulator*, uint16_t);
void opcode_68(Emulator*, uint16_t);
void opcode_69(Emulator*, uint16_t);
void opcode_6A(Emulator*, uint16_t);
void opcode_6B(Emulator*, uint16_t);
void opcode_6C(Emulator*, uint16_t);
void opcode_6D(Emulator*, uint16_t);
void opcode_6E(Emulator*, uint16_t);
void opcode_6F(Emulator*, uint16_t);

void opcode_70(Emulator*, uint16_t);
void opcode_71(Emulator*, uint16_t);
void opcode_72(Emulator*, uint16_t);
void opcode_73(Emulator*, uint16_t);
void opcode_74(Emulator*, uint16_t);
void opcode_75(Emulator*, uint16_t);
void opcode_76(Emulator*, uint16_t);
void opcode_77(Emulator*, uint16_t);
void opcode_78(Emulator*, uint16_t);
void opcode_79(Emulator*, uint16_t);
void opcode_7A(Emulator*, uint16_t);
void opcode_7B(Emulator*, uint16_t);
void opcode_7C(Emulator*, uint16_t);
void opcode_7D(Emulator*, uint16_t);
void opcode_7E(Emulator*, uint16_t);
void opcode_7F(Emulator*, uint16_t);

void opcode_80(Emulator*, uint16_t);
void opcode_81(Emulator*, uint16_t);
void opcode_82(Emulator*, uint16_t);
void opcode_83(Emulator*, uint16_t);
void opcode_84(Emulator*, uint16_t);
void opcode_85(Emulator*, uint16_t);
void opcode_86(Emulator*, uint16_t);
void opcode_87(Emulator*, uint16_t);
void opcode_88(Emulator*, uint16_t);
void opcode_89(Emulator*, uint16_t);
void opcode_8A(Emulator*, uint16_t);
void opcode_8B(Emulator*, uint16_t);
void opcode_8C(Emulator*, uint16_t);
void opcode_8D(Emulator*, uint16_t);
void opcode_8E(Emulator*, uint16_t);
void opcode_8F(Emulator*, uint16_t);

void opcode_90(Emulator*, uint16_t);
void opcode_91(Emulator*, uint16_t);
void opcode_92(Emulator*, uint16_t);
void opcode_93(Emulator*, uint16_t);
void opcode_94(Emulator*, uint16_t);
void opcode_95(Emulator*, uint16_t);
void opcode_96(Emulator*, uint16_t);
void opcode_97(Emulator*, uint16_t);
void opcode_98(Emulator*, uint16_t);
void opcode_99(Emulator*, uint16_t);
void opcode_9A(Emulator*, uint16_t);
void opcode_9B(Emulator*, uint16_t);
void opcode_9C(Emulator*, uint16_t);
void opcode_9D(Emulator*, uint16_t);
void opcode_9E(Emulator*, uint16_t);
void opcode_9F(Emulator*, uint16_t);

void opcode_A0(Emulator*, uint16_t);
void opcode_A1(Emulator*, uint16_t);
void opcode_A2(Emulator*, uint16_t);
void opcode_A3(Emulator*, uint16_t);
void opcode_A4(Emulator*, uint16_t);
void opcode_A5(Emulator*, uint16_t);
void opcode_A6(Emulator*, uint16_t);
void opcode_A7(Emulator*, uint16_t);
void opcode_A8(Emulator*, uint16_t);
void opcode_A9(Emulator*, uint16_t);
void opcode_AA(Emulator*, uint16_t);
void opcode_AB(Emulator*, uint16_t);
void opcode_AC(Emulator*, uint16_t);
void opcode_AD(Emulator*, uint16_t);
void opcode_AE(Emulator*, uint16_t);
void opcode_AF(Emulator*, uint16_t);

void opcode_B0(Emulator*, uint16_t);
void opcode_B1(Emulator*, uint16_t);
void opcode_B2(Emulator*, uint16_t);
void opcode_B3(Emulator*, uint16_t);
void opcode_B4(Emulator*, uint16_t);
void opcode_B5(Emulator*, uint16_t);
void opcode_B6(Emulator*, uint16_t);
void opcode_B7(Emulator*, uint16_t);
void opcode_B8(Emulator*, uint16_t);
void opcode_B9(Emulator*, uint16_t);
void opcode_BA(Emulator*, uint16_t);
void opcode_BB(Emulator*, uint16_t);
void opcode_BC(Emulator*, uint16_t);
void opcode_BD(Emulator*, uint16_t);
void opcode_BE(Emulator*, uint16_t);
void opcode_BF(Emulator*, uint16_t);

void opcode_C0(Emulator*, uint16_t);
void opcode_C1(Emulator*, uint16_t);
void opcode_C2(Emulator*, uint16_t);
void opcode_C3(Emulator*, uint16_t);
void opcode_C4(Emulator*, uint16_t);
void opcode_C5(Emulator*, uint16_t);
void opcode_C6(Emulator*, uint16_t);
void opcode_C7(Emulator*, uint16_t);
void opcode_C8(Emulator*, uint16_t);
void opcode_C9(Emulator*, uint16_t);
void opcode_CA(Emulator*, uint16_t);
void opcode_CB(Emulator*, uint16_t);
void opcode_CC(Emulator*, uint16_t);
void opcode_CD(Emulator*, uint16_t);
void opcode_CE(Emulator*, uint16_t);
void opcode_CF(Emulator*, uint16_t);

void opcode_D0(Emulator*, uint16_t);
void opcode_D1(Emulator*, uint16_t);
void opcode_D2(Emulator*, uint16_t);
// D3 é ilegal
void opcode_D4(Emulator*, uint16_t);
void opcode_D5(Emulator*, uint16_t);
void opcode_D6(Emulator*, uint16_t);
void opcode_D7(Emulator*, uint16_t);
void opcode_D8(Emulator*, uint16_t);
void opcode_D9(Emulator*, uint16_t);
void opcode_DA(Emulator*, uint16_t);
// DB é ilegal
void opcode_DC(Emulator*, uint16_t);
// DD é ilegal
void opcode_DE(Emulator*, uint16_t);
void opcode_DF(Emulator*, uint16_t);

void opcode_E0(Emulator*, uint16_t);
void opcode_E1(Emulator*, uint16_t);
void opcode_E2(Emulator*, uint16_t);
// E3 é ilegal
// E4 é ilegal
void opcode_E5(Emulator*, uint16_t);
void opcode_E6(Emulator*, uint16_t);
void opcode_E7(Emulator*, uint16_t);
void opcode_E8(Emulator*, uint16_t);
void opcode_E9(Emulator*, uint16_t);
void opcode_EA(Emulator*, uint16_t);
// EB, EC, ED são ilegais
void opcode_EE(Emulator*, uint16_t);
void opcode_EF(Emulator*, uint16_t);

void opcode_F0(Emulator*, uint16_t);
void opcode_F1(Emulator*, uint16_t);
void opcode_F2(Emulator*, uint16_t);
void opcode_F3(Emulator*, uint16_t);
// F4 é ilegal
void opcode_F5(Emulator*, uint16_t);
void opcode_F6(Emulator*, uint16_t);
void opcode_F7(Emulator*, uint16_t);
void opcode_F8(Emulator*, uint16_t);
void opcode_F9(Emulator*, uint16_t);
void opcode_FA(Emulator*, uint16_t);
void opcode_FB(Emulator*, uint16_t);
// FC, FD são ilegais
void opcode_FE(Emulator*, uint16_t);
void opcode_FF(Emulator*, uint16_t);