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

u16 bus_read16(u16 address)
{
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);

    return lo | (hi << 8);
}


void bus_write16(u16 address, u16 value)
{
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}