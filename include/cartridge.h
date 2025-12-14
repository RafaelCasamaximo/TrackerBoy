#pragma once

#include <stdint.h>

typedef struct {
    uint8_t *rom_data;      // Ponteiro para todos os dados da ROM
    uint32_t rom_size;      // Tamanho total em bytes
    
    uint8_t *ram_data;      // RAM externa (bateria de save)
    uint32_t ram_size;
    
    uint8_t mbc_type;       // Valor do endereço 0x0147
    uint8_t rom_bank_count; // Quantos bancos de 16KB existem
    
    char title[17];         // Título do jogo (ASCII)
} Cartridge;

int load_cartridge(const char* filepath, Cartridge* cart);
void free_cartridge(Cartridge* cart);
void write_cartridge(Cartridge* cart, uint16_t address, uint8_t value);
uint8_t read_cartridge(Cartridge* cart, uint16_t address);
