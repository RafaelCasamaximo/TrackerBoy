#include <bus.h>
#include <cartridge.h>

u8 bus_read(u16 address)
{
    // Read from ROM
    if(address < 0x8000)
    {
        return cartridge_read(address);
    }

    NO_IMPL
}

void bus_write(u16 address, u8 value)
{
    // Write from ROM
    if(address < 0x8000)
    {
        cartridge_write(address, value);
        return;
    }

    NO_IMPL
}
