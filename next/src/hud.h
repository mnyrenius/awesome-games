#pragma once

#include "linmath.h"
#include "types.h"

typedef struct Hud_t Hud_t;

Hud_t *Hud_Init(vec2 position, vec2 size);
void Hud_Update(Hud_t *hud, float dt);
void Hud_SetTime(Hud_t *hud, f32 time);
void Hud_SetLevel(Hud_t *hud, int level);
void Hud_SetScore(Hud_t *hud, int score);
void Hud_SetNotification(Hud_t *hud, const char *notification);
void Hud_ResetNotification(Hud_t *hud);
