#include <common.h>
#include <cpu.h>
#include <bus.h>
#include <string.h>

extern cpu_ctx ctx;

char* debug_build_base(int numStrings, const char* format, ...)
{
    // Calculate the total length of the concatenated string
    va_list args;
    va_start(args, format);
    int totalLength = vsnprintf(NULL, 0, format, args);
    va_end(args);

    // Allocate memory to store the concatenated string
    char* result = (char*)malloc(totalLength + 1); // +1 for the null terminator

    // Check if memory allocation was successful
    if (result == NULL) {
        ERROR("Memory allocation failed");
    }

    // Concatenate the formatted strings into the result buffer
    va_start(args, format);
    vsprintf(result, format, args);
    va_end(args);

    return result;
}

char* debug_build_instruction()
{
    u16 pc = ctx.registers.pc;

    char* instruction = debug_build_base(3, "%04X: (%02X) %-5s",
    pc,
    ctx.curr_opcode,
    inst_name_by_type(ctx.curr_inst->instruction_type));

    char* inst_content = NULL;

    switch (ctx.curr_inst->address_mode)
    {
        case AM_IMP:
        {
            inst_content = debug_build_base(0, "");
            break;
        }

        case AM_R:
        {
            inst_content = debug_build_base(1, "%s",
            register_name_by_type(ctx.curr_inst->register_1));
            break;
        }

        case AM_R_R:
        {
            inst_content = debug_build_base(2, "%s, %s",
            register_name_by_type(ctx.curr_inst->register_1),
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_MR_R:
        {
            inst_content = debug_build_base(2, "(0x%04X), %s",
            cpu_read_reg(ctx.curr_inst->register_1),
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_R_MR:
        {
            inst_content = debug_build_base(2, "%s, (0x%04X)",
            register_name_by_type(ctx.curr_inst->register_1),
            cpu_read_reg(ctx.curr_inst->register_2));
            break;
        }

        case AM_R_HLI:
        {
            inst_content = debug_build_base(2, "%s, (0x%04X++)",
            register_name_by_type(ctx.curr_inst->register_1),
            cpu_read_reg(ctx.curr_inst->register_2));
            break;
        }

        case AM_R_HLD:
        {
            inst_content = debug_build_base(2, "%s, (0x%04X--)",
            register_name_by_type(ctx.curr_inst->register_1),
            cpu_read_reg(ctx.curr_inst->register_2));
            break;
        }

        case AM_HLI_R:
        {
            inst_content = debug_build_base(2, "(0x%04X++), %s",
            cpu_read_reg(ctx.curr_inst->register_1),
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_HLD_R:
        {
            inst_content = debug_build_base(2, "(0x%04X--), %s",
            cpu_read_reg(ctx.curr_inst->register_1),
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_R_A8:
        {
            inst_content = debug_build_base(2, "%s, (0x%02X)",
            register_name_by_type(ctx.curr_inst->register_1),
            ctx.registers.pc-1);
            break;
        }

        case AM_R_D8:
        {
            inst_content = debug_build_base(2, "%s, 0x%02X",
            register_name_by_type(ctx.curr_inst->register_1),
            ctx.fetched_data);
            break;
        }

        case AM_A8_R:
        {
            inst_content = debug_build_base(2, "(0x%02X), %s",
            ctx.registers.pc-1,
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_HL_SPR:
        {
            inst_content = debug_build_base(2, "(0x%04X), (SP+0x%04X)",
            cpu_read_reg(ctx.curr_inst->register_1),
            ctx.registers.pc-1);
            break;
        }

        case AM_D8:
        {
            inst_content = debug_build_base(1, "0x%02X",
            ctx.fetched_data);
            break;
        }

        case AM_A16_R:
        {
            u16 hi = bus_read(ctx.registers.pc);
            u16 lo = bus_read(ctx.registers.pc - 1);
            u16 addr = lo | (hi << 8);

            inst_content = debug_build_base(2, "(0x%04X), %s",
            addr,
            register_name_by_type(ctx.curr_inst->register_2));
            break;
        }

        case AM_MR_D8:
        {
            inst_content = debug_build_base(2, "(0x%04X), 0x%02X",
            cpu_read_reg(ctx.curr_inst->register_1),
            ctx.fetched_data);
            break;
        }

        case AM_MR:
        {
            inst_content = debug_build_base(2, "(0x%04X)",
            cpu_read_reg(ctx.curr_inst->register_1));
            break;
        }

        case AM_R_A16:
        {
            inst_content = debug_build_base(2, "%s, (0x%04X)",
            register_name_by_type(ctx.curr_inst->register_1),
            ctx.fetched_data);
            break;  
        }

        case AM_R_D16:
        {
            u16 hi = bus_read(ctx.registers.pc);
            u16 lo = bus_read(ctx.registers.pc - 1);
            u16 addr = lo | (hi << 8);
            inst_content = debug_build_base(2, "%s, (0x%04X)",
            register_name_by_type(ctx.curr_inst->register_1),
            addr);
            break;  
        }

        case AM_A16:
        {
            inst_content = debug_build_base(1, "(0x%04X)",
            ctx.fetched_data);
            break;  
        }

        case AM_D16:
        {
            u16 hi = bus_read(ctx.registers.pc);
            u16 lo = bus_read(ctx.registers.pc - 1);
            u16 addr = lo | (hi << 8);
            inst_content = debug_build_base(1, "0x%04X",
            addr);
            break;
        }

        default:
        {
            inst_content = debug_build_base(0, "");
            break;
        }
    }

    return debug_build_base(1, "%s %-7s", instruction, inst_content);
}

char* debug_build_registers()
{
    return debug_build_base(7, "A: 0x%02X BC: 0x%02X%02X DE: 0x%02X%02X HL: 0x%02X%02X",
    ctx.registers.a,
    ctx.registers.b, ctx.registers.c,
    ctx.registers.d, ctx.registers.e,
    ctx.registers.h, ctx.registers.l);
}

char* debug_build_flags()
{
    return debug_build_base(4, "%c,%c,%c,%c",
    BIT(ctx.registers.f, 7) ? 'Z' : '-',
    BIT(ctx.registers.f, 6) ? 'N' : '-',
    BIT(ctx.registers.f, 5) ? 'H' : '-',
    BIT(ctx.registers.f, 4) ? 'C' : '-');
}

void debug_print()
{

    char* intruction = debug_build_instruction();
    char* registers = debug_build_registers();
    char* flags = debug_build_flags();


    printf("%s%s\t%s%s\t%s%s%s\n",
    ANSI_COLOR_BRIGHT_GREEN, intruction,
    ANSI_COLOR_BRIGHT_CYAN, registers,
    ANSI_COLOR_BRIGHT_YELLOW, flags,
    ANSI_COLOR_RESET);

    free(intruction);
    free(registers);
    free(flags);
}