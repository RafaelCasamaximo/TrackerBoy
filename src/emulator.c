#include <emulator.h>

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

    // Initialize emulator with a clean state



    // Emulator implementation goes here
}