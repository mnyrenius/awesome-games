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
  PLAYER_STATE_JUMPING,
  PLAYER_STATE_FALLING,
  PLAYER_STATE_END,
} Player_State_t;

typedef struct Player_Internal_t
{
  Player_State_t state;
  SpriteRenderer_t *renderer;
  Texture_t *textures[PLAYER_STATE_END];
  bool at_ground;
} Player_Internal_t;

Player_t Player_Init(void)
{
  Player_Internal_t *internal = malloc(sizeof(Player_Internal_t));

  internal->state = PLAYER_STATE_IDLE;
  internal->renderer = SpriteRenderer_Init();
  internal->textures[PLAYER_STATE_IDLE] = Texture_Init(&Virtual_Guy_Idle_32x32_png, Virtual_Guy_Idle_32x32_png_len, 11);
  internal->textures[PLAYER_STATE_RUNNING] = Texture_Init(&Virtual_Guy_Run_32x32_png, Virtual_Guy_Run_32x32_png_len, 12);
  internal->textures[PLAYER_STATE_JUMPING] = Texture_Init(&Virtual_Guy_Jump_32x32_png, Virtual_Guy_Jump_32x32_png_len, 1);
  internal->textures[PLAYER_STATE_FALLING] = Texture_Init(&Virtual_Guy_Fall_32x32_png, Virtual_Guy_Fall_32x32_png_len, 1);
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
  bool flip = false;

  if (player->velocity[0] > -0.1f && player->velocity[0] < 0.1f && player->velocity[1] > -0.1f && player->velocity[1] < 0.1f)
  {
    player->internal->state = PLAYER_STATE_IDLE;
    if (player->last_vel_x < 0.0f)
    {
      flip = true;
    }
  }

  else if (player->velocity[1] < 0.0f)
  {
    player->internal->state = PLAYER_STATE_JUMPING;

    if (player->last_vel_x < 0.0f)
    {
      flip = true;
    }
  }

  else if (player->velocity[1] > 0.0f)
  {
    player->internal->state = PLAYER_STATE_FALLING;

    if (player->last_vel_x < 0.0f)
    {
      flip = true;
    }
  }

  else if (player->velocity[0] < 0.0f)
  {
    player->internal->state = PLAYER_STATE_RUNNING;
    flip = true;
  }

  else if (player->velocity[0] > 0.0f)
  {
    player->internal->state = PLAYER_STATE_RUNNING;
  }

  vec2 uv = {0.0f, 0.0f};
  SpriteRenderer_RenderObject(player->internal->renderer, player->internal->textures[player->internal->state], player->position, player->size, uv, flip);

  vec2 view = {0.0f, 0.0f - Global_Time.now * 10.0f};

  if (player->position[1] < 300.0f)
  {
    view[1] = player->position[1] - 300.0f - Global_Time.now * 10.0f;
  }
  SpriteRenderer_UpdateOrtho(player->internal->renderer, view);
}

void Player_Delete(Player_t *player)
{
  SpriteRenderer_Delete(player->internal->renderer);

  for (u32 i = 0; i < PLAYER_STATE_END; ++i)
  {
    Texture_Delete(player->internal->textures[i]);
  }

  free(player->internal);
}