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

typedef enum Game_State_t
{
  GAME_STATE_PLAYING,
  GAME_STATE_WON,
  GAME_STATE_DEAD,
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
} Game_t;

void reset_level(Game_t *game, bool reset_score_level)
{
  Level_Delete(game->level);
  game->level = Level_Init(game->width, game->height - 50, game->level_no);

  if (reset_score_level)
  {
    game->score = 0;
    game->level_no = 1;
  }

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
  vec2 new_player_velocity = {0.0f, 0.0f};

  if (game->keys[GLFW_KEY_UP])
  {
    new_player_velocity[1] = -100.f;
    Player_SetVelocity(game->player, new_player_velocity);
  }

  else if (game->keys[GLFW_KEY_DOWN])
  {
    new_player_velocity[1] = 100.f;
    Player_SetVelocity(game->player, new_player_velocity);
  }

  else if (game->keys[GLFW_KEY_LEFT])
  {
    new_player_velocity[0] = -100.f;
    Player_SetVelocity(game->player, new_player_velocity);
  }

  else if (game->keys[GLFW_KEY_RIGHT])
  {
    new_player_velocity[0] = 100.f;
    Player_SetVelocity(game->player, new_player_velocity);
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
      Player_Eat(game->player);
      game->score += 10;
      Hud_SetScore(game->hud, game->score);
    }
  }

  for (int i = 0; i < level_objects.num_walls; ++i)
  {
    Level_Wall_t *w = &level_objects.walls[i];
    if (Collision_PointToRectangle(player_pos, w->position, w->size))
    {
      vec2 v = { 0.0f, 0.0f };
      Player_SetVelocity(game->player, v);
      game->state = GAME_STATE_DEAD;
    }
  }

  if (Level_IsFinished(game->level))
  {
    vec2 v = {0.0f, 0.0f};
    Player_SetVelocity(game->player, v);
    game->state = GAME_STATE_WON;
  }

  Level_Render(game->level);
  Hud_Update(game->hud);
}

void state_won(Game_t *game, float dt)
{
  Player_Update(game->player, dt);
  Level_Render(game->level);
  Hud_Update(game->hud);

  const char *str = "Level Cleared. Press ENTER to continue.";
  vec2 text_pos = {game->width / 2 - strlen(str) * 8, 200.f};
  TextRenderer_RenderString(game->text, str, text_pos, 2.0f);

  if (game->keys[GLFW_KEY_ENTER])
  {
    Level_Delete(game->level);
    game->level_no++;
    game->level = Level_Init(game->width, game->height - 100, game->level_no);
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
  Hud_Update(game->hud);

  const char *str = "You died. Pess ENTER to try again.";
  vec2 text_pos = {game->width / 2 - strlen(str) * 8, 200.f};
  TextRenderer_RenderString(game->text, str, text_pos, 2.0f);

  if (game->keys[GLFW_KEY_ENTER])
  {
    reset_level(game, true);
  }
}

Game_t * Game_Init(unsigned int width, unsigned int height)
{
 srand(time(NULL));
 Game_t * game = malloc(sizeof(Game_t));
 game->width = width;
 game->height = height;
 game->player = Player_Init();
 memset(game->keys, 0, sizeof(game->keys)/sizeof(game->keys[0]));
 game->level_no = 1;
 game->level = Level_Init(800, 550, game->level_no);
 game->state = GAME_STATE_PLAYING;
 vec2 hud_pos = { 0.0f, 550.0f };
 vec2 hud_size = { 800.0f, 50.0f };
 game->hud = Hud_Init(hud_pos, hud_size);
 game->score = 0;
 game->text = TextRenderer_Init();

 Hud_SetScore(game->hud, game->score);
 Hud_SetLevel(game->hud, game->level_no);

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

  case GAME_STATE_WON:
    state_won(game, dt);
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

bool Game_IsGameOver(Game_t *game)
{
  return game->state != GAME_STATE_PLAYING;
}