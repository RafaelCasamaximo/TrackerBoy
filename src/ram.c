#include <ram.h>

typedef struct{
    u8 wram[0x2000];
    u8 hram[0x80];
} ram_ctx;

static ram_ctx ctx;

u8 wram_read(u16 address)
{
    address -= 0xC000;

    if(address >= 0x2000)
    {
        ERROR("INVALID WRAM READ ADDRESS: %x", address + 0xC000);
    }

    return ctx.wram[address];
}

void wram_write(u16 address, u8 value)
{
    address -= 0xC000;

    if(address >= 0x2000)
    {
        ERROR("INVALID WRAM WRITE ADDRESS: %x", address + 0xC000);
    }

    ctx.wram[address] = value;
}


u8 hram_read(u16 address)
{
    address -= 0xFF80;

    if(address >= 0x80)
    {
        ERROR("INVALID HRAM READ ADDRESS: %x", address + 0xFF80);
    }

    return ctx.hram[address];
}

void hram_write(u16 address, u8 value)
{
    address -= 0xFF80;

    if(address >= 0x80)
    {
        ERROR("INVALID HRAM WRITE ADDRESS: %x", address + 0xFF80);
    }

    ctx.hram[address] = value;
}

