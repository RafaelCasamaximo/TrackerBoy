#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct Emulator Emulator;

// Cores padrão (ARGB) - Estilo esverdeado clássico ou P&B
#define COLOR_WHITE 0xFF9BBC0F // Cor 0
#define COLOR_LIGHT 0xFF8BAC0F // Cor 1
#define COLOR_DARK  0xFF306230 // Cor 2
#define COLOR_BLACK 0xFF0F380F // Cor 3

// Modos da PPU
typedef enum {
    PPU_MODE_HBLANK = 0,
    PPU_MODE_VBLANK = 1,
    PPU_MODE_OAM_SEARCH = 2,
    PPU_MODE_PIXEL_TRANSFER = 3
} PpuMode;

typedef struct PPU {
    int cycle_counter;
    uint8_t current_line; // LY
    PpuMode mode;

    // Buffer de vídeo: 160x144 pixels
    uint32_t framebuffer[160 * 144];

} PPU;

void ppu_init(Emulator* emu);
void ppu_step(Emulator* emu, int cpu_cycles);