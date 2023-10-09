#pragma once

#include "linmath.h"
#include "types.h"

typedef struct Level_t Level_t;

typedef struct Level_Quad_t
{
  vec2 position;
  vec2 size;
} Level_Quad_t;

typedef struct Level_Objects_t
{
  Level_Quad_t *quads;
  u32 num_quads;
} Level_Objects_t;

Level_t *Level_Init(void);
void Level_Update(Level_t *level);
Level_Objects_t Level_GetObjects(Level_t *level);
void Level_Delete(Level_t *level);
