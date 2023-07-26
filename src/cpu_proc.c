#include <common.h>
#include <cpu.h>

static void proc_none(cpu_ctx* ctx)
{

}

static void proc_nop(cpu_ctx* ctx)
{

}

static void proc_ld(cpu_ctx* ctx)
{

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

}

static void proc_xor(cpu_ctx* ctx)
{

}

static void proc_or(cpu_ctx* ctx)
{

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

}

static void proc_ei(cpu_ctx* ctx)
{

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