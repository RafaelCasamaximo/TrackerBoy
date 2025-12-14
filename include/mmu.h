#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct MMU 
{
    uint8_t* memory;
    size_t size;
} MMU;

void initialize_mmu(MMU* mmu);