#ifndef MEM_H
#define MEM_H
#define NULL ((void *)0) // not using standard lib so need our own NULL def

// Not portable but we're targeting only wasm for now...
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

typedef void *(*Allocate)(
    u32 size // u32 for size because WASM only supports addressing 32 bits
);

typedef struct {
  Allocate allocate;
} Allocator;

__attribute__((import_module("env"), import_name("wasm_malloc"))) extern i32
wasm_malloc(i32 size);

static void *walloc(u32 size) {
  void *ptr = (void *)wasm_malloc(size);
  if (ptr == 0)
    return NULL;

  return ptr;
}


__attribute__((import_module("env"), import_name("wasm_memset"))) extern i32
wasm_memset(u32 ptr, u32 size, u32 val);

static void wemset(u32 ptr, u32 size, u32 val) {
  wasm_memset(ptr, size, val);
}
#endif
