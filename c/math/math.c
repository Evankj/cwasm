#include "math.h"

static Vector2 vector2Add(Vector2 v1, Vector2 v2) {
  return (Vector2){.x = v1.x + v2.x, .y = v1.y + v2.y};
}

static Vector2 vector2Sub(Vector2 v1, Vector2 v2) {
  return (Vector2){.x = v1.x - v2.x, .y = v1.y - v2.y};
}

static Vector2 vector2Mult(Vector2 v1, Vector2 v2) {
  return (Vector2){.x = v1.x * v2.x, .y = v1.y * v2.y};
}

static Vector2 vector2Div(Vector2 v1, Vector2 v2) {
  return (Vector2){.x = v1.x / v2.x, .y = v1.y / v2.y};
}

static Vector2 vector2Scale(Vector2 v, f32 scalar) {
  return (Vector2){.x = v.x * scalar, .y = v.y * scalar};
}

const struct math Math = {.pi = 3.14,
                          .vector2Add = vector2Add,
                          .vector2Sub = vector2Sub,
                          .vector2Div = vector2Div,
                          .vector2Mult = vector2Mult,
                          .vector2Scale = vector2Scale};
