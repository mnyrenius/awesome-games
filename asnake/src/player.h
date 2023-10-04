#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "linmath.h"

typedef struct Player_t Player_t;
typedef enum Player_Movement_t
{
  PLAYER_MOVEMENT_UP,
  PLAYER_MOVEMENT_DOWN,
  PLAYER_MOVEMENT_LEFT,
  PLAYER_MOVEMENT_RIGHT,
  PLAYER_MOVEMENT_STOP,
} Player_Movement_t;

Player_t * Player_Init(void);
void Player_Update(Player_t * player, float dt);
void Player_Move(Player_t * player, Player_Movement_t movement);
vec2 *Player_GetPosition(Player_t *player);
void Player_Eat(Player_t *player, bool powerup);
bool Player_IsDead(Player_t *player);
bool Player_IsInvincible(Player_t *player);
void Player_Delete(Player_t *player);

#endif /* _PLAYER_H_ */