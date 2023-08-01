#pragma once

char* debug_build_base(int numStrings, const char* format, ...);
char* debug_build_instruction();
char* debug_build_registers();
char* debug_build_flags();
void debug_print();