#include <common.h>
#include <cpu.h>
#include <emulator.h>
#include <bus.h>

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
        if(!is_16bit_register(ctx->curr_inst->register_2))
        {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }
        return;
    }

    if(ctx->curr_inst->instruction_type == AM_HL_SPR)
    {
        u8 hflag = (cpu_read_reg(ctx->curr_inst->register_2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        u8 cflag = (cpu_read_reg(ctx->curr_inst->register_2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;
        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->curr_inst->register_1, cpu_read_reg(ctx->curr_inst->register_2) + (char)ctx->fetched_data);
    }

    cpu_set_reg(ctx->curr_inst->register_1, ctx->fetched_data);   
}

static void proc_inc(cpu_ctx* ctx)
{

}

static void proc_dec(cpu_ctx* ctx)
{

}

static void proc_rlca(cpu_ctx* ctx)
{

}

static void proc_add(cpu_ctx* ctx)
{

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

}

static void proc_sub(cpu_ctx* ctx)
{

}

static void proc_sbc(cpu_ctx* ctx)
{

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

}

static void proc_pop(cpu_ctx* ctx)
{

}

static void proc_jp(cpu_ctx* ctx)
{
    if(check_condition(ctx))
    {
        ctx->registers.pc = ctx->fetched_data;
        emu_cycles(1);
    }
}

static void proc_call(cpu_ctx* ctx)
{

}

static void proc_push(cpu_ctx* ctx)
{

}

static void proc_rst(cpu_ctx* ctx)
{

}

static void proc_cb(cpu_ctx* ctx)
{

} 

static void proc_reti(cpu_ctx* ctx)
{

}

static void proc_ldh(cpu_ctx* ctx)
{

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