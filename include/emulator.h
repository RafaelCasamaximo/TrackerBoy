#pragma once

#include "common.h"

typedef struct {
    bool paused;
    bool running;
    u8 ticks;
} emu_ctx;


emu_ctx* emu_get_ctx();
int emu_run(int argc, char** argv);