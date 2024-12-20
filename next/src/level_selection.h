#ifndef _LEVEL_SELECTION_H_
#define _LEVEL_SELECTION_H_

#include "types.h"

typedef struct LevelSelection_t LevelSelection_t;
typedef struct Highscores_t Highscores_t;

LevelSelection_t *LevelSelection_Init(Highscores_t *highscores);
void LevelSelection_Up(LevelSelection_t *level_selection);
void LevelSelection_Down(LevelSelection_t *level_selection);
void LevelSelection_Render(LevelSelection_t *level_selection);
u32 LevelSelection_Get(LevelSelection_t *level_selection);

#endif /* _LEVEL_SELECTION_H_ */
