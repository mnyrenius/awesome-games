#pragma once

#include "types.h"

#define NUM_LEVELS 10

typedef struct Highscores_t Highscores_t;

Highscores_t *Highscores_Init(void);
void Highscores_TryAdd(Highscores_t *highscores, u32 level, f32 time);
f32 *Highscores_GetHighscores(Highscores_t *highscores);