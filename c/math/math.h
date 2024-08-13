#pragma once
#include "../mem/mem.h"

typedef struct {
  f32 x, y;
} Vector2;

struct math {
  const f32 pi;
  Vector2 (*vector2Add)(Vector2 v1, Vector2 v2);
  Vector2 (*vector2Sub)(Vector2 v1, Vector2 v2);
  Vector2 (*vector2Mult)(Vector2 v1, Vector2 v2);
  Vector2 (*vector2Div)(Vector2 v1, Vector2 v2);
  Vector2 (*vector2Scale)(Vector2 v, f32 scalar);
};

extern const struct math Math;
