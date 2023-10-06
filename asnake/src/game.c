#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "game.h"
#include "player.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "linmath.h"
#include "shader.h"
#include "level.h"
#include "hud.h"
#include "text_renderer.h"
#include "collision.h"
#include "highscores.h"
#include "menu.h"

typedef enum Game_State_t
{
  GAME_STATE_MENU,
  GAME_STATE_PLAYING,
  GAME_STATE_WON,
  GAME_STATE_DEAD,
  GAME_STATE_HIGHSCORES,
} Game_State_t;

typedef struct Game_t
{
  unsigned int width;
  unsigned int height;
  Player_t * player;
  bool keys[512];
  bool keys_processed[512];
  Level_t *level;
  Game_State_t state;
  Hud_t *hud;
  int score;
  int level_no;
  TextRenderer_t *text;
  Highscores_t *highscores;
  Menu_t *menu;
  bool quit;
} Game_t;

void reset_level(Game_t *game, bool reset_score_level)
{
  Level_Delete(game->level);

  if (reset_score_level)
  {
    game->score = 0;
    game->level_no = 1;
  }

  game->level = Level_Init(game->width, game->height - 50, game->level_no);

  Player_Delete(game->player);
  game->player = Player_Init();
  Hud_SetScore(game->hud, game->score);
  Hud_SetLevel(game->hud, game->level_no);
  game->state = GAME_STATE_PLAYING;
}

void skip_level(Game_t *game)
{
  Level_Delete(game->level);
  game->level_no++;
  game->level = Level_Init(game->width, game->height - 50, game->level_no);
  Player_Delete(game->player);
  game->player = Player_Init();
  Hud_SetScore(game->hud, game->score);
  Hud_SetLevel(game->hud, game->level_no);
  game->state = GAME_STATE_PLAYING;
}

void state_playing(Game_t *game, float dt)
{
  if (game->keys[GLFW_KEY_UP])
  {
    Player_Move(game->player, PLAYER_MOVEMENT_UP);
  }

  else if (game->keys[GLFW_KEY_DOWN])
  {
    Player_Move(game->player, PLAYER_MOVEMENT_DOWN);
  }

  else if (game->keys[GLFW_KEY_LEFT])
  {
    Player_Move(game->player, PLAYER_MOVEMENT_LEFT);
  }

  else if (game->keys[GLFW_KEY_RIGHT])
  {
    Player_Move(game->player, PLAYER_MOVEMENT_RIGHT);
  }

  else if (game->keys[GLFW_KEY_R] && !game->keys_processed[GLFW_KEY_R])
  {
    reset_level(game, false);
    game->keys_processed[GLFW_KEY_R] = true;
    return;
  }

  else if (game->keys[GLFW_KEY_S] && !game->keys_processed[GLFW_KEY_S])
  {
    skip_level(game);
    game->keys_processed[GLFW_KEY_S] = true;
    return;
  }

  else if (game->keys[GLFW_KEY_ESCAPE] || (game->keys[GLFW_KEY_Q] && !game->keys_processed[GLFW_KEY_Q]))
  {
    game->state = GAME_STATE_MENU;
    game->keys_processed[GLFW_KEY_Q] = true;
    Level_SetAlpha(game->level, 0.2f);
    return;
  }

  Player_Update(game->player, dt);

  if (Player_IsDead(game->player))
  {
    game->state = GAME_STATE_DEAD;
  }

  vec2 player_pos;
  vec2_dup(player_pos, *Player_GetPosition(game->player));

  Level_Objects_t level_objects = Level_GetObjects(game->level);
  for (int i = 0; i < level_objects.num_foods; ++i)
  {
    if (level_objects.foods[i].eaten)
      continue;

    if (Collision_PointToPoint(level_objects.foods[i].position, player_pos, 10.0f))
    {
      Level_EatFood(game->level, i);
      Player_Eat(game->player, level_objects.foods[i].powerup);
      game->score += 10;
      Hud_SetScore(game->hud, game->score);
    }
  }

  if (!Player_IsInvincible(game->player))
  {
    for (int i = 0; i < level_objects.num_walls; ++i)
    {
      Level_Wall_t *w = &level_objects.walls[i];
      if (Collision_PointToRectangle(player_pos, w->position, w->size))
      {
        Player_Move(game->player, PLAYER_MOVEMENT_STOP);
        game->state = GAME_STATE_DEAD;
      }
    }
  }

  if (Level_IsFinished(game->level))
  {
    Player_Move(game->player, PLAYER_MOVEMENT_STOP);
    game->state = GAME_STATE_WON;
  }

  Level_Render(game->level);

  if (Player_IsInvincible(game->player))
  {
    Hud_SetNotification(game->hud, "[INVINCIBLE]");
  }

  else
  {
    Hud_ResetNotification(game->hud);
  }

  Hud_Update(game->hud, dt);
}

void state_won(Game_t *game, float dt)
{
  Player_Update(game->player, dt);
  Level_Render(game->level);
  Hud_Update(game->hud, dt);

  const char *str = "Level Cleared. Press ENTER to continue.";
  vec2 text_pos = {game->width / 2.0f - strlen(str) * 8.0f, 200.f};
  TextRenderer_RenderString(game->text, str, text_pos, 2.0f);

  if (game->keys[GLFW_KEY_ENTER])
  {
    Level_Delete(game->level);
    game->level_no++;
    game->level = Level_Init(game->width, game->height - 50, game->level_no);
    Player_Delete(game->player);
    game->player = Player_Init();
    Hud_SetLevel(game->hud, game->level_no);
    game->state = GAME_STATE_PLAYING;
  }
}

void state_dead(Game_t *game, float dt)
{
  Player_Update(game->player, dt);
  Level_Render(game->level);
  Hud_Update(game->hud, dt);

  if (Highscores_CheckScore(game->highscores, game->score))
  {
    const char *str = "NEW HIGH SCORE! Press ENTER to proceed";
    vec2 text_pos = {game->width / 2.0f - strlen(str) * 8.0f, 200.f};
    TextRenderer_RenderString(game->text, str, text_pos, 2.0f);
  }

  else
  {
    const char *str = "You died. Pess ENTER to try again.";
    vec2 text_pos = {game->width / 2.0f - strlen(str) * 8.0f, 200.f};
    TextRenderer_RenderString(game->text, str, text_pos, 2.0f);
  }

  if (game->keys[GLFW_KEY_ENTER] && !game->keys_processed[GLFW_KEY_ENTER])
  {
    if (Highscores_CheckScore(game->highscores, game->score))
    {
      game->state = GAME_STATE_HIGHSCORES;
      Highscores_Add(game->highscores, game->score);
    }

    else
    {
      reset_level(game, true);
    }

    game->keys_processed[GLFW_KEY_ENTER] = true;
  }
}

void state_highscores(Game_t *game, float dt)
{
  Level_Render(game->level);

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

  Highscores_Render(game->highscores);
}

void state_menu(Game_t *game, float dt)
{
  Level_Render(game->level);

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
    game->keys_processed[GLFW_KEY_Q] = true;
    return;
  }

  else if (game->keys[GLFW_KEY_ENTER] && !game->keys_processed[GLFW_KEY_ENTER])
  {
    game->keys_processed[GLFW_KEY_ENTER] = true;
    switch (Menu_Get(game->menu))
    {
    case MENU_ITEM_PLAY:
      game->state = GAME_STATE_PLAYING;
      Level_SetAlpha(game->level, 1.0f);
      reset_level(game, true);
      break;
    case MENU_ITEM_HIGHSCORES:
      game->state = GAME_STATE_HIGHSCORES;
      break;
    case MENU_ITEM_QUIT:
      game->quit = true;
      break;

    default:
      break;
    }
  }

  Menu_Render(game->menu);
}

Game_t * Game_Init(unsigned int width, unsigned int height)
{
 srand(time(NULL));
 Game_t * game = malloc(sizeof(Game_t));
 game->width = width;
 game->height = height;
 game->player = Player_Init();
 memset(game->keys, 0, sizeof(game->keys)/sizeof(game->keys[0]));
 memset(game->keys_processed, 0, sizeof(game->keys_processed)/sizeof(game->keys_processed[0]));
 game->level_no = 1;
 game->level = Level_Init(800, 550, game->level_no);
 game->state = GAME_STATE_MENU;
 vec2 hud_pos = { 0.0f, 550.0f };
 vec2 hud_size = { 800.0f, 50.0f };
 game->hud = Hud_Init(hud_pos, hud_size);
 game->score = 0;
 game->text = TextRenderer_Init();
 game->highscores = Highscores_Init();
 game->menu = Menu_Init();
 game->quit = false;

 Hud_SetScore(game->hud, game->score);
 Hud_SetLevel(game->hud, game->level_no);
 Level_SetAlpha(game->level, 0.2f);

 return game;
}

void Game_Update(Game_t * game, float dt)
{
  switch (game->state)
  {
    case GAME_STATE_MENU:
      state_menu(game, dt);
      break;

    case GAME_STATE_PLAYING:
      state_playing(game, dt);
      break;

    case GAME_STATE_DEAD:
      state_dead(game, dt);
      break;

    case GAME_STATE_WON:
      state_won(game, dt);
      break;

    case GAME_STATE_HIGHSCORES:
      state_highscores(game, dt);
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
  return game->quit;
}
