#define NULL ((void *)0) // not using standard lib so need our own NULL def

// Not portable but we're targeting only wasm for now...
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

typedef void *(*Allocate)(
    u32 size); // u32 for size because WASM only supports addressing 32 bits
typedef struct {
  Allocate allocate;
} Allocator;

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

// __attribute__((import_module("env"),
//                import_name("myJavaScriptFunction"))) extern void
// myJavaScriptFunction(int x, int y, int w, int h);

__attribute__((import_module("env"), import_name("wasm_malloc"))) extern i32
wasm_malloc(i32 size);

__attribute__((import_module("env"), import_name("debug"))) extern void
debug(i32 val);

__attribute__((import_module("env"), import_name("print"))) extern void
print(i32 ptr, i32 size);

__attribute__((import_module("env"), import_name("printCStr"))) extern void
printCStr(i32 ptr);

void *walloc(u32 size) {
  void *ptr = (void *)wasm_malloc(size);
  if (ptr == 0)
    return NULL;

  return ptr;
}

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

void test() {
  i32 *x = walloc(sizeof(i32));
  *x = 420;
  debug(*x);
  i32 *y = walloc(sizeof(i32));
  *y = 1234;
  debug(*y);

  char *str = "Hello, world!";
  print((i32)str, 13);
  printCStr((i32)str);
}

void tick(f32 dt) {
  Allocator wasmBufferAllocator = {walloc};
  // char *delta = getCStringFromInt32(dt, wasmBufferAllocator);
  // printCStr((i32)delta);
  int movingLeft = currentInput.keyStates[MOVE_LEFT];
  int movingRight = currentInput.keyStates[MOVE_RIGHT];
  int movingUp = currentInput.keyStates[MOVE_UP];
  int movingDown = currentInput.keyStates[MOVE_DOWN];
  f32 speed = 1;

  if (movingLeft) {
    playerX -= speed * dt;
  } else if (movingRight) {
    playerX += speed * dt;
  }

  if (movingUp) {
    playerY -= speed * dt;
  } else if (movingDown) {
    playerY += speed * dt;
  }
  
  setPlayerPos(playerX, playerY);
}
