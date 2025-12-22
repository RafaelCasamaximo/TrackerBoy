#include <ppu.h>
#include <emulator.h>
#include <mmu.h>
#include <interrupts.h>

// Constantes de Tempo
#define CYCLES_OAM 80
#define CYCLES_TRANSFER 172
#define CYCLES_HBLANK 204
#define CYCLES_PER_LINE 456
#define SCREEN_HEIGHT 144
#define SCREEN_WIDTH 160

// Função auxiliar para desenhar a linha atual no framebuffer
static void ppu_render_scanline(Emulator* emu) {
    uint8_t ly = emu->ppu.current_line;
    
    // Leitura dos registradores de controle
    uint8_t lcdc = mmu_read_byte(emu, 0xFF40);
    uint8_t scy = mmu_read_byte(emu, 0xFF42);
    uint8_t scx = mmu_read_byte(emu, 0xFF43);
    uint8_t bgp = mmu_read_byte(emu, 0xFF47); // Paleta do Background

    // 1. Verifica se o Background está ativado (Bit 0 do LCDC)
    if (!(lcdc & 0x01)) {
        // Se desligado, preenche a linha com branco
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            emu->ppu.framebuffer[ly * SCREEN_WIDTH + x] = COLOR_WHITE;
        }
        return;
    }

    // 2. Determina qual mapa de tiles usar (Bit 3 do LCDC)
    // 0 = 0x9800, 1 = 0x9C00
    uint16_t map_base = (lcdc & 0x08) ? 0x9C00 : 0x9800;

    // 3. Determina onde estão os dados dos tiles (Bit 4 do LCDC)
    // 0 = 0x8800 (Signed), 1 = 0x8000 (Unsigned)
    bool unsigned_mode = (lcdc & 0x10);
    uint16_t tile_data_base = unsigned_mode ? 0x8000 : 0x8800;

    // A linha Y real no mapa de 256x256 (considerando o Scroll Y)
    uint8_t y_map = scy + ly;

    // --- LOOP PIXEL POR PIXEL (0 a 159) ---
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // A coluna X real no mapa (considerando o Scroll X)
        uint8_t x_map = scx + x;

        // --- Passo A: Achar o Tile ---
        // O mapa tem 32x32 tiles. Cada tile tem 8x8 pixels.
        // Índice = (y / 8) * 32 + (x / 8)
        uint16_t tile_index_addr = map_base + ((y_map / 8) * 32) + (x_map / 8);
        uint8_t tile_id = mmu_read_byte(emu, tile_index_addr);

        // --- Passo B: Achar o endereço dos dados do Tile ---
        uint16_t tile_addr;
        if (unsigned_mode) {
            tile_addr = tile_data_base + (tile_id * 16); // Cada tile ocupa 16 bytes
        } else {
            // Modo 0x8800 usa IDs assinados (-128 a 127)
            // 0 fica em 0x9000, -128 em 0x8800
            tile_addr = 0x9000 + ((int8_t)tile_id * 16);
        }

        // --- Passo C: Ler a linha do Tile ---
        // Qual linha (0-7) dentro do tile estamos desenhando?
        // Multiplica por 2 porque cada linha usa 2 bytes (low e high bitplanes)
        uint8_t line_in_tile = (y_map % 8) * 2;
        
        uint8_t byte1 = mmu_read_byte(emu, tile_addr + line_in_tile);
        uint8_t byte2 = mmu_read_byte(emu, tile_addr + line_in_tile + 1);

        // --- Passo D: Decodificar a cor do pixel ---
        // Bit 7 é o pixel da esquerda (0), Bit 0 é o pixel da direita (7)
        int bit_idx = 7 - (x_map % 8);
        
        uint8_t color_bit1 = (byte1 >> bit_idx) & 1;
        uint8_t color_bit2 = (byte2 >> bit_idx) & 1;
        
        // Combina para formar o ID da cor (0 a 3)
        uint8_t color_id = (color_bit2 << 1) | color_bit1;

        // --- Passo E: Aplicar a Paleta (BGP) ---
        // A paleta mapeia o ID da cor para a cor real
        // Bits 0-1: Cor para ID 0, Bits 2-3: Cor para ID 1...
        uint8_t actual_color = (bgp >> (color_id * 2)) & 0x03;

        // Define o pixel no framebuffer
        uint32_t final_color = COLOR_WHITE;
        switch(actual_color) {
            case 0: final_color = COLOR_WHITE; break;
            case 1: final_color = COLOR_LIGHT; break;
            case 2: final_color = COLOR_DARK; break;
            case 3: final_color = COLOR_BLACK; break;
        }

        emu->ppu.framebuffer[ly * SCREEN_WIDTH + x] = final_color;
    }
}

// Atualiza o registrador STAT (0xFF41) com o modo atual
// Os 2 bits inferiores de STAT indicam o modo PPU
static void update_stat_mode(Emulator* emu, PpuMode mode) {
    uint8_t stat = mmu_read_byte(emu, 0xFF41);
    stat &= ~0x03; // Limpa bits 0 e 1
    stat |= (mode & 0x03); // Define novo modo
    mmu_write_byte(emu, 0xFF41, stat);
}

void ppu_step(Emulator* emu, int cpu_cycles) {
    emu->ppu.cycle_counter += cpu_cycles;

    switch (emu->ppu.mode) {
        // --- MODO 2: OAM SEARCH (80 ciclos) ---
        case PPU_MODE_OAM_SEARCH:
            if (emu->ppu.cycle_counter >= CYCLES_OAM) {
                emu->ppu.cycle_counter -= CYCLES_OAM;
                emu->ppu.mode = PPU_MODE_PIXEL_TRANSFER;
                update_stat_mode(emu, PPU_MODE_PIXEL_TRANSFER);
            }
            break;

        // --- MODO 3: PIXEL TRANSFER (172 ciclos) ---
        case PPU_MODE_PIXEL_TRANSFER:
            if (emu->ppu.cycle_counter >= CYCLES_TRANSFER) {
                emu->ppu.cycle_counter -= CYCLES_TRANSFER;
                
                // Mude para HBlank
                emu->ppu.mode = PPU_MODE_HBLANK;
                update_stat_mode(emu, PPU_MODE_HBLANK);

                // Desenha a linha AGORA (fim do modo 3)
                // Se implementarmos sprites, seria aqui também
                ppu_render_scanline(emu);
                
                // TODO: HBlank Interrupt (STAT) se habilitado
            }
            break;

        // --- MODO 0: HBLANK (204 ciclos) ---
        case PPU_MODE_HBLANK:
            if (emu->ppu.cycle_counter >= CYCLES_HBLANK) {
                emu->ppu.cycle_counter -= CYCLES_HBLANK;
                
                // Terminou a linha, avança LY
                emu->ppu.current_line++;
                mmu_write_byte(emu, 0xFF44, emu->ppu.current_line);

                // Se chegou na linha 144, entra em VBlank
                if (emu->ppu.current_line == SCREEN_HEIGHT) {
                    emu->ppu.mode = PPU_MODE_VBLANK;
                    update_stat_mode(emu, PPU_MODE_VBLANK);
                    
                    request_interrupt(emu, INT_VBLANK); // Dispara INT VBlank
                } else {
                    // Se não, volta para OAM Search da próxima linha
                    emu->ppu.mode = PPU_MODE_OAM_SEARCH;
                    update_stat_mode(emu, PPU_MODE_OAM_SEARCH);
                }
            }
            break;

        // --- MODO 1: VBLANK (4560 ciclos / 10 linhas) ---
        case PPU_MODE_VBLANK:
            if (emu->ppu.cycle_counter >= CYCLES_PER_LINE) {
                emu->ppu.cycle_counter -= CYCLES_PER_LINE;
                
                emu->ppu.current_line++;
                mmu_write_byte(emu, 0xFF44, emu->ppu.current_line);

                // Se passou da linha 153, reinicia o quadro
                if (emu->ppu.current_line >= 154) {
                    emu->ppu.current_line = 0;
                    mmu_write_byte(emu, 0xFF44, 0);
                    
                    emu->ppu.mode = PPU_MODE_OAM_SEARCH;
                    update_stat_mode(emu, PPU_MODE_OAM_SEARCH);
                }
            }
            break;
    }
}

void ppu_init(Emulator* emu) {
    emu->ppu.cycle_counter = 0;
    emu->ppu.current_line = 0;
    emu->ppu.mode = PPU_MODE_OAM_SEARCH;
    // Opcional: Limpar framebuffer com branco/preto
}