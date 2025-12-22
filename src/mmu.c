#include "mmu.h"
#include "emulator.h"
#include <stdlib.h>
#include <string.h> // Necessário para memset

void initialize_mmu(Emulator* emu)
{
    emu->mmu.size = 0x10000; // 64KB
    emu->mmu.memory = (uint8_t*)malloc(emu->mmu.size);
    
    // Importante: Limpar a memória com 0 para evitar lixo
    if (emu->mmu.memory) {
        memset(emu->mmu.memory, 0, emu->mmu.size);
    }
}

void free_mmu(Emulator* emu)
{
    if (emu->mmu.memory) {
        free(emu->mmu.memory);
        emu->mmu.memory = NULL;
    }
    emu->mmu.size = 0;
}

uint8_t mmu_read_byte(Emulator* emu, uint16_t address)
{
    // --- ROM (0x0000 - 0x7FFF) ---
    // A ROM é somente leitura e vem do cartucho, não da memória RAM interna
    if (address < 0x8000) {
        // Proteção: verifique se existe dados no cartucho
        if (emu->cart.rom_data) {
            return emu->cart.rom_data[address];
        }
        return 0xFF;
    }

    // --- VRAM (0x8000 - 0x9FFF) ---
    else if (address < 0xA000) {
        return emu->mmu.memory[address];
    }

    // --- External RAM (0xA000 - 0xBFFF) ---
    else if (address < 0xC000) {
        // TODO: Implementar enable/disable de RAM externa (MBC)
        return emu->mmu.memory[address];
    }

    // --- WRAM (0xC000 - 0xDFFF) ---
    else if (address < 0xE000) {
        return emu->mmu.memory[address];
    }

    // --- Echo RAM (0xE000 - 0xFDFF) ---
    // Espelho da WRAM. Ler de E000 é o mesmo que ler de C000.
    else if (address < 0xFE00) {
        return emu->mmu.memory[address - 0x2000];
    }

    // --- OAM (0xFE00 - 0xFE9F) ---
    // Memória de Sprites (Object Attribute Memory)
    else if (address < 0xFEA0) {
        return emu->mmu.memory[address];
    }

    // --- Unusable (0xFEA0 - 0xFEFF) ---
    // Essa área é proibida/não mapeada no hardware real.
    else if (address < 0xFF00) {
        return 0xFF; // Ou 0x00, comportamento indefinido
    }

    // --- I/O Registers (0xFF00 - 0xFF7F) ---
    // Joypad, Timer, Audio, LCD Control...
    else if (address < 0xFF80) {
        // Por enquanto, retorna o valor da memória, 
        // mas futuramente aqui terá switch cases para hardware específico.
        return emu->mmu.memory[address]; 
    }

    // --- HRAM (0xFF80 - 0xFFFE) ---
    // High RAM (Zero Page), muito usada pela pilha (Stack)
    else if (address < 0xFFFF) {
        return emu->mmu.memory[address];
    }

    // --- Interrupt Enable Register (0xFFFF) ---
    else if (address == 0xFFFF) {
        return emu->mmu.memory[0xFFFF];
    }

    return 0;
}

void mmu_write_byte(Emulator* emu, uint16_t address, uint8_t value)
{
    // --- ROM (0x0000 - 0x7FFF) ---
    // Tentar escrever na ROM não altera os dados.
    // É usado para enviar comandos ao controlador de memória (MBC).
    if (address < 0x8000) {
        // TODO: Implementar lógica de MBC (troca de bancos) aqui futuramente.
        // NÃO escreva em emu->mmu.memory[address] aqui!
        return; 
    }

    // --- VRAM (0x8000 - 0x9FFF) ---
    else if (address < 0xA000) {
        emu->mmu.memory[address] = value;
    }

    // --- External RAM (0xA000 - 0xBFFF) ---
    else if (address < 0xC000) {
        emu->mmu.memory[address] = value;
    }

    // --- WRAM (0xC000 - 0xDFFF) ---
    else if (address < 0xE000) {
        emu->mmu.memory[address] = value;
    }

    // --- Echo RAM (0xE000 - 0xFDFF) ---
    // Escrever no espelho também escreve na WRAM original
    else if (address < 0xFE00) {
        emu->mmu.memory[address - 0x2000] = value;
        // Opcional: manter a consistência escrevendo no espelho também
        // emu->mmu.memory[address] = value; 
    }

    // --- OAM (0xFE00 - 0xFE9F) ---
    else if (address < 0xFEA0) {
        emu->mmu.memory[address] = value;
    }

    // --- Unusable (0xFEA0 - 0xFEFF) ---
    else if (address < 0xFF00) {
        // Ignora escritas
        return;
    }

    // --- I/O Registers (0xFF00 - 0xFF7F) ---
    else if (address < 0xFF80) {
        // Interceptações especiais (EX: Resetar timer, DMA transfer)
        // TODO: Implementar lógica de I/O aqui
        
        // Exemplo: Escrever no DIV register (0xFF04) reseta ele para 0
        // if (address == 0xFF04) value = 0;

        emu->mmu.memory[address] = value;
    }

    // --- HRAM (0xFF80 - 0xFFFE) ---
    else if (address < 0xFFFF) {
        emu->mmu.memory[address] = value;
    }

    // --- Interrupt Enable Register (0xFFFF) ---
    else if (address == 0xFFFF) {
        emu->mmu.memory[0xFFFF] = value;
    }
}