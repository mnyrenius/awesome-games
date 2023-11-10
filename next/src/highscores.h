#pragma once

#include <stdbool.h>
#include "types.h"

typedef struct Highscores_t Highscores_t;
typedef enum Highscores_Mode_t
{
  HIGHSCORES_MODE_VIEW,
  HIGHSCORES_MODE_ADD
} Highscores_Mode_t;

Highscores_t *Highscores_Init(void);
void Highscores_Render(Highscores_t *highscores);
bool Highscores_CheckScore(Highscores_t *highscores, u32 score);
void Highscores_Add(Highscores_t *highscores, u32 score);
void Highscores_EnterKey(Highscores_t *highscores, u32 key);
Highscores_Mode_t Highscores_GetMode(Highscores_t *highscores);
