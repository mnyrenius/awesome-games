#include <stdlib.h>
#include "game.h"
#include "sprite_renderer.h"
#include <GLFW/glfw3.h>
#include "player.h"
#include "level.h"
#include "global.h"
#include "collision.h"
#include "util.h"

typedef struct Game_t
{
  bool keys[512];
  bool keys_processed[512];
  Player_t player;
  bool player_at_ground;
  bool player_max_jump;
  Level_t *level;
} Game_t;

Game_t * Game_Init(unsigned int width, unsigned int height)
{
  Game_t *game = malloc(sizeof(Game_t));
  memset(game->keys, 0, sizeof(game->keys) / sizeof(game->keys[0]));
  memset(game->keys_processed, 0, sizeof(game->keys_processed) / sizeof(game->keys_processed[0]));
  game->player = Player_Init();
  game->player_at_ground = false;
  game->player_max_jump = false;
  game->level = Level_Init();
  return game;
}

void Game_Update(Game_t * game, float dt)
{
  Level_Objects_t level_objs = Level_GetObjects(game->level);

  if (game->keys[GLFW_KEY_RIGHT])
  {
    game->player.velocity[0] = 150.0f;
    game->player.last_vel_x = game->player.velocity[0];
  }

  if (game->keys[GLFW_KEY_LEFT])
  {
    game->player.velocity[0] = -150.0f;
    game->player.last_vel_x = game->player.velocity[0];
  }

  if (game->keys[GLFW_KEY_UP])
  {
    game->player_at_ground = false;

    if (!game->player_max_jump)
    {
      game->player.velocity[1] += -30.0f;
    }

    if (game->player.velocity[1] < -200.f)
    {
      game->player_max_jump = true;
    }
  }

  if (!game->keys[GLFW_KEY_LEFT] && !game->keys[GLFW_KEY_RIGHT])
  {
    game->player.velocity[0] = 0.0f;
  }

  if (!game->player_at_ground)
  {
    game->player.velocity[1] += 300.0f * dt;
    game->player.position[1] += game->player.velocity[1] * dt;
  }

  game->player.position[0] += game->player.velocity[0] * dt;

  bool collision = false;

  for (u32 i = 0; i < level_objs.num_quads; ++i)
  {
    if (Collison_RectangleToRectangle(game->player.position, game->player.size, level_objs.quads[i].position, level_objs.quads[i].size))
    {
      //game->player.position[1] = level_objs.quads[i].position[1] - game->player.size[1];
      collision = true;
    }
  }

  if (collision)
  {
    game->player_at_ground = true;
    game->player.velocity[1] = 0.0f;
    game->player_max_jump = false;
  }
  else
  {
    game->player_at_ground = false;
  }

  Player_Update(&game->player);
  Level_Update(game->level);
}

void Game_UpdateKeys(Game_t * game, int key, int action)
{
  if (action == GLFW_PRESS)
  {
    game->keys[key] = true;
  }

  else if (action == GLFW_RELEASE)
  {
    game->keys[key] = false;
    game->keys_processed[key] = false;
  }
}

bool Game_IsQuit(Game_t *game)
{
  return false;
}

