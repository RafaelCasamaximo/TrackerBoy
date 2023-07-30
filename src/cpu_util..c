#include <cpu.h>

extern cpu_ctx ctx;

u16 reverse(u16 n)
{
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

bool is_16bit_register(Register_type type)
{
    return (type == RT_AF || type == RT_BC || type == RT_DE || type == RT_HL);
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

void cpu_set_reg(Register_type type, u16 value)
{
    switch (type)
    {
        case RT_A: ctx.registers.a = value & 0xFF; break;
        case RT_F: ctx.registers.f = value & 0xFF; break;
        case RT_B: ctx.registers.b = value & 0xFF; break;
        case RT_C: ctx.registers.c = value & 0xFF; break;
        case RT_D: ctx.registers.d = value & 0xFF; break;
        case RT_E: ctx.registers.e = value & 0xFF; break;
        case RT_H: ctx.registers.h = value & 0xFF; break;
        case RT_L: ctx.registers.l = value & 0xFF; break;

        case RT_AF: *((u16*)&ctx.registers.a) = reverse(value); break;
        case RT_BC: *((u16*)&ctx.registers.b) = reverse(value); break;
        case RT_DE: *((u16*)&ctx.registers.d) = reverse(value); break;
        case RT_HL: *((u16*)&ctx.registers.h) = reverse(value); break;

        case RT_PC: ctx.registers.pc = value; break;
        case RT_SP: ctx.registers.sp = value; break;
        case RT_NONE: ; break;
    }
}

cpu_registers* cpu_get_registers()
{
    return &ctx.registers;
}