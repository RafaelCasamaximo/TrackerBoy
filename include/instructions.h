#pragma once

#include <emulator.h>
#include <stdio.h>

typedef void (*InstructionFunc)(Emulator* emu, uint16_t opcode);

InstructionFunc instructions[256]; // Array de ponteiros para funções de instrução

void op_notImplemented(Emulator* emu, uint16_t opcode);

void opcode_00(Emulator* emu, uint16_t opcode); // NOP

void opcode_C3(Emulator* emu, uint16_t opcode); // JP nn