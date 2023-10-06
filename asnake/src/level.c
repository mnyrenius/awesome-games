#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "level.h"
#include "shader.h"
#include "renderer.h"
#include "collision.h"
#include "util.h"

typedef struct Level_t
{
  Level_Food_t *foods;
  int num_foods;
  Level_Wall_t *walls;
  int num_walls;
  Renderer_t *food_renderer;
  Renderer_t *wall_renderer;
  int difficulty;
  float alpha;
} Level_t;

Level_t *Level_Init(int width, int height, int difficulty)
{
  Level_t *level = malloc(sizeof(Level_t));

  level->num_walls = 4 + difficulty * 3;
  level->walls = malloc(sizeof(Level_Wall_t) * level->num_walls);

  level->walls[0].position[0] = 0.0f;
  level->walls[0].position[1] = 0.0f;
  level->walls[0].size[0] = width;
  level->walls[0].size[1] = 10.0f;

  level->walls[1].position[0] = 0.0f;
  level->walls[1].position[1] = 0.0f;
  level->walls[1].size[0] = 10.0f;
  level->walls[1].size[1] = height;

  level->walls[2].position[0] = width - 10.0f;
  level->walls[2].position[1] = 0.0f;
  level->walls[2].size[0] = 10.0f;
  level->walls[2].size[1] = height;

  level->walls[3].position[0] = 0.0f;
  level->walls[3].position[1] = height - 10.0f;
  level->walls[3].size[0] = width;
  level->walls[3].size[1] = 10.0f;

  vec2 center = { 400.0f, 300.f };

  for (int i = 4; i < level->num_walls; ++i)
  {
    level->walls[i].position[0] = rand() % width;
    level->walls[i].position[1] = rand() % 500;
    level->walls[i].size[0] = (rand() % 200) + 5.0f;
    level->walls[i].size[1] = (rand() % 100) + 5.0f;


    if (Collision_PointToRectangle(center, level->walls[i].position, level->walls[i].size))
    {
      i--;
      continue;
    }


    for (int wi = 0; wi < 4; ++wi)
    {
      if (Collison_RectangleToRectangle(level->walls[wi].position, level->walls[wi].size,
                                        level->walls[i].position, level->walls[i].size))
      {
        i--;
        break;
      }
    }
  }

  level->num_foods = 16;
  level->foods = malloc(sizeof(Level_Food_t) * level->num_foods);

  for (int i = 0; i < level->num_foods; ++i)
  {
    level->foods[i].position[0] = rand() % width;
    level->foods[i].position[1] = rand() % height;
    level->foods[i].eaten = false;
    level->foods[i].powerup = (rand() % 10) == 0;

    for (int wi = 0; wi < level->num_walls; ++wi)
    {
      if (Collision_PointToRectangle(level->foods[i].position, level->walls[wi].position, level->walls[wi].size))
      {
        LOG("%s", "Recalc food\n");
        i--;
        break;
      }
    }
  }

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  const char vs[] =
  {
    #include "shaders/generic_object.vs.data"
    , 0
  };
  const char fs_wall[] =
  {
    #include "shaders/wall.fs.data"
    , 0
  };
  Shader_t *wall_shader = Shader_Init();
  Shader_Load(wall_shader, vs, fs_wall);

  const char fs_food[] =
  {
    #include "shaders/food.fs.data"
    , 0
  };

  Shader_t *food_shader = Shader_Init();
  Shader_Load(food_shader, vs, fs_food);

  level->food_renderer = Renderer_Init_With_Shader(vertices, sizeof(vertices), food_shader);
  level->wall_renderer = Renderer_Init_With_Shader(vertices, sizeof(vertices), wall_shader);
  level->alpha = 1.0f;

  return level;
}

Level_Objects_t Level_GetObjects(Level_t *level)
{
  Level_Objects_t objects =
  {
    .foods = level->foods,
    .num_foods = level->num_foods,
    .walls = level->walls,
    .num_walls = level->num_walls,
  };

  return objects;
}

void Level_EatFood(Level_t *level, int index)
{
  level->foods[index].eaten = true;
}

bool Level_IsFinished(Level_t *level)
{
  for (int i = 0; i < level->num_foods; ++i)
  {
    if (!level->foods[i].eaten)
    {
      return false;
    }
  }

  return true;
}

void Level_Render(Level_t *level)
{
  vec2 food_size = {10.0f, 10.0f};
  vec4 wall_color = { 0.5f, 0.5f, 0.5f, level->alpha };
  vec4 food_color = { 1.0f, 0.1f, 0.1f, level->alpha };

  for (int i = 0; i < level->num_foods; ++i)
  {
    if(level->foods[i].eaten)
      continue;

    if (level->foods[i].powerup)
      food_color[1] = 0.8f;
    Renderer_RenderObject(level->food_renderer, level->foods[i].position, food_size, food_color);
    food_color[1] = 0.1f;
  }

  for (int i = 0; i < level->num_walls; ++i)
  {
    Renderer_RenderObject(level->wall_renderer, level->walls[i].position, level->walls[i].size, wall_color);
  }
}

void Level_SetAlpha(Level_t *level, float alpha)
{
  level->alpha = alpha;
}

void Level_Delete(Level_t *level)
{
  Renderer_Delete(level->food_renderer);
  Renderer_Delete(level->wall_renderer);
  free(level);
}
