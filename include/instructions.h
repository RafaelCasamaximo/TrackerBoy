#pragma once

#include <emulator.h>
#include <stdio.h>

typedef void (*InstructionFunc)(Emulator* emu);

InstructionFunc instructions[256]; // Array de ponteiros para funções de instrução

void op_notImplemented(Emulator* emu);