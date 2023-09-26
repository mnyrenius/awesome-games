#ifndef _HUD_H_
#define _HUD_H_
#include "linmath.h"

typedef struct Hud_t Hud_t;

Hud_t *Hud_Init(vec2 position, vec2 size);
void Hud_Update(Hud_t *hud);
void Hud_SetScore(Hud_t *hud, int score);
void Hud_SetLevel(Hud_t *hud, int level);

#endif /* _HUD_H_ */