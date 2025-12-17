#include <emulator.h>
#include <cartridge.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_emulator(Emulator* emu, char* cartridge_path)
{

    if (load_cartridge(cartridge_path, &emu->cart) != 0) {
        printf("Error loading cartridge\n");
        exit(1);
    }

    initialize_cpu(emu);
    initialize_mmu(emu);

    emu->is_running = true;
    emu->paused = false;
    emu->debug_mode = false;
}


void emu_run(int argc, char** argv)
{
    Emulator emu;
    initialize_emulator(&emu, argv[1]);

    while (emu.is_running) {
        cpu_step(&emu);
    }
}