#include <stdlib.h>
#include <stdio.h>
#include "hud.h"
#include "text_renderer.h"

#define NOTIFICATION_MAX_LENGTH 16

typedef struct Hud_t
{
  int score;
  int level;
  vec2 position;
  vec2 size;
  TextRenderer_t *renderer;
  char notification[NOTIFICATION_MAX_LENGTH];
  float notification_timer;
} Hud_t;

Hud_t *Hud_Init(vec2 position, vec2 size)
{
  Hud_t *hud = malloc(sizeof(Hud_t));

  hud->score = 0;
  hud->level = 0;
  vec2_dup(hud->position, position);
  vec2_dup(hud->size, size);

  hud->renderer = TextRenderer_Init();
  hud->notification_timer = 0.0f;

  return hud;
}

void Hud_Update(Hud_t *hud, float dt)
{
  char str[16];
  vec2 pos = { 20.0f, hud->position[1] + 20.0f };
  sprintf(str, "Score: %d", hud->score);
  TextRenderer_RenderString(hud->renderer, str, pos, 2.0f);
  sprintf(str, "Level: %d", hud->level);
  pos[0] = hud->size[0] - 20.0f - strlen(str) * 8.0f * 2.0f;
  TextRenderer_RenderString(hud->renderer, str, pos, 2.0f);

  hud->notification_timer += dt;
  if (hud->notification_timer > 0.5f)
  {
    pos[0] = hud->size[0] / 2.0f - strlen(hud->notification) * 8.0f;
    TextRenderer_RenderString(hud->renderer, hud->notification, pos, 2.0f);
    if (hud->notification_timer > 1.0f)
    {
      hud->notification_timer = 0.0f;
    }
  }
}

void Hud_SetScore(Hud_t *hud, int score)
{
  hud->score = score;
}

void Hud_SetLevel(Hud_t *hud, int level)
{
  hud->level = level;
}

void Hud_SetNotification(Hud_t *hud, const char *notification)
{
  strncpy(hud->notification, notification, NOTIFICATION_MAX_LENGTH - 1);
}

void Hud_ResetNotification(Hud_t *hud)
{
  strcpy(hud->notification, "");
}