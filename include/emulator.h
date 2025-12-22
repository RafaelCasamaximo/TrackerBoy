#pragma once

#include <stdbool.h>
#include <cpu.h> // Use aspas para arquivos locais
#include <mmu.h>
#include <cartridge.h>
#include <ppu.h>        
#include <interrupts.h> 

typedef struct Emulator 
{
    CPU cpu;
    MMU mmu;
    PPU ppu;
    Cartridge cart;

    bool is_running;
    bool paused;
    bool debug_mode;

} Emulator;

void initialize_emulator(Emulator* emu, char* cartridge_path);
void emu_run(int argc, char** argv);