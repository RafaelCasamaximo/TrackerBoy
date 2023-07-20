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
    u16 fetch_data;
    u16 mem_dest;
    u8 curr_opcode;
    Instruction curr_inst;

    // Flags
    bool halted;
    bool stepping;

} cpu_ctx;

void cpu_init();
bool cpu_step();