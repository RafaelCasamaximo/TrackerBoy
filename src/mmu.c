#include <mmu.h>
#include <stdlib.h>
#include <emulator.h>

void initialize_mmu(Emulator* emu)
{
    emu->mmu.size = 0xFFFF;
    emu->mmu.memory = (uint8_t*)malloc(emu->mmu.size);    
}

void free_mmu(Emulator* emu)
{
    free(emu->mmu.memory);
    emu->mmu.memory = NULL;
    emu->mmu.size = 0;
}

uint8_t mmu_read_byte(Emulator* emu, uint16_t address)
{
    if (address < 0x4000) {
        // 0000-3FFF: ROM bank 00
        // TODO: Ler da ROM fixa
    } else if (address < 0x8000) {
        // 4000-7FFF: ROM bank NN
        // TODO: Ler da ROM bancável
    } else if (address < 0xA000) {
        // 8000-9FFF: VRAM
        // TODO: Ler da VRAM
    } else if (address < 0xC000) {
        // A000-BFFF: External RAM
        // TODO: Ler da RAM externa
    } else if (address < 0xE000) {
        // C000-DFFF: WRAM
        // TODO: Ler da WRAM
    } else if (address < 0xFE00) {
        // E000-FDFF: Echo RAM (mirror de C000-DDFF)
        // TODO: Mirror de C000-DDFF
    } else if (address < 0xFEA0) {
        // FE00-FE9F: OAM
        // TODO: Ler da OAM
    } else if (address < 0xFF00) {
        // FEA0-FEFF: Não utilizável
        // TODO: Retornar 0xFF ou comportamento especial
    } else if (address < 0xFF80) {
        // FF00-FF7F: I/O Registers
        // TODO: Ler dos registradores de I/O
    } else if (address < 0xFFFF) {
        // FF80-FFFE: HRAM
        // TODO: Ler da HRAM
    } else if (address == 0xFFFF) {
        // FFFF: Interrupt Enable Register
        // TODO: Ler do IE
    }
    // Por enquanto, retorna da memória linear (placeholder)
    if (address < emu->mmu.size) {
        return emu->mmu.memory[address];
    }
    return 0;
}

void mmu_write_byte(Emulator* emu, uint16_t address, uint8_t value)
{
    if (address < 0x4000) {
        // 0000-3FFF: ROM bank 00
        // TODO: Escrita na ROM fixa (normalmente ignorada ou usada para MBC)
    } else if (address < 0x8000) {
        // 4000-7FFF: ROM bank NN
        // TODO: Escrita na ROM bancável (normalmente ignorada ou usada para MBC)
    } else if (address < 0xA000) {
        // 8000-9FFF: VRAM
        // TODO: Escrever na VRAM
    } else if (address < 0xC000) {
        // A000-BFFF: External RAM
        // TODO: Escrever na RAM externa
    } else if (address < 0xE000) {
        // C000-DFFF: WRAM
        // TODO: Escrever na WRAM
    } else if (address < 0xFE00) {
        // E000-FDFF: Echo RAM (mirror de C000-DDFF)
        // TODO: Mirror de C000-DDFF
    } else if (address < 0xFEA0) {
        // FE00-FE9F: OAM
        // TODO: Escrever na OAM
    } else if (address < 0xFF00) {
        // FEA0-FEFF: Não utilizável
        // TODO: Ignorar ou comportamento especial
    } else if (address < 0xFF80) {
        // FF00-FF7F: I/O Registers
        // TODO: Escrever nos registradores de I/O
    } else if (address < 0xFFFF) {
        // FF80-FFFE: HRAM
        // TODO: Escrever na HRAM
    } else if (address == 0xFFFF) {
        // FFFF: Interrupt Enable Register
        // TODO: Escrever no IE
    }
    // Por enquanto, escreve na memória linear (placeholder)
    if (address < emu->mmu.size) {
        emu->mmu.memory[address] = value;
    }
}
