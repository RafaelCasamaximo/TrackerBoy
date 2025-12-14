#pragma once

#include <stdbool.h>
#include <cpu.h>
#include <mmu.h>

typedef struct Emulator 
{
    CPU cpu;
    MMU mmu;
    bool is_running;
    bool paused;
    bool debug_mode;
} Emulator;

void initialize_emulator(Emulator* emu);

void emu_run(int argc, char** argv);