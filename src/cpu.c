#include <cpu.h>
#include <bus.h>
#include <emulator.h>
#include <common.h>

cpu_ctx ctx = {0};

void cpu_init()
{
    ctx.registers.pc = 0x100;
    ctx.halted = false;
    NO_IMPL
}

/*
    Read opcode from ROM
    Increases PC in the process
    Get instruction by opcode
*/
static void fetch_instruction()
{
    ctx.curr_opcode = bus_read(ctx.registers.pc++);
    ctx.curr_inst = intruction_by_opcode(ctx.curr_opcode);

    if(ctx.curr_inst == NULL)
    {
        ERROR("Unknown Instruction: %02X @ %x", ctx.curr_opcode, ctx.registers.pc - 1);
    }
}


/*
    Fetch the data differently according to the Address Mode of the current instruction
*/
static void fetch_data()
{
    ctx.mem_dest = 0;
    ctx.dest_is_mem = false;

    switch (ctx.curr_inst->address_mode)
    {
        // Do nothing if the AM is implied
        case AM_IMP: return;

        // Set fetched data as content of register 1
        case AM_R:
        {
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_1);
            return;
        }

        // Set fetched data as content of the value of PC (u8)
        // Increment PC
        case AM_R_D8:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;
        }

        // Set fetched data as content of the PC and PC+1 (u16)
        case AM_A16:
        case AM_D16:
        {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            // Combine lo and hi to form hilo. Example:
            // lo = 0xAA, hi = 0xBC => hi << 8 = 0xBC00 => 0x00AA | 0xBC00 => 0xBCAA
            ctx.fetched_data = lo | (hi << 8);
            ctx.registers.pc += 2;

            return;
        }

        default:
            // ERROR("Unknown Addressing Mode: %d", ctx.curr_inst->address_mode);
    }
}

static void execute()
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
        fetch_instruction();
        fetch_data();
        log_debug("%04X: %-7s", ctx.registers.pc, inst_name_by_type(ctx.curr_inst->instruction_type));
        execute();
    }

    return true;
}