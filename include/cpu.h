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
    u8 ie_register;
} cpu_ctx;

// CPU FETCH
void fetch_instruction();
void fetch_data();
void execute();

// CPU
void cpu_init();
bool cpu_step();

// CPU UTIL
cpu_registers* cpu_get_registers();
u16 cpu_read_reg(Register_type rt);
void cpu_set_reg(Register_type type, u16 value);
bool is_16bit_register(Register_type type);
u8 cpu_read_reg8(Register_type rt);
void cpu_set_reg8(Register_type rt, u8 value);

// CPU PROC
typedef void (*IN_PROC)(cpu_ctx *);
IN_PROC inst_get_processor(Instruction_type type);

#define CPU_FLAG_Z BIT(ctx->registers.f, 7)
#define CPU_FLAG_C BIT(ctx->registers.f, 4)

u8 cpu_get_ie_register();
void cpu_set_ie_register(u8 value);