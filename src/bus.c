#include "../include/bus.h"
#include "../include/cartridge.h"

u8 read_address(u16 address)
{
    // Read from ROM
    if(address < 0x8000)
    {
        return cartridge_read(address);
    }

    NO_IMPL
}

void write_address(u16 address, u8 value)
{
    // Write from ROM
    if(address < 0x8000)
    {
        cartridge_write(address, value);
        return;
    }

    NO_IMPL
}
