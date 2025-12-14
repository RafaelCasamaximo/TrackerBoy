#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct MMU 
{
    uint8_t* memory;
    size_t size;
} MMU;

void initialize_mmu(MMU* mmu);
void free_mmu(MMU* mmu);

uint8_t mmu_read_byte(MMU* mmu, uint16_t address);
void mmu_write_byte(MMU* mmu, uint16_t address, uint8_t value);
