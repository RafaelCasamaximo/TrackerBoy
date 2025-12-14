#include <mmu.h>
#include <stdlib.h>

void initialize_mmu(MMU* mmu)
{
    mmu->size = 0xFFFF;
    mmu->memory = (uint8_t*)malloc(mmu->size);    
}