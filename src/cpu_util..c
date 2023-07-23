#include <cpu.h>

extern cpu_ctx ctx;

u16 reverse(u16 n)
{
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

u16 cpu_read_reg(Register_type rt)
{
    switch (rt)
    {
        case RT_A: return ctx.registers.a;
        case RT_F: return ctx.registers.f;
        case RT_B: return ctx.registers.b;
        case RT_C: return ctx.registers.c;
        case RT_D: return ctx.registers.d;
        case RT_E: return ctx.registers.e;
        case RT_H: return ctx.registers.h;
        case RT_L: return ctx.registers.l;

        case RT_AF: return reverse(*((u16*)&ctx.registers.a));
        case RT_BC: return reverse(*((u16*)&ctx.registers.b));
        case RT_DE: return reverse(*((u16*)&ctx.registers.d));
        case RT_HL: return reverse(*((u16*)&ctx.registers.h));

        case RT_PC: return ctx.registers.pc;
        case RT_SP: return ctx.registers.sp;

        default: return 0;
    }
}