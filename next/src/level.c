#include <stdlib.h>
#include "level.h"
#include "renderer.h"
#include "util.h"
#include "sprites.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "collision.h"

#define NUM_FRUITS 20
#define NUM_QUADS 100
#define QUADS_PER_ROW 4

typedef struct Level_t
{
  Level_Quad_t *quads;
  Level_Quad_t flag;
  SpriteRenderer_t *renderer;
  Texture_t *terrain_texture;
  Texture_t *flag_texture;
  Texture_t *fruit_texture;
  Level_Fruit_t *fruits;
  float alpha;
} Level_t;

Level_t *Level_Init(void)
{
  Level_t *level = malloc(sizeof(Level_t));
  level->quads = malloc(sizeof(Level_Quad_t) * NUM_QUADS);
  level->fruits = malloc(sizeof(Level_Fruit_t) * NUM_FRUITS);

  for (u32 i = 0; i < 25; ++i)
  {
    level->quads[i].position[0] = i * 32.0f;
    level->quads[i].position[1] = 600.0f - 32.0f;
    level->quads[i].size[0] = 32.0f;
    level->quads[i].size[1] = 32.0f;
  }

  float y = 500.0f;

  for (u32 i = 0; i < (NUM_QUADS - 25) / QUADS_PER_ROW; ++i)
  {
    for (u32 j = 0; j < QUADS_PER_ROW; ++j)
    {
      Level_Quad_t *new = &level->quads[i * QUADS_PER_ROW + j + 25];
      new->position[0] = rand() % 700;
      new->position[1] = y;
      new->size[0] = 32.0f;
      new->size[1] = 32.0f;

      for (u32 k = 0; k < j; ++k)
      {
        Level_Quad_t *other = &level->quads[i * QUADS_PER_ROW + k + 25];
        if (Collision_RectangleToRectangle(new->position, new->size, other->position, other->size).collision)
        {
          j--;
          break;
        }
      }
    }
    y -= 70.0f;
  }

  level->flag.position[0] = level->quads[NUM_QUADS - 1].position[0] - 16.0f / 2;
  level->flag.position[1] = level->quads[NUM_QUADS - 1].position[1] - 64.0f;
  vec2_dup(level->flag.size, (vec2){64.0f, 64.0f});

  for (u32 i = 0; i < NUM_FRUITS; ++i)
  {
    Level_Quad_t *q = &level->quads[(rand() % (NUM_QUADS - 1)) + 25];
    level->fruits[i].quad.position[0] = q->position[0];
    level->fruits[i].quad.position[1] = q->position[1] - 32.0f;
    vec2_dup(level->fruits[i].quad.size, (vec2){32.0f, 32.0f});
    level->fruits[i].taken = false;
  }

  level->renderer = SpriteRenderer_Init();
  level->terrain_texture = Texture_Init(&Terrain_16x16_png, Terrain_16x16_png_len, 1);
  level->flag_texture = Texture_Init(&Flag_Idle_64x64_png, Flag_Idle_64x64_png_len, 10);
  level->fruit_texture = Texture_Init(&Orange_png, Orange_png_len, 17);
  level->alpha = 1.0f;

  return level;
}

void Level_Update(Level_t *level, vec2 player_pos)
{
  float y_min = player_pos[1] > 300.0f ? 0.0f : player_pos[1] - 350.0f;
  float y_max = player_pos[1] + 300.0f;

  vec2 uv = {98.0f, 0.0f};
  for (u32 i = 0; i < 25 && player_pos[1] > 250.0f; ++i)
  {
    SpriteRenderer_RenderObject_With_Color(level->renderer, level->terrain_texture, level->quads[i].position, level->quads[i].size, uv, (vec4){1.0f, 1.0f, 1.0f, level->alpha}, false);
  }

  uv[0] = 208.0f;
  uv[1] = 79.0f;
  for (u32 i = 25; i < NUM_QUADS; ++i)
  {
    Level_Quad_t *q = &level->quads[i];
    if (q->position[1] >= y_min && q->position[1] <= y_max)
    {
      SpriteRenderer_RenderObject_With_Color(level->renderer, level->terrain_texture, q->position, q->size, uv, (vec4){1.0f, 1.0f, 1.0f, level->alpha}, false);
    }
  }

  for (u32 i = 0; i < NUM_FRUITS; ++i)
  {
    Level_Fruit_t *f = &level->fruits[i];
    if (f->quad.position[1] >= y_min && f->quad.position[1] <= y_max && !level->fruits[i].taken)
    {
      SpriteRenderer_RenderObject_With_Color(level->renderer, level->fruit_texture, f->quad.position, f->quad.size, (vec2){0.0f, 0.0f}, (vec4){1.0f, 1.0f, 1.0f, level->alpha}, false);
    }
  }

  SpriteRenderer_RenderObject_With_Color(level->renderer, level->flag_texture, level->flag.position, level->flag.size, (vec2){0.0f, 0.0f}, (vec4){1.0f, 1.0f, 1.0f, level->alpha}, false);

  vec2 view = {0.0f, 0.0f};

  if (player_pos[1] < 300.0f)
  {
    view[1] = player_pos[1] - 300.0f;
  }

  SpriteRenderer_UpdateOrtho(level->renderer, view);
}

Level_Objects_t Level_GetObjects(Level_t *level)
{
  return (Level_Objects_t){
      .num_quads = NUM_QUADS,
      .quads = level->quads,
      .flag = &level->flag,
      .fruits = level->fruits,
      .num_fruits = NUM_FRUITS,
  };
}

void Level_SetAlpha(Level_t *level, float alpha)
{
  level->alpha = alpha;
}

void Level_Delete(Level_t *level)
{
  SpriteRenderer_Delete(level->renderer);
  free(level->quads);
  free(level);
  Texture_Delete(level->terrain_texture);
}