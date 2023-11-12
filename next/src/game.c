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
#include <math.h>
#include "highscores.h"

typedef enum Game_State_t
{
  GAME_STATE_PLAYING,
  GAME_STATE_WON,
  GAME_STATE_MENU,
  GAME_STATE_HIGHSCORES,
} Game_State_t;

typedef struct Game_t
{
  Game_State_t state;
  Player_t player;
  Level_t *level;
  Menu_t *menu;
  Hud_t *hud;
  Highscores_t *highscores;
  TextRenderer_t *text_renderer;
  u32 width;
  u32 height;
  bool keys[512];
  bool keys_processed[512];
  bool player_at_ground;
  bool player_max_jump;
  u32 level_no;
  f32 time;
  int score;
  bool quit;
  f32 menu_camera_position;
  u32 num_fruits_taken;
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
  game->time = 0;
  game->num_fruits_taken = 0;
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

  if ((game->keys[GLFW_KEY_ESCAPE] && !game->keys_processed[GLFW_KEY_ESCAPE]) || (game->keys[GLFW_KEY_Q] && !game->keys_processed[GLFW_KEY_Q]))
  {
    game->keys_processed[GLFW_KEY_ESCAPE] = true;
    game->keys_processed[GLFW_KEY_Q] = true;
    Level_SetAlpha(game->level, 0.2f);
    game->state = GAME_STATE_MENU;
    return;
  }

  if (game->keys[GLFW_KEY_S] && !game->keys_processed[GLFW_KEY_S])
  {
    Level_Delete(game->level);
    Player_Delete(&game->player);
    game->level_no++;
    reset_game(game);
    game->state = GAME_STATE_PLAYING;
    Hud_SetLevel(game->hud, game->level_no);
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
  if (flag_collision.collision && game->num_fruits_taken == level_objs.num_fruits)
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
      game->num_fruits_taken++;
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
    game->state = GAME_STATE_PLAYING;
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

  else if (game->keys[GLFW_KEY_Q] && !game->keys_processed[GLFW_KEY_Q])
  {
    game->quit = true;
  }

  else if (game->keys[GLFW_KEY_ENTER])
  {
    switch (Menu_Get(game->menu))
    {
    case MENU_ITEM_PLAY:
      Level_SetAlpha(game->level, 1.0f);
      game->state = GAME_STATE_PLAYING;
      break;
    case MENU_ITEM_HIGHSCORES:
      game->state = GAME_STATE_HIGHSCORES;
      break;
    case MENU_ITEM_QUIT:
      game->quit = true;

    default:
      break;
    }
  }

  game->menu_camera_position -= 0.01 * dt;

  Level_Update(game->level, (vec2){400.0f, 800.0f * cos(game->menu_camera_position)});
  Menu_Render(game->menu);
}

void state_highscores(Game_t *game, float dt)
{
  if (Highscores_GetMode(game->highscores) == HIGHSCORES_MODE_VIEW)
  {
    if ((game->keys[GLFW_KEY_ESCAPE] && !game->keys_processed[GLFW_KEY_ESCAPE]) ||
        (game->keys[GLFW_KEY_Q] && !game->keys_processed[GLFW_KEY_Q]))
    {
      game->state = GAME_STATE_MENU;
      game->keys_processed[GLFW_KEY_ESCAPE] = true;
      game->keys_processed[GLFW_KEY_Q] = true;
    }
  }

  else if (Highscores_GetMode(game->highscores) == HIGHSCORES_MODE_ADD)
  {
    if (game->keys[GLFW_KEY_ENTER] && !game->keys_processed[GLFW_KEY_ENTER])
    {
      Highscores_EnterKey(game->highscores, GLFW_KEY_ENTER);
      game->keys_processed[GLFW_KEY_ENTER] = true;
    }

    else if (game->keys[GLFW_KEY_BACKSPACE] && !game->keys_processed[GLFW_KEY_BACKSPACE])
    {
      Highscores_EnterKey(game->highscores, GLFW_KEY_BACKSPACE);
      game->keys_processed[GLFW_KEY_BACKSPACE] = true;
    }

    else
    {
      for (int i = GLFW_KEY_A; i <= GLFW_KEY_Z; ++i)
      {
        if (game->keys[i] && !game->keys_processed[i])
        {
          Highscores_EnterKey(game->highscores, i);
          game->keys_processed[i] = true;
        }
      }
    }
  }

  game->menu_camera_position -= 0.01 * dt;

  Level_Update(game->level, (vec2){400.0f, 800.0f * cos(game->menu_camera_position)});
  Highscores_Render(game->highscores);
}

Game_t *Game_Init(unsigned int width, unsigned int height)
{
  Game_t *game = malloc(sizeof(Game_t));
  game->hud = Hud_Init((vec2){0.0f, 550.0f}, (vec2){800.0f, 50.0f});
  game->highscores = Highscores_Init();
  game->text_renderer = TextRenderer_Init();
  game->menu = Menu_Init();
  game->level_no = 1;
  game->quit = false;
  game->state = GAME_STATE_MENU;
  game->menu_camera_position = 300.0f;
  reset_game(game);
  Level_SetAlpha(game->level, 0.2f);
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

  case GAME_STATE_HIGHSCORES:
    state_highscores(game, dt);
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
  return game->quit;
}
