#ifndef ARENA_H
#define ARENA_H
#include "mem.h"

typedef struct ArenaAllocator {
  void *(*allocate)(struct ArenaAllocator *, u32);
  void (*clear)(struct ArenaAllocator *);
  void (*destroy)(struct ArenaAllocator *);
  void *base;
  u32 offset;
  u32 capacity;
} ArenaAllocator;


static void *arenaAllocate(ArenaAllocator *allocator, u32 size) {
  if ((allocator->offset+size) <= allocator->capacity) {
    void *base = (char *)allocator->base + allocator->offset;
    allocator->offset += size;
    return base;
  }
  return NULL;
}

static void arenaClear(ArenaAllocator *allocator) {
  allocator->offset = 0;
}

static void arenaDestroy(ArenaAllocator *allocator) {
  // No-op for wasm here
}

static ArenaAllocator *arenaAllocatorCreate(u32 capacity, Allocator allocator) {
  ArenaAllocator *arena = (ArenaAllocator*)allocator.allocate(sizeof(ArenaAllocator));

  // Using our wasm allocator for this:
  void *base = allocator.allocate(capacity);

  *arena = (ArenaAllocator){
    .allocate = arenaAllocate,
    .clear = arenaClear,
    .destroy = arenaDestroy,
    .base = base,
    .offset = 0,
    .capacity = capacity,
  };
  return arena;
}

static ArenaAllocator *arenaAllocatorCreateScratch(u32 capacity, ArenaAllocator *arena) {
  ArenaAllocator *scratchArena = (ArenaAllocator*)arena->allocate(arena, sizeof(ArenaAllocator));

  void *base = arena->allocate(arena, capacity);

  *scratchArena = (ArenaAllocator){
    .allocate = arenaAllocate,
    .clear = arenaClear,
    .destroy = arenaDestroy,
    .base = base,
    .offset = 0,
    .capacity = capacity,
  };

  return scratchArena;
}

#endif
