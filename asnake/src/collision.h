#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "linmath.h"

bool Collision_PointToPoint(vec2 a, vec2 b, float margin);
bool Collision_PointToRectangle(vec2 a, vec2 b, vec2 b_size);
bool Collison_RectangleToRectangle(vec2 a, vec2 a_size, vec2 b, vec2 b_size);

#endif /* _COLLISION_H_ */