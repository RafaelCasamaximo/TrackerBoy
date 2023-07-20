#include <common.h>

typedef struct{
    u8 a, f;
    u8 b, c;
    u8 d, e;
    u8 h, l;
    u16 sp;
    u16 pc;
} cpu_registers;

typedef struct{
    cpu_registers registers;
} cpu_ctx;