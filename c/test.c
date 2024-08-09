#include "arena.h"
#include "ecs.h"
#include "math/math.h"

#define NULL ((void *)0) // not using standard lib so need our own NULL def

typedef i32 KeyCode;
typedef i32 KeyMode;

#define DEF_ENUM(name, ...) enum name { __VA_ARGS__, name##_LENGTH }

DEF_ENUM(Keys, NONE, MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_RIGHT);

typedef struct {
  KeyMode keyStates[Keys_LENGTH];
} Input;

Input currentInput = {NONE};
Input lastInput = {NONE};

int playerX, playerY = 0;
__attribute__((import_module("env"), import_name("setPlayerPos"))) extern void
setPlayerPos(int x, int y);

__attribute__((import_module("env"), import_name("debug"))) extern void
debug(i32 val);

__attribute__((import_module("env"), import_name("print"))) extern void
print(i32 ptr, i32 size);

__attribute__((import_module("env"), import_name("printCStr"))) extern void
printCStr(i32 ptr);

__attribute__((import_module("env"), import_name("breakpoint"))) extern void
breakpoint();

#define DEBUG_BREAKPOINT                                                       \
  printCStr((i32) "BREAKPOINT: ");                                             \
  printCStr((i32)__FILE__);                                                    \
  debug((i32)__LINE__);                                                        \
  breakpoint()

// __attribute__((import_module("env"), import_name("assertFailed"))) extern
// void test(i32 testName);

// ref:
// https://stackoverflow.com/questions/3118490/getting-each-individual-digit-from-a-whole-integer
// This doesn't support num == 0 and num ending with 0
char *getCStringFromInt32(i32 num, Allocator allocator) {
  int numDigits = 0;
  for (int div = 1; div <= num; div *= 10)
    numDigits++;

  char *str = allocator.allocate(numDigits);
  int index = 0;
  int div;

  for (div = 1; div <= num; div *= 10)
    ;

  do {
    div /= 10;
    str[index] = (num / div) + '0'; // ascii code of the digit
    num %= div;
    index++;
  } while (num);

  str[numDigits] = '\0';

  return str;
}

// THIS IS CALLED BY THE BROWSER
extern void setInputState(KeyCode keycode, KeyMode mode) {
  if (keycode > Keys_LENGTH) {
    char *str = "Keycode outside of range!";
    printCStr((i32)str);
  } else {
    currentInput.keyStates[keycode] = mode;
  }
}

// ArenaAllocator arena = walloc(100000);arenaAllocatorCreate(sizeof(World));
// World *world = worldCreate();

ArenaAllocator *arena;
typedef float Speed;
ComponentType *speedComponent;
typedef Vector2 Position;
ComponentType *positionComponent;
World *world;
void test() {

  arena =
      arenaAllocatorCreate(sizeof(World) * 2, (Allocator){.allocate = walloc});

  world = worldCreate(arena);

  speedComponent = REGISTER_COMPONENT(world, Speed);
  positionComponent = REGISTER_COMPONENT(world, Position);

  Entity *e = worldCreateEntity(world);

  Speed *speed = entityAddComponentByID(world, e->id, speedComponent);

  *speed = 1;

  Position *pos = entityAddComponentByID(world, e->id, positionComponent);

  *pos = (Vector2){.x = 10, .y = 10};

  // i32 *x = walloc(sizeof(i32));
  // *x = 420;
  // debug(*x);
  // i32 *y = walloc(sizeof(i32));
  // *y = 1234;
  // debug(*y);
  //
  // char *str = "Hello, world!";
  // print((i32)str, 13);
  // printCStr((i32)str);
  //
  // Vector2 v1 = {
  //     .x = 1,
  //     .y = 1,
  // };
  //
  // Vector2 v2 = {
  //     .x = 1,
  //     .y = 1,
  // };
  //
  // Vector2 add = Math.vector2Add(v1, v2);
  //
  // char *xStr = getCStringFromInt32((i32)add.x, (Allocator){.allocate =
  // walloc}); char *yStr = getCStringFromInt32((i32)add.y,
  // (Allocator){.allocate = walloc});
  //
  // printCStr((i32)xStr);
  // printCStr((i32)yStr);
}

void movementSystem(World *world, EntityID id, f32 dt) {
  Speed *speed = entityGetComponentByID(world, id, speedComponent);
  if (!speed)
    return;
  Position *pos = entityGetComponentByID(world, id, positionComponent);
  if (!pos)
    return;

  int movingLeft = currentInput.keyStates[MOVE_LEFT];
  int movingRight = currentInput.keyStates[MOVE_RIGHT];
  int movingUp = currentInput.keyStates[MOVE_UP];
  int movingDown = currentInput.keyStates[MOVE_DOWN];

  if (movingLeft) {
    pos->x -= *speed * dt;
  } else if (movingRight) {
    pos->x += *speed * dt;
  }

  if (movingUp) {
    pos->y -= *speed * dt;
  } else if (movingDown) {
    pos->y += *speed * dt;
  }

  setPlayerPos(pos->x, pos->y);
}

void tick(f32 dt) {

  // int movingLeft = currentInput.keyStates[MOVE_LEFT];
  // int movingRight = currentInput.keyStates[MOVE_RIGHT];
  // int movingUp = currentInput.keyStates[MOVE_UP];
  // int movingDown = currentInput.keyStates[MOVE_DOWN];
  // f32 speed = 1;
  //
  //
  //
  // if (movingLeft) {
  //   playerX -= speed * dt;
  // } else if (movingRight) {
  //   playerX += speed * dt;
  // }
  //
  // if (movingUp) {
  //   playerY -= speed * dt;
  // } else if (movingDown) {
  //   playerY += speed * dt;
  // }
  //
  // setPlayerPos(playerX, playerY);

  for (EntityID id = 0; id < MAX_ENTITIES; id++) {
    movementSystem(world, id, dt);
  }

  // Position *pos = entityGetComponentByID(world, 0, positionComponent);
  //
  // // DEBUG_BREAKPOINT;
}
