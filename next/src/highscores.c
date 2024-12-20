#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include "highscores.h"
#include "shader.h"
#include "util.h"
#include "types.h"

#define HIGHSCORE_FILE "jumpf.dat"

#ifdef __linux__
#define HOME_ENVVAR "HOME"
#elif _WIN32
#define HOME_ENVVAR "USERPROFILE"
#endif

typedef struct Highscores_t
{
  f32 time[NUM_LEVELS];
} Highscores_t;

static u32 calc_checksum(Highscores_t *highscores)
{
  u32 checksum = 0;
  char *p = (char *)&highscores->time;
  for (u32 i = 0; i < sizeof(highscores->time); ++i)
  {
    checksum += p[i];
  }

  return checksum;
}

static FILE *open_file(const char *mode)
{
  FILE *fp = NULL;
  char *home = getenv(HOME_ENVVAR);
  if (home)
  {
    char *path = malloc(strlen(home) + strlen(HIGHSCORE_FILE) + 2);
    sprintf(path, "%s/.%s", home, HIGHSCORE_FILE);
    LOG("Open %s\n", path);
    fp = fopen(path, mode);
    free(path);
  }

  return fp;
}

static void write_file(Highscores_t *highscores)
{
  FILE *fp = open_file("wb+");
  if (!fp)
  {
    LOG("Failed to open file: %s\n", HIGHSCORE_FILE);
    return;
  }

  u32 checksum = calc_checksum(highscores);

  fwrite(&checksum, sizeof(u32), 1, fp);
  fwrite(highscores->time, sizeof(f32), NUM_LEVELS, fp);
  fclose(fp);
}

Highscores_t *Highscores_Init(void)
{
  Highscores_t *highscores = malloc(sizeof(Highscores_t));

  FILE *fp = open_file("rb");
  bool read_success = false;
  if (fp)
  {
    u32 checksum = -1;
    size_t checksum_read = fread(&checksum, sizeof(u32), 1, fp);
    size_t hs_read = fread(highscores->time, sizeof(f32), NUM_LEVELS, fp);
    if (checksum_read == 1 &&
        hs_read == NUM_LEVELS &&
        checksum == calc_checksum(highscores))
    {
      read_success = true;
      LOG("%s\n", "Read highscore file ok");
    }
  }

  if (!read_success)
  {
    LOG("%s\n", "Failed to read highscore file, reset it");

    for (u32 i = 0; i < NUM_LEVELS; ++i)
    {
      highscores->time[i] = -1.0f;
    }

    write_file(highscores);
  }

  return highscores;
}

void Highscores_TryAdd(Highscores_t *highscores, u32 level, f32 time)
{
  if (level <= NUM_LEVELS && (time < highscores->time[level] || highscores->time[level] == -1.0f))
  {
    highscores->time[level] = time;
    write_file(highscores);
  }
}

f32 *Highscores_GetHighscores(Highscores_t *highscores)
{
  return highscores->time;
}