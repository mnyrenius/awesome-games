#include <stdlib.h>
#include <stdbool.h>
#include "linmath.h"
#include "player.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "sprites.h"
#include "global.h"
#include "GLFW/glfw3.h"
#include "util.h"

typedef enum Player_State_t
{
  PLAYER_STATE_IDLE,
  PLAYER_STATE_RUNNING,
  PLAYER_STATE_END,
} Player_State_t;

typedef enum Player_Sprite_t
{
  PLAYER_SPRITE_IDLE_LEFT,
  PLAYER_SPRITE_IDLE_RIGHT,
  PLAYER_SPRITE_RUN_LEFT,
  PLAYER_SPRITE_RUN_RIGHT,
  PLAYER_SPRITE_JUMP_LEFT,
  PLAYER_SPRITE_JUMP_RIGHT,
  PLAYER_SPRITE_FALL_LEFT,
  PLAYER_SPRITE_FALL_RIGHT,
  PLAYER_SPRITE_END
} Player_Sprite_t;

typedef struct Player_Internal_t
{
  Player_State_t state;
  Player_Sprite_t sprite;
  SpriteRenderer_t *renderer;
  Texture_t *textures[PLAYER_SPRITE_END];
  bool at_ground;
} Player_Internal_t;

Player_t Player_Init(void)
{
  Player_Internal_t *internal = malloc(sizeof(Player_Internal_t));

  internal->state = PLAYER_STATE_IDLE;
  internal->sprite = PLAYER_SPRITE_IDLE_RIGHT;
  internal->renderer = SpriteRenderer_Init();
  internal->textures[PLAYER_SPRITE_IDLE_LEFT] = Texture_Init(&Virtual_Guy_Idle_Left_32x32_png, Virtual_Guy_Idle_Left_32x32_png_len, 11);
  internal->textures[PLAYER_SPRITE_IDLE_RIGHT] = Texture_Init(&Virtual_Guy_Idle_32x32_png, Virtual_Guy_Idle_32x32_png_len, 11);
  internal->textures[PLAYER_SPRITE_RUN_LEFT] = Texture_Init(&Virtual_Guy_Run_Left_32x32_png, Virtual_Guy_Run_Left_32x32_png_len, 12);
  internal->textures[PLAYER_SPRITE_RUN_RIGHT] = Texture_Init(&Virtual_Guy_Run_32x32_png, Virtual_Guy_Run_32x32_png_len, 12);
  internal->textures[PLAYER_SPRITE_JUMP_LEFT] = Texture_Init(&Virtual_Guy_Jump_Left_32x32_png, Virtual_Guy_Jump_Left_32x32_png_len, 1);
  internal->textures[PLAYER_SPRITE_JUMP_RIGHT] = Texture_Init(&Virtual_Guy_Jump_32x32_png, Virtual_Guy_Jump_32x32_png_len, 1);
  internal->textures[PLAYER_SPRITE_FALL_LEFT] = Texture_Init(&Virtual_Guy_Fall_Left_32x32_png, Virtual_Guy_Fall_Left_32x32_png_len, 1);
  internal->textures[PLAYER_SPRITE_FALL_RIGHT] = Texture_Init(&Virtual_Guy_Fall_32x32_png, Virtual_Guy_Fall_32x32_png_len, 1);
  internal->at_ground = false;

  return (Player_t)
  {
    .position = {400.0f, 500.0f},
    .size = {32.0f, 32.0f},
    .velocity = {0.0f, 0.0f},
    .last_vel_x = 1.0f,
    .internal = internal
  };
}

void Player_Update(Player_t *player)
{
  if (player->velocity[0] > -0.1f && player->velocity[0] < 0.1f && player->velocity[1] > -0.1f && player->velocity[1] < 0.1f)
  {
    if (player->last_vel_x > 0.0f)
    {
      player->internal->sprite = PLAYER_SPRITE_IDLE_RIGHT;
    }
    else
    {
      player->internal->sprite = PLAYER_SPRITE_IDLE_LEFT;
    }
  }

  else if (player->velocity[1] < 0.0f)
  {
    if (player->last_vel_x > 0.0f)
    {
      player->internal->sprite = PLAYER_SPRITE_JUMP_RIGHT;
    }
    else
    {
      player->internal->sprite = PLAYER_SPRITE_JUMP_LEFT;
    }
  }

  else if (player->velocity[1] > 0.0f)
  {
    if (player->last_vel_x > 0.0f)
    {
      player->internal->sprite = PLAYER_SPRITE_FALL_RIGHT;
    }
    else
    {
      player->internal->sprite = PLAYER_SPRITE_FALL_LEFT;
    }
  }

  else if (player->velocity[0] < 0.0f)
  {
    player->internal->sprite = PLAYER_SPRITE_RUN_LEFT;
  }

  else if (player->velocity[0] > 0.0f)
  {
    player->internal->sprite = PLAYER_SPRITE_RUN_RIGHT;
  }

  SpriteRenderer_RenderObject(player->internal->renderer, player->internal->textures[player->internal->sprite], player->position, player->size, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
}

void Player_Delete(Player_t *player)
{

}