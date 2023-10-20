#pragma once

#include "linmath.h"

typedef enum Collision_Direction_t
{
  COLLISION_DIRECTION_UP,
  COLLISION_DIRECTION_DOWN,
  COLLISION_DIRECTION_LEFT,
  COLLISION_DIRECTION_RIGHT,
} Collision_Direction_t;

typedef struct Collision_Result_t
{
  bool collision;
  Collision_Direction_t direction;
} Collision_Result_t;

bool Collision_PointToPoint(vec2 a, vec2 b, float margin);
bool Collision_PointToRectangle(vec2 a, vec2 b, vec2 b_size);
Collision_Result_t Collison_RectangleToRectangle(vec2 a, vec2 a_size, vec2 b, vec2 b_size);
