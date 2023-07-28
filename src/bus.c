#include <bus.h>
#include <cartridge.h>
#include <common.h>
#include <ram.h>
#include <cpu.h>

/*
    Start	End	    Description                         Notes
    0000	3FFF	16 KiB ROM bank 00	                From cartridge, usually a fixed bank
    4000	7FFF	16 KiB ROM Bank 01~NN	            From cartridge, switchable bank via mapper (if any)
    8000	9FFF	8 KiB Video RAM (VRAM)	            In CGB mode, switchable bank 0/1
    A000	BFFF	8 KiB External RAM                  From cartridge, switchable bank if any
    C000	CFFF	4 KiB Work RAM (WRAM)	
    D000	DFFF	4 KiB Work RAM (WRAM)	            In CGB mode, switchable bank 1~7
    E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	    Nintendo says use of this area is prohibited.
    FE00	FE9F	Object attribute memory (OAM)	
    FEA0	FEFF	Not Usable                          Nintendo says use of this area is prohibited
    FF00	FF7F	I/O Registers	
    FF80	FFFE	High RAM (HRAM)	
    FFFF	FFFF	Interrupt Enable register (IE)	
*/


// Alternative Map

/*
    0x0000 - 0x3FFF : ROM Bank 0
    0x4000 - 0x7FFF : ROM Bank 1 - Switchable
    0x8000 - 0x97FF : CHR RAM
    0x9800 - 0x9BFF : BG Map 1
    0x9C00 - 0x9FFF : BG Map 2
    0xA000 - 0xBFFF : Cartridge RAM
    0xC000 - 0xCFFF : RAM Bank 0
    0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
    0xE000 - 0xFDFF : Reserved - Echo RAM
    0xFE00 - 0xFE9F : Object Attribute Memory
    0xFEA0 - 0xFEFF : Reserved - Unusable
    0xFF00 - 0xFF7F : I/O Registers
    0xFF80 - 0xFFFE : Zero Page
*/

u8 bus_read(u16 address)
{
    // Read from ROM
    if(address < 0x8000)
    {
        return cartridge_read(address);
    }
    // Read from VRAM
    if(address < 0xA000)
    {
        log_warn("Not implemented Bus Read: VRAM (0x%2X)", address);
        return 0;
    }
    // Read from Cartridge RAM
    if(address < 0xC000)
    {
        // Read RAM as any other cartridge memory
        return cartridge_read(address);
    }
    // Read from WRAM (Working RAM)
    if(address < 0xE000)
    {
        return wram_read(address);
    }
    // ECHO RAM - Unusable
    if(address < 0xFE00)
    {
        return 0;
    }
    // Read from OAM
    if(address < 0xFEA0)
    {
        log_warn("Not implemented Bus Read: OAM (0x%2X)", address);
        return 0x0;
    }
    // Reserved - Unusable RAM
    if(address < 0xFF00)
    {
        return 0x0;
    }
    // IO Registers
    if(address < 0xFF80)
    {
        log_warn("Not implemented Bus Read: IO (0x%2X)", address);
        return 0;
    }
    // Read from HRAM
    if(address < 0xFFFF)
    {
        return hram_read(address);
    }
    if(address == 0xFFFF)
    {
        return cpu_get_ie_register();
    }

    ERROR("BUS_READ INVALID ADDRESS: 0x%2X", address);
}

void bus_write(u16 address, u8 value)
{
    // Write from ROM
    if(address < 0x8000)
    {
        cartridge_write(address, value);
        return;
    }
    // Write to VRAM
    if(address < 0xA000)
    {
        log_warn("Not implemented Bus Write: VRAM (0x%2X: 0x%2X)", address, value);
        return;
    }
    // Write to Cartridge RAM
    if(address < 0xC000)
    {
        // Write in RAM as any other cartridge memory
        cartridge_write(address, value);
        return;
    }
    // Write to WRAM (Working RAM)
    if(address < 0xE000)
    {
        wram_write(address, value);
        return;
    }
    // ECHO RAM - Unusable
    if(address < 0xFE00)
    {
        return;
    }
    if(address < 0xFEA0)
    {
        log_warn("Not implemented Bus Write: OAM (0x%2X: 0x%2X)", address, value);
        return;
    }
    // Reserved - Unusable RAM
    if(address < 0xFF00)
    {
        return;
    }
    // IO Registers
    if(address < 0xFF80)
    {
        log_warn("Not implemented Bus Write: IO (0x%2X: 0x%2X)", address, value);
        return;
    }
    // Write to HRAM
    if(address < 0xFFFF)
    {
        hram_write(address, value);
        return;
    }
    if(address == 0xFFFF)
    {
        cpu_set_ie_register(value);
        return;
    }

    ERROR("BUS_WRITE INVALID ADDRESS: 0x%2X", address);
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