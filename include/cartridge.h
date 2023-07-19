#pragma once

#include "common.h"

/**
 * Retrieved from: https://gbdev.io/pandocs/The_Cartridge_Header.html
*/

typedef struct {
    u8 entry_point[0x4];        // 0100-0103
    u8 logo[0x30];              // 0104-0133
    char title[0x10];           // 0134-0143 (Does not count Manufacturer code and CGB flag)
    u16 new_licensee_code;      // 0144-0145
    u8 sgb_flag;                // 0146
    u8 type;                    // 0147
    u8 rom_size;                // 0148 ROM Size = 32 KiB × (1 << <value>)
    u8 ram_size;                // 0149
    u8 dest_code;               // 014A
    u8 lic_code;                // 014B
    u8 version;                 // 014C
    u8 checksum;                // 014D
    u16 global_checksum;        // 014E-014F
}cartridge_header;

int cartridge_load(char* file_name);