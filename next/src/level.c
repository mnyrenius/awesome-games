#include <stdlib.h>
#include "level.h"
#include "renderer.h"
#include "util.h"
#include "global.h"
#include "sprites.h"
#include "sprite_renderer.h"
#include "texture.h"

typedef struct Level_t
{
  Level_Quad_t *quads;
  u32 num_quads;
  SpriteRenderer_t *renderer;
  Texture_t *terrain_texture;

} Level_t;

Level_t *Level_Init(void)
{
  Level_t *level = malloc(sizeof(Level_t));
  level->num_quads = 100;
  level->quads = malloc(sizeof(Level_Quad_t) * level->num_quads);

  for (u32 i = 0; i < 25; ++i)
  {
    level->quads[i].position[0] = i * 32.0f;
    level->quads[i].position[1] = 600.0f - 32.0f;
    level->quads[i].size[0] = 32.0f;
    level->quads[i].size[1] = 32.0f;
  }

  float y = 500.0f;

  for (u32 i = 25; i < level->num_quads; ++i)
  {
    if (i % 4 == 0)
    {
      y -= 70.0f;
    }

    level->quads[i].position[0] = rand() % 700;
    level->quads[i].position[1] = y;
    level->quads[i].size[0] = 32.0f;
    level->quads[i].size[1] = 32.0f;
  }

  level->renderer = SpriteRenderer_Init();
  level->terrain_texture = Texture_Init(&Terrain_16x16_png, Terrain_16x16_png_len, 1);

  return level;
}

void Level_Update(Level_t *level, vec2 player_pos)
{
  float y_min = player_pos[1] > 300.0f ? 0.0f : player_pos[1] - 350.0f;
  float y_max = player_pos[1] + 300.0f;

  vec2 uv = {98.0f, 0.0f};
  for (u32 i = 0; i < 25 && player_pos[1] > 250.0f; ++i)
  {
    SpriteRenderer_RenderObject(level->renderer, level->terrain_texture, level->quads[i].position, level->quads[i].size, uv, false);
  }

  uv[0] = 208.0f;
  uv[1] =  79.0f;
  for (u32 i = 25; i < level->num_quads; ++i)
  {
    Level_Quad_t *q = &level->quads[i];
    if (q->position[1] >= y_min && q->position[1] <= y_max)
    {
      SpriteRenderer_RenderObject(level->renderer, level->terrain_texture, q->position, q->size, uv, false);
    }
  }

  vec2 view = {0.0f, 0.0f};

  if (player_pos[1] < 300.0f)
  {
    view[1] = player_pos[1] - 300.0f;
  }

  SpriteRenderer_UpdateOrtho(level->renderer, view);
}

Level_Objects_t Level_GetObjects(Level_t *level)
{
  return (Level_Objects_t)
  {
    .num_quads = level->num_quads,
    .quads = level->quads
  };
}

void Level_Delete(Level_t *level)
{
  SpriteRenderer_Delete(level->renderer);
  free(level);
}