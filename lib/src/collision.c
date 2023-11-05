#include <stdbool.h>
#include "collision.h"
#include "util.h"
#include "linmath.h"
#include "types.h"

bool Collision_PointToPoint(vec2 a, vec2 b, float margin)
{
  float xdiff = a[0] - b[0];
  float ydiff = a[1] - b[1];
  if (xdiff < margin && xdiff > -margin && ydiff < margin && ydiff > -margin)
  {
    return true;
  }

  return false;
}

bool Collision_PointToRectangle(vec2 a, vec2 b, vec2 b_size)
{
  if ((a[0] + 10.0f) >= b[0] && a[0] <= (b[0] + b_size[0]) &&
      (a[1] + 10.0f) >= b[1] && a[1] <= (b[1] + b_size[1]))
  {
    return true;
  }

  return false;
}

float dot(vec2 a, vec2 b)
{
  return a[0] * b[0] + a[1] * b[1];
}

float clamp(float value, float min, float max)
{
  return fmax(min, fmin(value, max));
}

Collision_Result_t Collision_RectangleToRectangle(vec2 a, vec2 a_size, vec2 b, vec2 b_size)
{
  Collision_Result_t result = {.collision = false, .direction = COLLISION_DIRECTION_UP};
  vec2 a_mid = {a[0] + a_size[0] / 2.0f, a[1] + a_size[1] / 2.0f};
  vec2 b_mid = {b[0] + b_size[0] / 2.0f, b[1] + b_size[1] / 2.0f};
  vec2 difference;
  vec2_sub(difference, a_mid, b_mid);
  difference[0] = clamp(difference[0], -b_size[0] / 2.0f, b_size[0] / 2.0f);
  difference[1] = clamp(difference[1], -b_size[1] / 2.0f, b_size[1] / 2.0f);
  vec2 closest;
  vec2_add(closest, b_mid, difference);
  vec2_sub(difference, closest, a_mid);

  if (vec2_len(difference) < 16.0f)
  {
    vec2 compass[] =
        {
            {0.0f, -1.0f}, // up
            {0.0f, 1.0f},  // down
            {-1.0f, 0.0f}, // left
            {1.0f, 0.0f},  // right
        };

    vec2_norm(difference, difference);
    result.collision = true;
    float max = 0.0f;
    for (u32 i = 0; i < 4; ++i)
    {
      float d = dot(difference, compass[i]);
      if (d > max)
      {
        max = d;
        result.direction = i;
      }
    }
  }

  return result;
}