#include <stdlib.h>
#include <stdio.h>
#include "hud.h"
#include "text_renderer.h"

typedef struct Hud_t
{
  int score;
  int level;
  vec2 position;
  vec2 size;
  TextRenderer_t *renderer;
} Hud_t;

Hud_t *Hud_Init(vec2 position, vec2 size)
{
  Hud_t *hud = malloc(sizeof(Hud_t));

  hud->score = 0;
  hud->level = 0;
  vec2_dup(hud->position, position);
  vec2_dup(hud->size, size);

  hud->renderer = TextRenderer_Init();

  return hud;
}

void Hud_Update(Hud_t *hud)
{
  char str[16];
  vec2 pos = { 20.0f, hud->position[1] + 20.0f };
  sprintf(str, "Score: %d", hud->score);
  TextRenderer_RenderString(hud->renderer, str, pos, 2.0f);
  sprintf(str, "Level: %d", hud->level);
  pos[0] = hud->size[0] - 20.0f - strlen(str) * 8 * 2.0f;
  TextRenderer_RenderString(hud->renderer, str, pos, 2.0f);
}

void Hud_SetScore(Hud_t *hud, int score)
{
  hud->score = score;
}

void Hud_SetLevel(Hud_t *hud, int level)
{
  hud->level = level;
}

