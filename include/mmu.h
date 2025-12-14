#pragma once

#include <stddef.h>

typedef struct MMU 
{
    uint8_t* memory;
    size_t size;
} MMU;