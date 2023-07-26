#include <stdio.h>

#include <log.h>
#include <emulator.h>
#include <cartridge.h>
#include <cpu.h>

/**
 * Emulator Components:
 * 
 * Cartridge
 * CPU
 * Address Bus
 * PPU
 * Timer
*/


static emu_ctx ctx = {0};

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

    log_info("Cartridge Loaded.");

    cpu_init();

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while (ctx.running)
    {
        if(ctx.paused)
        {
            // SDL
            // delay(10);
            continue;
        }

        if(!cpu_step())
        {
            ERROR("CPU Stopped!");
        }

        ctx.ticks++;
    }

    return 0;
}

void emu_cycles(int emu_cycles)
{
    // NO_IMPL
}