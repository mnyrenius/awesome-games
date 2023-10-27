#include <stdlib.h>
#include "game.h"
#include "sprite_renderer.h"
#include <GLFW/glfw3.h>
#include "player.h"
#include "level.h"
#include "global.h"
#include "collision.h"
#include "util.h"
#include "time.h"
#include "text_renderer.h"

typedef enum Game_State_t
{
  GAME_STATE_PLAYING,
  GAME_STATE_DEAD
} Game_State_t;

typedef struct Game_t
{
  u32 width;
  u32 height;
  bool keys[512];
  bool keys_processed[512];
  Player_t player;
  bool player_at_ground;
  bool player_max_jump;
  Level_t *level;
  Game_State_t state;
  TextRenderer_t *text_renderer;
} Game_t;

char *collision_direction_to_string(Collision_Direction_t direction)
{
  switch (direction)
  {
  case COLLISION_DIRECTION_UP:
    return "Up";
  case COLLISION_DIRECTION_DOWN:
    return "Down";
  case COLLISION_DIRECTION_LEFT:
    return "Left";
  case COLLISION_DIRECTION_RIGHT:
    return "Right";
  default:
    return "Unknown";
  }
}

void state_playing(Game_t *game, float dt)
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
      game->player.velocity[1] += -50.0f;
    }

    if (game->player.velocity[1] < -250.f)
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
    game->player.velocity[1] += 500.0f * dt;
    game->player.position[1] += game->player.velocity[1] * dt;
  }

  game->player.position[0] += game->player.velocity[0] * dt;

  bool collision = false;
  for (u32 i = 0; i < level_objs.num_quads; ++i)
  {
    Player_t *p = &game->player;
    Level_Quad_t *q = &level_objs.quads[i];

    if ((p->position[1] + p->size[1]) < q->position[1])
    {
      continue;
    }

    if (p->position[1] > (q->position[1] + q->size[1]))
    {
      break;
    }

    Collision_Result_t collision_result = Collison_RectangleToRectangle(p->position, p->size, q->position, q->size);
    if (collision_result.collision)
    {
      collision = true;
      if (collision_result.direction == COLLISION_DIRECTION_DOWN)
      {
        game->player.velocity[1] = 0.0f;
        game->player.position[1] = q->position[1] - p->size[1];
        game->player_at_ground = true;
        game->player_max_jump = false;
      }
      else if (collision_result.direction == COLLISION_DIRECTION_UP)
      {
        game->player.velocity[1] = -game->player.velocity[1] * 0.8f;
        game->player_max_jump = true;
      }
      else if (collision_result.direction == COLLISION_DIRECTION_LEFT)
      {
        game->player.position[0] = q->position[0] + q->size[0];
      }
      else if (collision_result.direction == COLLISION_DIRECTION_RIGHT)
      {
        game->player.position[0] = q->position[0] - p->size[0];
      }
      break;
    }
  }

  if (!collision)
  {
    game->player_at_ground = false;
  }

  Player_Update(&game->player);
  Level_Update(game->level, game->player.position);

  if (game->player.position[1] > (600.0f - game->player.size[1])- Global_Time.now * 10.0f)
  {
    game->state = GAME_STATE_DEAD;
  }
}

void state_dead(Game_t *game, float dt)
{
  game->player.velocity[0] = 0.0f;
  game->player.velocity[1] = 0.0f;
  Player_Update(&game->player);
  Level_Update(game->level, game->player.position);

  char *text = "YOU DIED.";
  TextRenderer_RenderString(game->text_renderer, text, (vec2){400.0f - strlen(text) * 8.0f * 2, 300.0f}, 4.0f);

  if (game->keys[GLFW_KEY_ENTER])
  {
    Level_Delete(game->level);
    Player_Delete(&game->player);
    game->player = Player_Init();
    game->player_at_ground = false;
    game->player_max_jump = false;
    game->level = Level_Init();
    game->state = GAME_STATE_PLAYING;
  }
}

Game_t * Game_Init(unsigned int width, unsigned int height)
{
  srand(time(NULL));
  Game_t *game = malloc(sizeof(Game_t));
  memset(game->keys, 0, sizeof(game->keys) / sizeof(game->keys[0]));
  memset(game->keys_processed, 0, sizeof(game->keys_processed) / sizeof(game->keys_processed[0]));
  game->player = Player_Init();
  game->player_at_ground = false;
  game->player_max_jump = false;
  game->level = Level_Init();
  game->state = GAME_STATE_PLAYING;
  game->text_renderer = TextRenderer_Init();
  return game;
}

void Game_Update(Game_t * game, float dt)
{
  switch (game->state)
  {
  case GAME_STATE_PLAYING:
    state_playing(game, dt);
    break;

  case GAME_STATE_DEAD:
    state_dead(game, dt);
    break;

  default:
    break;
  }
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
