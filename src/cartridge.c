#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartridge.h"

int load_cartridge(const char *filepath, Cartridge *cart) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return -1;
    }

    // Get ROM size
    fseek(file, 0, SEEK_END);
    cart->rom_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for ROM data
    cart->rom_data = (uint8_t*)malloc(cart->rom_size);
    if (!cart->rom_data) {
        fclose(file);
        return -2;
    }

    // Read ROM data
    if (fread(cart->rom_data, 1, cart->rom_size, file) != cart->rom_size) {
        free(cart->rom_data);
        fclose(file);
        return -3;
    }

    fclose(file);

    // Extract title (0x0134-0x0143, 16 bytes, ASCII, not null-terminated)
    memcpy(cart->title, &cart->rom_data[0x0134], 16);
    cart->title[16] = '\0';

    // MBC type (0x0147)
    cart->mbc_type = cart->rom_data[0x0147];

    // ROM size (0x0148)
    uint8_t rom_size_code = cart->rom_data[0x0148];
    cart->rom_bank_count = 2 << rom_size_code; // 2^(n+1) banks of 16KB

    // RAM size (0x0149)
    uint8_t ram_size_code = cart->rom_data[0x0149];
    switch (ram_size_code) {
        case 0: cart->ram_size = 0; break;
        case 1: cart->ram_size = 2 * 1024; break;
        case 2: cart->ram_size = 8 * 1024; break;
        case 3: cart->ram_size = 32 * 1024; break;
        case 4: cart->ram_size = 128 * 1024; break;
        case 5: cart->ram_size = 64 * 1024; break;
        default: cart->ram_size = 0; break;
    }

    if (cart->ram_size > 0) {
        cart->ram_data = (uint8_t*)calloc(1, cart->ram_size);
        if (!cart->ram_data) {
            free(cart->rom_data);
            return -4;
        }
    } else {
        cart->ram_data = NULL;
    }

    return 0;
}

void free_cartridge(Cartridge *cart) {
    if (cart->rom_data) {
        free(cart->rom_data);
        cart->rom_data = NULL;
    }
    if (cart->ram_data) {
        free(cart->ram_data);
        cart->ram_data = NULL;
    }
}
// Basic read/write for ROM-only cartridges (no MBC support)

void write_cartridge(Cartridge *cart, uint16_t address, uint8_t value)
{
    // ROM-only: writes have no effect
    (void)cart;
    (void)address;
    (void)value;
    // TODO: Retornar erro ou logar tentativa de escrita em ROM
}

uint8_t read_cartridge(Cartridge *cart, uint16_t address)
{
    if (address < cart->rom_size) {
        return cart->rom_data[address];
    }
    return 0xFF; // Open bus
}
