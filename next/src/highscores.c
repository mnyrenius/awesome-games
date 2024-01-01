#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include "highscores.h"
#include "renderer.h"
#include "text_renderer.h"
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
  Renderer_t *renderer;
  TextRenderer_t *text_renderer;
  TextRenderer_t *title_renderer;
  Shader_t *title_shader;
  u32 scores_seconds[NUM_LEVELS];
  f32 counter;
} Highscores_t;

static u32 calc_checksum(Highscores_t *highscores)
{
  u32 checksum = 0;
  char *p = (char *)&highscores->scores_seconds;
  for (u32 i = 0; i < sizeof(highscores->scores_seconds); ++i)
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
  fwrite(highscores->scores_seconds, sizeof(u32), NUM_LEVELS, fp);
  fclose(fp);
}

Highscores_t *Highscores_Init(void)
{
  Highscores_t *highscores = malloc(sizeof(Highscores_t));

  f32 vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  highscores->renderer = Renderer_Init(vertices, sizeof(vertices));
  highscores->title_shader = Shader_Init();

  const char vs[] =
      {
#include "shaders/text.vs.data"
          , 0};

  const char fs[] =
      {
#include "shaders/text.fs.data"
          , 0};
  Shader_Load(highscores->title_shader, vs, fs);
  highscores->title_renderer = TextRenderer_Init_With_Shader(highscores->title_shader);

  highscores->text_renderer = TextRenderer_Init();

  FILE *fp = open_file("rb");
  bool read_success = false;
  if (fp)
  {
    u32 checksum = -1;
    size_t checksum_read = fread(&checksum, sizeof(u32), 1, fp);
    size_t hs_read = fread(highscores->scores_seconds, sizeof(u32), NUM_LEVELS, fp);
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
    for (u32 i = 3; i < NUM_LEVELS; ++i)
    {
      highscores->scores_seconds[i] = 0;
    }
  }

  highscores->counter = 0.0f;

  return highscores;
}

void Highscores_Render(Highscores_t *highscores)
{
  vec2 pos = {200.0f, 250.0f};
  vec2 size = {400.0f, 200.0f};
  vec4 color = {1.0f, 1.0f, 1.0f, 0.1f};
  Renderer_RenderObject(highscores->renderer, pos, size, color);
  pos[0] += 5.0f;
  pos[1] += 5.0f;
  size[0] -= 10.0f;
  size[1] -= 10.0f;
  color[0] = 1.0f;
  color[1] = 0.2f;
  color[2] = 0.0f;
  Renderer_RenderObject(highscores->renderer, pos, size, color);

  vec2 text_pos;
  static const char *title = "HIGH SCORES";
  text_pos[0] = 400.0f - strlen(title) * 8.0f * 8.0f / 2.0f;
  text_pos[1] = 100.0f;
  Shader_Use(highscores->title_shader);
  Shader_SetFloat(highscores->title_shader, "time", (1.0f + sin(highscores->counter += 0.01f)) / 2.5f);
  TextRenderer_RenderString(highscores->title_renderer, title, text_pos, 8.0f);

  text_pos[0] = 220.0f;
  text_pos[1] = 270.0f;

  char str[16];
  text_pos[1] += 16.0f;
  for (u32 i = 0; i < NUM_LEVELS; ++i)
  {
    u32 seconds = highscores->scores_seconds[i];
    sprintf(str, "Level %d", i);
    text_pos[0] = 220.0f;
    text_pos[1] += 16.0f;
    TextRenderer_RenderString(highscores->text_renderer, str, text_pos, 2.0f);
    if (seconds > 0)
      sprintf(str, "%d", seconds);
    else
      sprintf(str, "%s", "-");
    text_pos[0] = 420.0f;
    TextRenderer_RenderString(highscores->text_renderer, str, text_pos, 2.0f);
  }
}

void Highscores_TryAdd(Highscores_t *highscores, u32 level, u32 score)
{
  if (level <= NUM_LEVELS && score > highscores->scores_seconds[level])
  {
    highscores->scores_seconds[level] = score;
    write_file(highscores);
  }
}

u32 *Highscores_GetHighscores(Highscores_t *highscores)
{
  return highscores->scores_seconds;
}