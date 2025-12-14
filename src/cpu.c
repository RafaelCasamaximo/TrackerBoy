#include <cpu.h>

void initialize_cpu(CPU* cpu)
{
    cpu->A = 0;
    cpu->F = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->E = 0;
    cpu->H = 0;
    cpu->L = 0;

    cpu->PC = 0x0000; // Program Counter starts at 0
    cpu->SP = 0xFFFE; // Stack Pointer starts at top of memory

    cpu->halted = false;
    cpu->interrupts_enabled = false;
}