#include <stdio.h>

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
