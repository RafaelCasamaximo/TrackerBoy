#include <stdio.h>

#include "../include/log.h"
#include "../include/emulator.h"
#include "../include/cartridge.h"

/**
 * Emulator Components:
 * 
 * Cartridge
 * CPU
 * Address Bus
 * PPU
 * Timer
*/


static emu_ctx ctx;

emu_ctx* emu_get_ctx()
{
    return &ctx;
}

int emu_run(int argc, char** argv)
{

    if(argc < 2)
    {
        log_error("Wrong parameters. Usage: ./TrackerBoy <rom.gb>");
        return -1;
    }

    if(!cartridge_load(argv[1]))
    {
        printf("Failed to load cartridge.\n");
    }

    return 0;
}