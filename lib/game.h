#ifndef _GAME_H_
#define _GAME_H_

#include <stdbool.h>

typedef struct Game_t Game_t;

Game_t * Game_Init(unsigned int width, unsigned int height);
void Game_Update(Game_t * game, float dt);
void Game_UpdateKeys(Game_t * game, int key, int action);
bool Game_IsQuit(Game_t *game);

#endif /* _GAME_H_ */