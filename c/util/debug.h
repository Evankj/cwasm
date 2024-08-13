#ifndef DEBUG_H
#define DEBUG_H
#include "../mem/mem.h"

__attribute__((import_module("env"), import_name("debugi32"))) extern void
debugi32(i32 val);

__attribute__((import_module("env"), import_name("debugf32"))) extern void
debugf32(f32 val);

__attribute__((import_module("env"), import_name("drawf32"))) extern void
drawf32(f32 val);

__attribute__((import_module("env"), import_name("print"))) extern void
print(i32 ptr, i32 size);

__attribute__((import_module("env"), import_name("printCStr"))) extern void
printCStr(i32 ptr);

__attribute__((import_module("env"), import_name("breakpoint"))) extern void
breakpoint();

#define DEBUG_BREAKPOINT                                                       \
  printCStr((i32) "BREAKPOINT: ");                                             \
  printCStr((i32)__FILE__);                                                    \
  debugi32((i32)__LINE__);                                                     \
  breakpoint()

#endif

