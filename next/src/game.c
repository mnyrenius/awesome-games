#include <stdlib.h>
#include "game.h"

typedef struct Game_t
{
  int dummy;
} Game_t;

Game_t * Game_Init(unsigned int width, unsigned int height)
{
  Game_t *game = malloc(sizeof(Game_t));
  return game;
}

void Game_Update(Game_t * game, float dt)
{

}

void Game_UpdateKeys(Game_t * game, int key, int action)
{

}
bool Game_IsQuit(Game_t *game)
{
  return false;
}

