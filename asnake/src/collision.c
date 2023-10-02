#include <stdbool.h>
#include "collision.h"
#include "util.h"

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

bool Collison_RectangleToRectangle(vec2 a, vec2 a_size, vec2 b, vec2 b_size)
{
  bool result = false;

  if (((a[0] + a_size[0]) >= b[0]) && (a[0] <= (b[0] + b_size[0])) &&
         ((a[1] + a_size[1]) >= b[1]) && (a[1] <= (b[1] + b_size[1])))
         {
          LOG("%s", "Collision\n");
          result = true;
         }

  return result;
}