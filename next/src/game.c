#include <stdlib.h>
#include <stdbool.h>
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
#include "hud.h"
#include "menu.h"

typedef enum Game_State_t
{
  GAME_STATE_PLAYING,
  GAME_STATE_WON,
  GAME_STATE_MENU,
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
  u32 level_no;
  Game_State_t state;
  TextRenderer_t *text_renderer;
  Hud_t *hud;
  float time;
  int score;
  Menu_t *menu;
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

void reset_game(Game_t *game)
{
  srand(game->level_no * 1234 + 5678);
  memset(game->keys, 0, sizeof(game->keys) / sizeof(game->keys[0]));
  memset(game->keys_processed, 0, sizeof(game->keys_processed) / sizeof(game->keys_processed[0]));
  game->player = Player_Init();
  game->player_at_ground = false;
  game->player_max_jump = false;
  game->level = Level_Init();
  game->state = GAME_STATE_PLAYING;
  game->time = 0;
  game->score = 0;
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

  if (game->keys[GLFW_KEY_ESCAPE] || game->keys[GLFW_KEY_Q])
  {
    Level_SetAlpha(game->level, 0.2f);
    game->state = GAME_STATE_MENU;
    return;
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

  vec2 corrected_flag_pos, corrected_flag_size;
  corrected_flag_pos[0] = level_objs.flag->position[0] + 32.0f;
  corrected_flag_pos[1] = level_objs.flag->position[1] + 32.0f;
  corrected_flag_size[0] = level_objs.flag->size[0] / 2;
  corrected_flag_size[1] = level_objs.flag->size[1] / 2;
  Collision_Result_t flag_collision = Collision_RectangleToRectangle(game->player.position, game->player.size,
                                                                     corrected_flag_pos, corrected_flag_size);
  if (flag_collision.collision)
  {
    game->state = GAME_STATE_WON;
    return;
  }

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

    Collision_Result_t collision_result = Collision_RectangleToRectangle(p->position, p->size, q->position, q->size);
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

  for (u32 i = 0; i < level_objs.num_fruits; ++i)
  {
    Player_t *p = &game->player;
    Level_Fruit_t *q = &level_objs.fruits[i];

    if (q->taken)
    {
      continue;
    }

    /*
    if ((p->position[1] + p->size[1]) < q->position[1])
    {
      continue;
    }

    if (p->position[1] > (q->position[1] + q->size[1]))
    {
      break;
    }
    */
    Collision_Result_t collision_result = Collision_RectangleToRectangle(p->position, p->size, q->quad.position, q->quad.size);
    if (collision_result.collision)
    {
      q->taken = true;
      game->score += 10;
      Hud_SetScore(game->hud, game->score);
    }
  }

  game->time += dt;
  Hud_SetTime(game->hud, game->time);

  Player_Update(&game->player);
  Level_Update(game->level, game->player.position);
  Hud_Update(game->hud, dt);
}

void state_won(Game_t *game, float dt)
{
  game->player.velocity[0] = 0.0f;
  game->player.velocity[1] = 0.0f;
  Player_Update(&game->player);
  Level_Update(game->level, game->player.position);
  Hud_Update(game->hud, dt);

  char *text = "LEVEL CLEARED.";
  TextRenderer_RenderString(game->text_renderer, text, (vec2){400.0f - strlen(text) * 8.0f * 2, 300.0f}, 4.0f);

  if (game->keys[GLFW_KEY_ENTER])
  {
    Level_Delete(game->level);
    Player_Delete(&game->player);
    game->level_no++;
    reset_game(game);
    Hud_SetLevel(game->hud, game->level_no);
  }
}

void state_menu(Game_t *game, float dt)
{
  if (game->keys[GLFW_KEY_UP] && !game->keys_processed[GLFW_KEY_UP])
  {
    Menu_Up(game->menu);
    game->keys_processed[GLFW_KEY_UP] = true;
  }

  else if (game->keys[GLFW_KEY_DOWN] && !game->keys_processed[GLFW_KEY_DOWN])
  {
    Menu_Down(game->menu);
    game->keys_processed[GLFW_KEY_DOWN] = true;
  }

  Level_Update(game->level, game->player.position);
  Menu_Render(game->menu);
}

Game_t *Game_Init(unsigned int width, unsigned int height)
{
  Game_t *game = malloc(sizeof(Game_t));
  game->hud = Hud_Init((vec2){0.0f, 550.0f}, (vec2){800.0f, 50.0f});
  game->text_renderer = TextRenderer_Init();
  game->menu = Menu_Init();
  game->level_no = 0;
  reset_game(game);
  return game;
}

void Game_Update(Game_t *game, float dt)
{
  switch (game->state)
  {
  case GAME_STATE_PLAYING:
    state_playing(game, dt);
    break;

  case GAME_STATE_WON:
    state_won(game, dt);
    break;

  case GAME_STATE_MENU:
    state_menu(game, dt);
    break;

  default:
    break;
  }
}

void Game_UpdateKeys(Game_t *game, int key, int action)
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
