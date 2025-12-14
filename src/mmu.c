#include <mmu.h>
#include <stdlib.h>

void initialize_mmu(MMU* mmu)
{
    mmu->size = 0xFFFF;
    mmu->memory = (uint8_t*)malloc(mmu->size);    
}

void free_mmu(MMU *mmu)
{
    free(mmu->memory);
    mmu->memory = NULL;
    mmu->size = 0;
}

uint8_t mmu_read_byte(MMU *mmu, uint16_t address)
{
    if (address < mmu->size) {
        return mmu->memory[address];
    }
    return 0;
}

void mmu_write_byte(MMU *mmu, uint16_t address, uint8_t value)
{
    if (address < mmu->size) {
        mmu->memory[address] = value;
    }
}