#pragma once
#include <stdint.h>
#include <stdbool.h>

// Forward declaration para evitar include circular
typedef struct Emulator Emulator;

// Máscaras de Bits do Registrador IF (Interrupt Flag) e IE (Interrupt Enable)
#define INT_VBLANK   0x01  // Bit 0 (Prioridade 1)
#define INT_LCD_STAT 0x02  // Bit 1 (Prioridade 2)
#define INT_TIMER    0x04  // Bit 2 (Prioridade 3)
#define INT_SERIAL   0x08  // Bit 3 (Prioridade 4)
#define INT_JOYPAD   0x10  // Bit 4 (Prioridade 5)

// Endereços dos Vetores de Interrupção
#define VEC_VBLANK   0x0040
#define VEC_LCD_STAT 0x0048
#define VEC_TIMER    0x0050
#define VEC_SERIAL   0x0058
#define VEC_JOYPAD   0x0060

// Função chamada pelos periféricos (PPU, Timer, Joypad) para avisar que algo aconteceu
void request_interrupt(Emulator* emu, uint8_t interrupt_mask);

// Função chamada no loop principal para processar as interrupções pendentes
void handle_interrupts(Emulator* emu);
