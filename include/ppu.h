#pragma once

#include <stdint.h>
#include <stdbool.h>

// Forward declaration
typedef struct Emulator Emulator;

// Modos da PPU (STAT Register)
// O Game Boy cicla por estes modos a cada linha desenhada
typedef enum {
    PPU_MODE_HBLANK = 0,      // Modo 0: Intervalo horizontal (CPU pode acessar VRAM/OAM)
    PPU_MODE_VBLANK = 1,      // Modo 1: Intervalo vertical (CPU pode acessar tudo)
    PPU_MODE_OAM_SEARCH = 2,  // Modo 2: Buscando sprites na OAM (OAM bloqueada)
    PPU_MODE_PIXEL_TRANSFER = 3 // Modo 3: Enviando pixels para o LCD (VRAM/OAM bloqueadas)
} PpuMode;

typedef struct PPU {
    // Estado da Temporização
    int cycle_counter;      // Acumulador de ciclos da CPU
    uint8_t current_line;   // Registrador LY (0-153)
    PpuMode mode;           // Modo atual (0-3)

    // Aqui futuramente entrarão coisas como:
    // uint32_t framebuffer[160 * 144];
    // Paletas, Scroll X/Y, Window X/Y, etc.
} PPU;

// Inicializa variáveis
void ppu_init(Emulator* emu);

// Chamado a cada iteração do loop principal para avançar o tempo
void ppu_step(Emulator* emu, int cpu_cycles);
