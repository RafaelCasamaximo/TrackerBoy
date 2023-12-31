#include <cpu.h>
#include <bus.h>
#include <emulator.h>
#include <common.h>
#include <debug.h>

cpu_ctx ctx = {0};

void cpu_init()
{
    ctx.registers.pc = 0x100;
    ctx.halted = false;
}

/*
    Read opcode from ROM
    Increases PC in the process
    Get instruction by opcode
*/
void fetch_instruction()
{
    ctx.curr_opcode = bus_read(ctx.registers.pc++);
    ctx.curr_inst = intruction_by_opcode(ctx.curr_opcode);

    if(ctx.curr_inst == NULL)
    {
        ERROR("Unknown Instruction: %02X @ %x", ctx.curr_opcode, ctx.registers.pc - 1);
    }
}

void execute()
{
    IN_PROC proc = inst_get_processor(ctx.curr_inst->instruction_type);
    if(!proc)
    {
        return;
    }
    proc(&ctx);
}

bool cpu_step()
{
    
    if(!ctx.halted)
    {
        u16 pc = ctx.registers.pc;

        fetch_instruction();
        fetch_data();

        debug_print();
        // log_debug("%04X: (%02X) %-7s A: 0x%02X BC: 0x%02X%02X DE: 0x%02X%02X HL: 0x%02X%02X %c,%c,%c,%c",
        // pc,
        // ctx.curr_opcode,
        // inst_name_by_type(ctx.curr_inst->instruction_type),
        // ctx.registers.a,
        // ctx.registers.b, ctx.registers.c,
        // ctx.registers.d, ctx.registers.e,
        // ctx.registers.h, ctx.registers.l,
        // BIT(ctx.registers.f, 7) ? 'Z' : '-',
        // BIT(ctx.registers.f, 6) ? 'N' : '-',
        // BIT(ctx.registers.f, 5) ? 'H' : '-',
        // BIT(ctx.registers.f, 4) ? 'C' : '-');
        execute();
    }

    return true;
}

u8 cpu_get_ie_register()
{
    return ctx.ie_register;
}

void cpu_set_ie_register(u8 value)
{
    ctx.ie_register = value;
}
