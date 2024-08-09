#ifndef ECS_H
#define ECS_H
#include "arena.h"
const i32 MAX_COMPONENTS = 192; // This must be a multiple of 64
const i32 MAX_ENTITIES = 1000;
const i32 MASK_SECTIONS = MAX_COMPONENTS / 64;

typedef u32 EntityID;
typedef u64 BitMask;
typedef BitMask EntityComponentMask[MASK_SECTIONS];

typedef struct {
  BitMask mask;
  int section;
} ComponentMask;
typedef struct {
  int id;
  ComponentMask mask;
  char *name;
  i32 size;
  void **entries; // This is an array of the actual components with size = size
                  // * MAX_ENTITIES
} ComponentType;

typedef struct {
  EntityID id;
  EntityComponentMask mask;
} Entity;

typedef struct {
  ArenaAllocator *allocator;
  i32 componentCount;
  i32 entityCount;
  Entity entities[MAX_ENTITIES];
  // EntityID freeIds[MAX_ENTITIES];
  ComponentType components[MAX_COMPONENTS];
} World;

// worldAllocator = the allocator to allocate the world and its allocator with
static World *worldCreate(ArenaAllocator *worldAllocator) {
  World *world =
      (World *)worldAllocator->allocate(worldAllocator, sizeof(World));

  world->allocator = worldAllocator;
  world->entityCount = 0;

  return world;
}

static ComponentType *worldRegisterComponent(World *world, i32 size,
                                             char *name) {

  if (world->componentCount >= MAX_COMPONENTS) {
    return (ComponentType *)NULL;
  }

  BitMask mask = 0;
  int section = 0;
  if (world->componentCount <= 0) {
    // this is first component, we want this one to have bitmask of 1
    mask = 1;
  } else {
    section = world->componentCount / 64;
    mask = (1 << world->componentCount) % 64;
  }
  void *entries =
      world->allocator->allocate(world->allocator, size * MAX_ENTITIES);

  if (!entries) {
    return (ComponentType *)NULL;
  }

  ComponentType *component = &world->components[world->componentCount];
  component->id = world->componentCount;

  world->componentCount += 1;

  component->mask.mask = mask;
  component->mask.section = section;
  component->name = name;
  component->entries = &entries;
  component->size = size;

  return component;
}

#define REGISTER_COMPONENT(world, componentType)                               \
  worldRegisterComponent(world, sizeof(componentType), #componentType)

static Entity *worldCreateEntity(World *world) {
  EntityID id = world->entityCount;
  Entity e = world->entities[id];
  e.id = id;
  for (i32 i = 0; i < MASK_SECTIONS; i++) {
    e.mask[i] = 0;
  }

  world->entityCount += 1;

  return &world->entities[id];
}

static int hasComponent(EntityComponentMask entityMask,
                        ComponentMask componentMask) {
  i32 res = entityMask[componentMask.section] & componentMask.mask;
  return res > 0;
}

static void addComponentToMask(EntityComponentMask *entityMask,
                               ComponentMask componentMask) {
  BitMask section = *entityMask[componentMask.section];
  section |= componentMask.mask;
  *entityMask[componentMask.section] = section;
}

static void *entityGetComponentByID(World *world, EntityID entityID,
                                    ComponentType *componentType) {
  if (hasComponent(world->entities[entityID].mask, componentType->mask)) {
    return componentType->entries[entityID];
  }
  return NULL;
}

static void *entityAddComponentByID(World *world, EntityID entityID,
                                    ComponentType *componentType) {
  addComponentToMask(&world->entities[entityID].mask, componentType->mask);

  // componentType->entries[entityID] = {0};

  return componentType->entries[entityID];
}
#endif
