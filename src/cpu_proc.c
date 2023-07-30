#include <common.h>
#include <cpu.h>
#include <emulator.h>
#include <bus.h>
#include <stack.h>

static bool check_condition(cpu_ctx* ctx)
{
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (ctx->curr_inst->condition)
    {
        case CT_NONE: return true;
        case CT_C: return c;
        case CT_NC: return !c;
        case CT_Z: return z;
        case CT_NZ: return !z;
    }

    return false;
}

static void cpu_set_flags(cpu_ctx* ctx, char z, char n, char h, char c)
{
    if(z != -1)
    {
        BIT_SET(ctx->registers.f, 7, z);
    }
    if(n != -1)
    {
        BIT_SET(ctx->registers.f, 6, n);
    }
    if(h != -1)
    {
        BIT_SET(ctx->registers.f, 5, h);
    }
    if(c != -1)
    {
        BIT_SET(ctx->registers.f, 4, c);
    }
}

static void goto_addr(cpu_ctx* ctx, u16 addr, bool pushpc)
{
    if(check_condition(ctx))
    {

        if(pushpc)
        {
            emu_cycles(2);
            stack_push16(ctx->registers.pc);
        }

        ctx->registers.pc = addr;
        emu_cycles(1);
    }
}

static void proc_none(cpu_ctx* ctx)
{
    ERROR("INVALID INSTRUCTION DETECTED: %02X @ %x", ctx->curr_opcode, ctx->registers.pc);
}

static void proc_nop(cpu_ctx* ctx)
{
    return;
}

static void proc_ld(cpu_ctx* ctx)
{
    // Special Case: Destination is memory
    if(ctx->dest_is_mem)
    {
        // 16bit register
        if(is_16bit_register(ctx->curr_inst->register_2))
        {
            emu_cycles(1);
            bus_write16(ctx->mem_dest, ctx->fetched_data);
        }
        // 8bit register
        else
        {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }

        emu_cycles(1);
        
        return;
    }

    if(ctx->curr_inst->instruction_type == AM_HL_SPR)
    {
        u8 hflag = (cpu_read_reg(ctx->curr_inst->register_2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        u8 cflag = (cpu_read_reg(ctx->curr_inst->register_2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;
        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->curr_inst->register_1, cpu_read_reg(ctx->curr_inst->register_2) + (char)ctx->fetched_data);
        return;
    }

    cpu_set_reg(ctx->curr_inst->register_1, ctx->fetched_data);   
}

static void proc_inc(cpu_ctx* ctx)
{
    u16 val = cpu_read_reg(ctx->curr_inst->register_1) + 1;
    
    if(is_16bit_register(ctx->curr_inst->register_1))
    {
        emu_cycles(1);
    }

    if(ctx->curr_inst->register_1 == RT_HL && ctx->curr_inst->address_mode == AM_MR)
    {
        val = bus_read(cpu_read_reg(RT_HL)) + 1;
        val &= 0xFF;
        bus_write(cpu_read_reg(RT_HL), val);
    }
    else
    {
        cpu_set_reg(ctx->curr_inst->register_1, val);
        val = cpu_read_reg(ctx->curr_inst->register_1);
    }

    if((ctx->curr_opcode == 0x03) == 0x03)
    {
        return;
    }

    cpu_set_flags(ctx, val == 0, 0, (val & 0x0F) == 0, -1);
}

static void proc_dec(cpu_ctx* ctx)
{
    u16 val = cpu_read_reg(ctx->curr_inst->register_1) - 1;
    
    if(is_16bit_register(ctx->curr_inst->register_1))
    {
        emu_cycles(1);
    }

    if(ctx->curr_inst->register_1 == RT_HL && ctx->curr_inst->address_mode == AM_MR)
    {
        val = bus_read(cpu_read_reg(RT_HL)) - 1;
        bus_write(cpu_read_reg(RT_HL), val);
    }
    else
    {
        cpu_set_reg(ctx->curr_inst->register_1, val);
        val = cpu_read_reg(ctx->curr_inst->register_1);
    }

    if((ctx->curr_opcode == 0x0B) == 0x0B)
    {
        return;
    }

    cpu_set_flags(ctx, val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

static void proc_rlca(cpu_ctx* ctx)
{

}

static void proc_add(cpu_ctx* ctx)
{
    u32 val = cpu_read_reg(ctx->curr_inst->register_1) + ctx->fetched_data;

    bool is_16bit = is_16bit_register(ctx->curr_inst->register_1);

    if(is_16bit)
    {
        emu_cycles(1);
    }

    if(ctx->curr_inst->register_1 == RT_SP)
    {
        val = cpu_read_reg(ctx->curr_inst->register_1) + (char)ctx->fetched_data;
    }

    int z = (val & 0xFF) == 0;
    int h = (cpu_read_reg(ctx->curr_inst->register_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
    int c = (int)(cpu_read_reg(ctx->curr_inst->register_1) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;
    
    if(is_16bit)
    {
        z = -1;
        h = (cpu_read_reg(ctx->curr_inst->register_1) & 0xFFF) + (ctx->fetched_data & 0xFFF) >= 0x1000;
        u32 n = ((u32)cpu_read_reg(ctx->curr_inst->register_1)) + ((u32)ctx->fetched_data);
        c = n >= 0x10000;
    }

    if(ctx->curr_inst->register_1 == RT_SP)
    {
        z = 0;
        h = (cpu_read_reg(ctx->curr_inst->register_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        c = (int)(cpu_read_reg(ctx->curr_inst->register_1) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;
    }

    cpu_set_reg(ctx->curr_inst->register_1, val & 0xFFFF);
    cpu_set_flags(ctx, z, 0, h, c);
}

static void proc_rrca(cpu_ctx* ctx)
{

}

static void proc_stop(cpu_ctx* ctx)
{

}

static void proc_rla(cpu_ctx* ctx)
{

}

static void proc_jr(cpu_ctx* ctx)
{
    char rel = (char)(ctx->fetched_data & 0xFF);
    u16 addr = ctx->registers.pc + rel;
    goto_addr(ctx, addr, false);
}

static void proc_rra(cpu_ctx* ctx)
{

}

static void proc_daa(cpu_ctx* ctx)
{

}

static void proc_cpl(cpu_ctx* ctx)
{

}

static void proc_scf(cpu_ctx* ctx)
{

}

static void proc_ccf(cpu_ctx* ctx)
{

}

static void proc_halt(cpu_ctx* ctx)
{

}

static void proc_adc(cpu_ctx* ctx)
{
    u16 u = ctx->fetched_data;
    u16 a = ctx->registers.a;
    u16 c = CPU_FLAG_C;

    ctx->registers.a = (a + u + c) & 0xFF;

    cpu_set_flags(ctx, ctx->registers.a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
}

static void proc_sub(cpu_ctx* ctx)
{
    u16 val = cpu_read_reg(ctx->curr_inst->register_1) - ctx->fetched_data;

    int z = val == 0;
    int h = ((int)cpu_read_reg(ctx->curr_inst->register_1) & 0xF) - ((int)cpu_read_reg(ctx->fetched_data) & 0xF) < 0;
    int c = ((int)cpu_read_reg(ctx->curr_inst->register_1)) - ((int)cpu_read_reg(ctx->fetched_data)) < 0;

    cpu_set_reg(ctx->curr_inst->register_1, val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_sbc(cpu_ctx* ctx)
{
    u16 val = ctx->fetched_data + CPU_FLAG_C;

    int z = cpu_read_reg(ctx->curr_inst->register_1) - val == 0;
    int h = ((int)cpu_read_reg(ctx->curr_inst->register_1) & 0xF) - ((int)cpu_read_reg(ctx->fetched_data) & 0xF) - ((int)CPU_FLAG_C) < 0;
    int c = ((int)cpu_read_reg(ctx->curr_inst->register_1)) - ((int)cpu_read_reg(ctx->fetched_data)) - ((int)CPU_FLAG_C) < 0;

    cpu_set_reg(ctx->curr_inst->register_1, cpu_read_reg(ctx->curr_inst->register_1) - val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_and(cpu_ctx* ctx)
{
    ctx->registers.a &= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

static void proc_xor(cpu_ctx* ctx)
{
    ctx->registers.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

static void proc_or(cpu_ctx* ctx)
{
    ctx->registers.a |= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->registers.a == 0, 0, 0, 0);
}

static void proc_cp(cpu_ctx* ctx)
{

}

static void proc_ret(cpu_ctx* ctx)
{

    if(ctx->curr_inst->condition != CT_NONE)
    {
        emu_cycles(1);
    }

    if(check_condition(ctx))
    {
        u16 lo = stack_pop();
        emu_cycles(1);
        u16 hi = stack_pop();
        emu_cycles(1);

        u16 n = (hi << 8) | lo;
        ctx->registers.pc = n;

        emu_cycles(1);
    }
}

static void proc_pop(cpu_ctx* ctx)
{
    u16 lo = stack_pop();
    emu_cycles(1);
    u16 hi = stack_pop();
    emu_cycles(1);

    u16 n = (hi << 8) | lo;

    cpu_set_reg(ctx->curr_inst->register_1, n);

    if(ctx->curr_inst->register_1 == RT_AF)
    {
        cpu_set_reg(ctx->curr_inst->register_1, n & 0xFFF0);
    }
}

static void proc_jp(cpu_ctx* ctx)
{
    goto_addr(ctx, ctx->fetched_data, false);
}

static void proc_call(cpu_ctx* ctx)
{
    goto_addr(ctx, ctx->fetched_data, true);
}

static void proc_push(cpu_ctx* ctx)
{
    u16 hi = (cpu_read_reg(ctx->curr_inst->register_1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi);

    u16 lo = cpu_read_reg(ctx->curr_inst->register_1) & 0xFF;
    emu_cycles(1);
    stack_push(lo);

    emu_cycles(1);
}

static void proc_rst(cpu_ctx* ctx)
{
    goto_addr(ctx, ctx->curr_inst->param, true);
}

static void proc_cb(cpu_ctx* ctx)
{

} 

static void proc_reti(cpu_ctx* ctx)
{
    ctx->int_master_enabled = true;
    proc_ret(ctx);
}

static void proc_ldh(cpu_ctx* ctx)
{
    if(ctx->curr_inst->register_1 == RT_A)
    {
        cpu_set_reg(RT_A, bus_read(0xFF00 | ctx->fetched_data));
    }
    else{
        bus_write(0xFF | ctx->fetched_data, ctx->registers.a);
    }

    emu_cycles(1);
}

static void proc_di(cpu_ctx* ctx)
{
    ctx->int_master_enabled = false;
}

static void proc_ei(cpu_ctx* ctx)
{
    ctx->int_master_enabled = true;
}

// LD HL,SP+r8 has alternative mnemonic LDHL SP,r8
static void proc_ldhl(cpu_ctx* ctx)
{

}


static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_INC] = proc_inc,
    [IN_DEC] = proc_dec,
    [IN_RLCA] = proc_rlca,
    [IN_ADD] = proc_add,
    [IN_RRCA] = proc_rrca,
    [IN_STOP] = proc_stop,
    [IN_RLA] = proc_rla,
    [IN_JR] = proc_jr,
    [IN_RRA] = proc_rra,
    [IN_DAA] = proc_daa,
    [IN_CPL] = proc_cpl,
    [IN_SCF] = proc_scf,
    [IN_CCF] = proc_ccf,
    [IN_HALT] = proc_halt,
    [IN_ADC] = proc_adc,
    [IN_SUB] = proc_sub,
    [IN_SBC] = proc_sbc,
    [IN_AND] = proc_and,
    [IN_XOR] = proc_xor,
    [IN_OR] = proc_or,
    [IN_CP] = proc_cp,
    [IN_RET] = proc_ret,
    [IN_POP] = proc_pop,
    [IN_JP] = proc_jp,
    [IN_CALL] = proc_call,
    [IN_PUSH] = proc_push,
    [IN_RST] = proc_rst,
    [IN_CB] = proc_cb, 
    [IN_RETI] = proc_reti,
    [IN_LDH] = proc_ldh,
    [IN_DI] = proc_di,
    [IN_EI] = proc_ei,
    // LD HL,SP+r8 has alternative mnemonic LDHL SP,r8
    [IN_LDHL] = proc_ldhl,
};

IN_PROC inst_get_processor(Instruction_type type)
{
    return processors[type];
}