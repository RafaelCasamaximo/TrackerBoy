#include <cpu.h>
#include <common.h>
#include <bus.h>
#include <emulator.h>

extern cpu_ctx ctx;

/*
    Fetch the data differently according to the Address Mode of the current instruction
*/
void fetch_data()
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

        // Set fetched data as source register (register 2)
        case AM_R_R:
        {
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_2);
            return;
        }

        // Set fetched data as Register 2 and set dest_is_mem as true
        // If instruction works on HRAM (Uses register C) adds to HRAM page
        case AM_MR_R:
        {
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_2);
            ctx.mem_dest = cpu_read_reg(ctx.curr_inst->register_1);
            ctx.dest_is_mem = true;

            if(ctx.curr_inst->register_1 == RT_C)
            {
                ctx.mem_dest |= 0xFF00;
            }

            return;
        }

        // Read memory of a location pointed by a register
        // If its register C adds to HRAM
        case AM_R_MR:
        {
            u16 addr = cpu_read_reg(ctx.curr_inst->register_2);

            if(ctx.curr_inst->register_2 == RT_C)
            {
                addr |= 0xFF00;
            }

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);

            return;
        }

        case AM_R_HLI:
        {
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->register_2));
            emu_cycles(1);

            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;
        }

        case AM_R_HLD:
        {
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->register_2));
            emu_cycles(1);

            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;
        }

        case AM_HLI_R:
        {
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_2);
            ctx.mem_dest = cpu_read_reg(ctx.curr_inst->register_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;
        }

        case AM_HLD_R:
        {
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_2);
            ctx.mem_dest = cpu_read_reg(ctx.curr_inst->register_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;
        }


        // Set fetched data as content of the value of PC (u8)
        // Increment PC
        // Merge with AM_R_D8
        case AM_R_A8:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
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

        case AM_A8_R:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc) | 0xFF00;
            ctx.dest_is_mem = true;
            emu_cycles(1);
            ctx.registers.pc++;
            return;
        }

        case AM_HL_SPR:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;
        }

        case AM_A8:
        case AM_D8:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            return;
        }

        case AM_A16_R:
        case AM_D16_R:
        {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            // Combine lo and hi to form hilo. Example:
            // lo = 0xAA, hi = 0xBC => hi << 8 = 0xBC00 => 0x00AA | 0xBC00 => 0xBCAA
            ctx.mem_dest = lo | (hi << 8);
            ctx.registers.pc += 2;

            ctx.dest_is_mem = true;
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->register_2);
            return;
        }

        case AM_MR_D8:
        {
            ctx.fetched_data = bus_read(ctx.registers.pc);
            emu_cycles(1);
            ctx.registers.pc++;
            ctx.mem_dest = cpu_read_reg(ctx.curr_inst->register_1);
            ctx.dest_is_mem = true;
            return;
        }

        case AM_MR:
        {
            ctx.mem_dest = cpu_read_reg(ctx.curr_inst->register_1);
            ctx.dest_is_mem = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->register_1));
            emu_cycles(1);
            return;
        }

        case AM_R_A16:
        {
            u16 lo = bus_read(ctx.registers.pc);
            emu_cycles(1);

            u16 hi = bus_read(ctx.registers.pc + 1);
            emu_cycles(1);

            // Combine lo and hi to form hilo. Example:
            // lo = 0xAA, hi = 0xBC => hi << 8 = 0xBC00 => 0x00AA | 0xBC00 => 0xBCAA
            u16 addr = lo | (hi << 8);
            ctx.registers.pc += 2;

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);

            return;  
        }

        // Set fetched data as content of the PC and PC+1 (u16)
        // Works with little endian
        case AM_R_D16:
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