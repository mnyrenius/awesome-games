#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "linmath.h"
#include "renderer.h"

typedef struct Level_t Level_t;

typedef struct Level_Food_t
{
  vec2 position;
  bool eaten;
  bool powerup;
} Level_Food_t;

typedef struct Level_Wall_t
{
  vec2 position;
  vec2 size;
} Level_Wall_t;

typedef struct Level_Objects_t
{
  Level_Food_t *foods;
  int num_foods;
  Level_Wall_t *walls;
  int num_walls;
} Level_Objects_t;

Level_t *Level_Init(int width, int height, int difficulty);
Level_Objects_t Level_GetObjects(Level_t *level);
void Level_EatFood(Level_t *level, int index);
bool Level_IsFinished(Level_t *level);
void Level_Render(Level_t *level);
void Level_SetAlpha(Level_t *level, float alpha);
void Level_Delete(Level_t *level);

#endif /* _LEVEL_H_ */