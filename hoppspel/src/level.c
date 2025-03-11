#include <stdlib.h>
#include "level.h"
#include "renderer.h"
#include "util.h"
#include "sprites.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "collision.h"

#define TILE_SIZE 32.0f
#define REGION_SIZE (TILE_SIZE * 4)
#define REGION_ROWS 10
#define REGION_COLS 6

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
  u32 num_quads;
  u32 num_fruits;
  u32 max_quads_per_platform;
} Level_t;

typedef struct Level_Region_t
{
  bool visited;
  bool part_of_path;
} Level_Region_t;

typedef struct Level_Backtrack_t
{
  u32 regions[10][2];
  i32 index;
} Level_Backtrack_t;

typedef struct Level_Gen_State_t
{
  u32 quad_index;
  Level_Region_t regions[REGION_ROWS][REGION_COLS];
  u32 prev_platform_size;
} Level_Gen_State_t;

static void backtrack_init(Level_Backtrack_t *backtrack)
{
  memset(backtrack, 0, sizeof(Level_Backtrack_t));
  backtrack->index = -1;
}

static void backtrack_push(Level_Backtrack_t *backtrack, const u32 region[2])
{
  if (backtrack->index == 9)
  {
    memcpy(&backtrack->regions[0], &backtrack->regions[1], sizeof(u32[2]) * 9);
    backtrack->regions[9][0] = region[0];
    backtrack->regions[9][1] = region[1];
  }
  else
  {
    i32 index = ++backtrack->index;
    backtrack->regions[index][0] = region[0];
    backtrack->regions[index][1] = region[1];
  }
}

static bool backtrack_pop(Level_Backtrack_t *backtrack, u32 region[2])
{
  if (backtrack->index < 0)
    return false;
  i32 index = backtrack->index--;
  region[0] = backtrack->regions[index][0];
  region[1] = backtrack->regions[index][1];
  return true;
}

static bool find_region_path(Level_Gen_State_t *state, u32 target_row, u32 target_col)
{
  bool found = false;
  u32 current_region[] = {0, 1 + (rand() % REGION_COLS)};
  Level_Backtrack_t backtrack;

  backtrack_init(&backtrack);

  for (u32 i = 0; i < REGION_ROWS; ++i)
  {
    for (u32 j = 0; j < REGION_COLS; ++j)
    {
      if (rand() % 5 == 0 && i != target_row && j != target_col)
      {
        LOG("Region (%u, %u) is blocked\n", i, j);
        state->regions[i][j].visited = true;
      }
    }
  }

  while (!found)
  {
    u32 x = current_region[1];
    u32 y = current_region[0];

    if (x == target_col && y == target_row)
    {
      LOG("Target found in region (%u, %u)\n", y, x);
      state->regions[y][x].part_of_path = true;
      state->regions[y][x].visited = true;
      found = true;
      break;
    }
    else
    {
      LOG("Target is not in region (%u, %u)\n", y, x);
      state->regions[y][x].part_of_path = true;
      state->regions[y][x].visited = true;
    }

    u32 new_region[2];

    new_region[0] = y + 1;
    new_region[1] = x;

    if (new_region[0] < REGION_ROWS)
    {
      if (!state->regions[new_region[0]][new_region[1]].visited)
      {
        backtrack_push(&backtrack, (u32[2]){y, x});
        current_region[0] = new_region[0];
        current_region[1] = new_region[1];
        continue;
      }

      LOG("Region (%u, %u) is visited\n", new_region[0], new_region[1]);
    }
    else
      LOG("Region (%u, %u) is outside map\n", new_region[0], new_region[1]);

    new_region[0] = y;
    new_region[1] = x - 1;

    if (new_region[1] < REGION_COLS)
    {
      if (!state->regions[new_region[0]][new_region[1]].visited)
      {
        backtrack_push(&backtrack, (u32[2]){y, x});
        current_region[0] = new_region[0];
        current_region[1] = new_region[1];
        continue;
      }

      LOG("Region (%u, %u) is visited\n", new_region[0], new_region[1]);
    }
    else
      LOG("Region (%u, %u) is outside map\n", new_region[0], new_region[1]);

    new_region[0] = y;
    new_region[1] = x + 1;

    if (new_region[1] < REGION_COLS)
    {
      if (!state->regions[new_region[0]][new_region[1]].visited)
      {
        backtrack_push(&backtrack, (u32[2]){y, x});
        current_region[0] = new_region[0];
        current_region[1] = new_region[1];
        continue;
      }

      LOG("Region (%u, %u) is visited\n", new_region[0], new_region[1]);
    }
    else
      LOG("Region (%u, %u) is outside map\n", new_region[0], new_region[1]);

    new_region[0] = y - 1;
    new_region[1] = x;

    if (new_region[0] < REGION_ROWS)
    {
      if (!state->regions[new_region[0]][new_region[1]].visited)
      {
        backtrack_push(&backtrack, (u32[2]){y, x});
        current_region[0] = new_region[0];
        current_region[1] = new_region[1];
        continue;
      }

      LOG("Region (%u, %u) is visited\n", new_region[0], new_region[1]);
    }
    else
      LOG("Region (%u, %u) is outside map\n", new_region[0], new_region[1]);

    if (backtrack_pop(&backtrack, current_region))
    {
      LOG("Backtrack index: %i region (%u, %u)\n", backtrack.index, current_region[0], current_region[1]);
    }

    else
    {
      LOG("%s\n", "Backtracking failed");
      break;
    }
  }

  return found;
}

static void insert_quad(Level_Quad_t *quads, vec2 position, u32 i, u32 max_quads)
{
  if (i >= max_quads)
    return;

  Level_Quad_t *new = &quads[i];
  vec2_dup(new->position, position);
  vec2_dup(new->size, (vec2){32.0f, 32.0f});

  Level_Quad_t *prev = &quads[i - 1];

  while (i > 0 && new->position[1] > prev->position[1])
  {
    vec2 temp;
    vec2_dup(temp, new->position);
    vec2_dup(new->position, prev->position);
    vec2_dup(prev->position, temp);
    i--;
    prev = &quads[i - 1];
    new = &quads[i];
  }
}

static void generate_quads(Level_Quad_t *quads, Level_Gen_State_t *state, u32 max_quads, u32 max_quads_per_platform)
{
  for (u32 i = 0; i < REGION_ROWS; ++i)
  {
    for (u32 j = 0; j < REGION_COLS; ++j)
    {
      if (state->regions[i][j].part_of_path)
      {
        vec2 position = {j * REGION_SIZE + REGION_SIZE / 4, (600.0f - REGION_SIZE) - (i * REGION_SIZE + REGION_SIZE / 4)};
        u32 num_quads = 1 + (rand() % max_quads_per_platform);
        vec2 np;
        vec2_dup(np, position);
        for (u32 k = 0; k < num_quads; ++k)
        {
          insert_quad(quads, np, state->quad_index++, max_quads);
          np[0] += 32.0f;
        }

        position[0] += REGION_SIZE / 2;
        position[1] += REGION_SIZE / 2;
        vec2_dup(np, position);
        for (u32 k = 0; k < num_quads; ++k)
        {
          insert_quad(quads, np, state->quad_index++, max_quads);
          np[0] += 32.0f;
        }
      }
    }
  }

  for (u32 i = state->quad_index; i < max_quads; ++i)
  {
    vec2 region = {rand() % REGION_COLS, rand() % REGION_ROWS};
    f32 x = region[0] * REGION_SIZE + (rand() % (u32)REGION_SIZE);
    f32 y = 600.0f - (region[1] * REGION_SIZE + (rand() % (u32)REGION_SIZE));
    vec2 pos = {x, y};
    bool collision = false;
    u32 num_quads = 1 + (rand() % max_quads_per_platform);

    for (u32 j = 0; j < state->quad_index; ++j)
    {
      if (Collision_RectangleToRectangle(pos, (vec2){32.0f * num_quads + 32.0f * 2.0f, 96.0f}, quads[j].position, (vec2){96.0f, 96.0f}).collision)
      {
        collision = true;
        i--;
        break;
      }
    }
    if (!collision)
    {
      for (u32 j = 0; j < num_quads; ++j)
      {
        insert_quad(quads, pos, state->quad_index++, max_quads);
        pos[0] += 32.0f;
      }
    }
  }
}

Level_t *Level_Init(void)
{
  Level_t *level = malloc(sizeof(Level_t));
  level->num_quads = 100 + (rand() % 100);
  level->num_fruits = 5 + (rand() % 25);
  level->max_quads_per_platform = 1 + (rand() % 5);
  level->quads = malloc(sizeof(Level_Quad_t) * level->num_quads);
  level->fruits = malloc(sizeof(Level_Fruit_t) * level->num_fruits);

  for (u32 i = 0; i < 25; ++i)
  {
    level->quads[i].position[0] = i * 32.0f;
    level->quads[i].position[1] = 600.0f - 32.0f;
    level->quads[i].size[0] = 32.0f;
    level->quads[i].size[1] = 32.0f;
  }

  u32 target_row = (REGION_ROWS - 3) + (rand() % 2);
  u32 target_col = rand() % REGION_COLS;
  LOG("Target is region (%u, %u)\n", target_row, target_col);

  Level_Gen_State_t gen_state;
  u32 retries = 20;
  while (retries > 0)
  {
    memset(gen_state.regions, 0, sizeof(Level_Region_t) * REGION_ROWS * REGION_COLS);
    if (find_region_path(&gen_state, target_row, target_col))
      break;

    retries--;
  }

  gen_state.quad_index = 25;
  gen_state.prev_platform_size = 25;

  generate_quads(level->quads, &gen_state, level->num_quads, level->max_quads_per_platform);

  u32 flag_offset = rand() % 20;

  for (u32 i = level->num_quads - 1; i > 25; --i)
  {
    u32 region_row = fabs(600.0f - level->quads[i].position[1]) / (u32)REGION_SIZE;
    u32 region_col = level->quads[i].position[0] / (u32)REGION_SIZE;
    if (gen_state.regions[region_row][region_col].part_of_path && (i < (level->num_quads - flag_offset)))
    {
      level->flag.position[0] = level->quads[i].position[0] - 16.0f / 2;
      level->flag.position[1] = level->quads[i].position[1] - 64.0f;
      vec2_dup(level->flag.size, (vec2){64.0f, 64.0f});
      break;
    }
  }

  for (u32 i = 0; i < level->num_fruits; ++i)
  {
    Level_Quad_t *q = &level->quads[(rand() % (level->num_quads - 26)) + 25];
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
  for (u32 i = 25; i < level->num_quads; ++i)
  {
    Level_Quad_t *q = &level->quads[i];
    if (q->position[1] >= y_min && q->position[1] <= y_max)
    {
      SpriteRenderer_RenderObject_With_Color(level->renderer, level->terrain_texture, q->position, q->size, uv, (vec4){1.0f, 1.0f, 1.0f, level->alpha}, false);
    }
  }

  for (u32 i = 0; i < level->num_fruits; ++i)
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
      .num_quads = level->num_quads,
      .quads = level->quads,
      .flag = &level->flag,
      .fruits = level->fruits,
      .num_fruits = level->num_fruits,
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