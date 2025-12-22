#include <emulator.h>
#include <cartridge.h>
#include <ppu.h>        
#include <interrupts.h> 
#include <stdio.h>
#include <stdlib.h>

void initialize_emulator(Emulator* emu, char* cartridge_path)
{
    if (load_cartridge(cartridge_path, &emu->cart) != 0) {
        printf("Error loading cartridge\n");
        exit(1);
    }

    initialize_cpu(emu);
    initialize_mmu(emu);
    ppu_init(emu); // <--- Inicializa a PPU (zera contadores, define modo)

    emu->is_running = true;
    emu->paused = false;
    emu->debug_mode = false;
}

void emu_run(int argc, char** argv)
{
    Emulator emu;
    initialize_emulator(&emu, argv[1]);

    while (emu.is_running) {
        if (emu.paused) {
            // Se implementar pause no futuro, pode colocar um sleep aqui
            continue;
        }

        // 1. Executa a CPU
        // O ideal é mudar a assinatura de cpu_step para: int cpu_step(Emulator* emu)
        // Por enquanto, se ela retorna void, vamos assumir 4 ciclos (mínimo de um NOP)
        
        // --- SE cpu_step RETORNA VOID (Seu código atual): ---
        cpu_step(&emu);
        int cycles = 4; // Valor estimado fixo para teste
        
        /* --- SE VOCÊ MUDAR cpu_step PARA RETORNAR INT (Recomendado): ---
        int cycles = cpu_step(&emu);
        */

        // 2. Executa a PPU
        // A PPU avança baseada no tempo que a CPU gastou
        ppu_step(&emu, cycles);

        // 3. Gerencia Interrupções
        // Verifica se a PPU (ou timer) pediu interrupção e acorda a CPU do HALT
        handle_interrupts(&emu);
    }
}