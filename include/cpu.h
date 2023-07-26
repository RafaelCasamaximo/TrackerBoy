#pragma once

#include <common.h>
#include <instructions.h>

typedef struct{
    u8 a, f;
    u8 b, c;
    u8 d, e;
    u8 h, l;
    u16 sp;
    u16 pc;
} cpu_registers;

typedef struct{
    // CPU Registers State
    cpu_registers registers;

    // Current fetch/ cycle
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 curr_opcode;
    Instruction* curr_inst;

    // CPU settings Flags
    bool halted;
    bool stepping;

    // Interruptions enabled or disabled (for DI and EI)
    bool int_master_enabled;
} cpu_ctx;

void cpu_init();
bool cpu_step();

static void fetch_instruction();
static void fetch_data();
static void execute();

u16 cpu_read_reg(Register_type rt);

typedef void (*IN_PROC)(cpu_ctx *);
IN_PROC inst_get_processor(Instruction_type type);

#define CPU_FLAG_Z BIT(ctx->registers.f, 7)
#define CPU_FLAG_C BIT(ctx->registers.f, 4)