#include <stdlib.h>
#include "level.h"
#include "renderer.h"
#include "util.h"
#include "global.h"

typedef struct Level_t
{
  Level_Quad_t *quads;
  u32 num_quads;
  Renderer_t *quad_renderer;
} Level_t;

Level_t *Level_Init(void)
{
  Level_t *level = malloc(sizeof(Level_t));
  level->num_quads = 50;
  level->quads = malloc(sizeof(Level_Quad_t) * level->num_quads);

  float y = 500.0f;

  level->quads[0].position[0] = 0.0f;
  level->quads[0].position[1] = 550.0f;
  level->quads[0].size[0] = 800.0f;
  level->quads[0].size[1] = 50.0f;

  for (u32 i = 1; i < level->num_quads; ++i)
  {
    if (i % 4 == 0)
    {
      y -= 70.0f;
    }

    level->quads[i].position[0] = rand() % 700;
    level->quads[i].position[1] = y;
    level->quads[i].size[0] = 30.0f;
    level->quads[i].size[1] = 20.0f;
    LOG("Quad at %f, %f\n", level->quads[i].position[0], level->quads[i].position[1]);
  }

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  level->quad_renderer = Renderer_Init(vertices, sizeof(vertices));

  return level;
}

void Level_Update(Level_t *level, vec2 player_pos)
{
  for (u32 i = 0; i < level->num_quads; ++i)
  {
    Renderer_RenderObject(level->quad_renderer, level->quads[i].position, level->quads[i].size, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
  }

  vec2 view = {0.0f, 0.0f};

  if (player_pos[1] < 300.0f)
  {
    view[1] = player_pos[1] - 300.0f;
  }

  Renderer_UpdateOrtho(level->quad_renderer, view);
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
  Renderer_Delete(level->quad_renderer);
  free(level);
}