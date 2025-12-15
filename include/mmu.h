#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct Emulator Emulator;

typedef struct MMU 
{
    uint8_t* memory;
    size_t size;
} MMU;

void initialize_mmu(Emulator* emu);
void free_mmu(Emulator* emu);

uint8_t mmu_read_byte(Emulator* emu, uint16_t address);
void mmu_write_byte(Emulator* emu, uint16_t address, uint8_t value);