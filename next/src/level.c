#include <stdlib.h>
#include "level.h"
#include "renderer.h"

typedef struct Level_t
{
  Level_Quad_t *quads;
  u32 num_quads;
  Renderer_t *quad_renderer;
} Level_t;

Level_t *Level_Init(void)
{
  Level_t *level = malloc(sizeof(Level_t));
  level->num_quads = 3;
  level->quads = malloc(sizeof(Level_Quad_t) * level->num_quads);

  level->quads[0].position[0] = 0.0f;
  level->quads[0].position[1] = 550.0f;
  level->quads[0].size[0] = 800.0f;
  level->quads[0].size[1] = 50.0f;

  level->quads[1].position[0] = 350.0f;
  level->quads[1].position[1] = 530.0f;
  level->quads[1].size[0] = 50.0f;
  level->quads[1].size[1] = 20.0f;

  level->quads[2].position[0] = 400.0f;
  level->quads[2].position[1] = 470.0f;
  level->quads[2].size[0] = 100.0f;
  level->quads[2].size[1] = 20.0f;

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

void Level_Update(Level_t *level)
{
  for (u32 i = 0; i < level->num_quads; ++i)
  {
    Renderer_RenderObject(level->quad_renderer, level->quads[i].position, level->quads[i].size, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
  }
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