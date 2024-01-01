#pragma once

#include "types.h"

#define NUM_LEVELS 99

typedef struct Highscores_t Highscores_t;

Highscores_t *Highscores_Init(void);
void Highscores_Render(Highscores_t *highscores);
void Highscores_TryAdd(Highscores_t *highscores, u32 level, u32 score_seconds);
u32 *Highscores_GetHighscores(Highscores_t *highscores);