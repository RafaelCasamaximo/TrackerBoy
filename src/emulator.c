#include <emulator.h>
#include <cartridge.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_emulator(Emulator* emu)
{
    initialize_cpu(&emu->cpu);
    initialize_mmu(&emu->mmu);

    emu->is_running = false;
    emu->paused = false;
    emu->debug_mode = false;
}


void emu_run(int argc, char** argv)
{
    Emulator emu;
    initialize_emulator(&emu);

    Cartridge cart;
    if (load_cartridge(argv[1], &cart) != 0) {
        // Handle error loading cartridge
        printf("Error loading cartridge\n");
        return;
    }

    emu.is_running = true;

    // Main emulation loop would go here
}