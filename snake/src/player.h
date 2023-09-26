#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "linmath.h"

typedef struct Player_t Player_t;

Player_t * Player_Init(void);
void Player_Update(Player_t * player, float dt);
void Player_SetVelocity(Player_t * player, vec2 velocity);
vec2 *Player_GetPosition(Player_t *player);
void Player_Eat(Player_t *player);
bool Player_IsDead(Player_t *player);
void Player_Delete(Player_t *player);

#endif /* _PLAYER_H_ */