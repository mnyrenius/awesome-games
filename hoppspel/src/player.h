#pragma once

#include "linmath.h"
#include "types.h"

typedef struct Player_Internal_t Player_Internal_t;
typedef struct Player_t
{
  vec2 position;
  vec2 size;
  vec2 velocity;
  f32 last_vel_x;
  Player_Internal_t *internal;
} Player_t;

Player_t Player_Init(void);
void Player_Update(Player_t *player);
void Player_Delete(Player_t *player);