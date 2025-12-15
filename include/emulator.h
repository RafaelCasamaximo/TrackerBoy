#pragma once

#include <stdbool.h>
#include <cpu.h>
#include <mmu.h>
#include <cartridge.h>

typedef struct Emulator 
{
    CPU cpu;
    MMU mmu;
    bool is_running;
    bool paused;
    bool debug_mode;

    Cartridge cart;

} Emulator;

void initialize_emulator(Emulator* emu, char* cartridge_path);

void emu_run(int argc, char** argv);