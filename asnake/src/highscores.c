#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include "highscores.h"
#include "renderer.h"
#include "text_renderer.h"
#include "shader.h"
#include "util.h"

#define NAME_LENGTH 8
#define NUM_HIGHSCORES 8
#define HIGHSCORE_FILE "asnake.dat"

#ifdef __linux__
#define HOME_ENVVAR "HOME"
#elif _WIN32
#define HOME_ENVVAR "USERPROFILE"
#endif

typedef struct Highscores_Entry_t
{
  char name[NAME_LENGTH];
  int score;
} Highscores_Entry_t;

typedef struct Highscores_t
{
  Renderer_t *renderer;
  TextRenderer_t *text_renderer;
  TextRenderer_t *title_renderer;
  Shader_t *title_shader;
  Highscores_Entry_t scores[NUM_HIGHSCORES];
  float counter;
  Highscores_Entry_t new;
  Highscores_Mode_t mode;
} Highscores_t;

static void mode_view(Highscores_t *highscores)
{
  vec2 text_pos = { 220.0f, 270.0f };

  char str[16];
  TextRenderer_RenderString(highscores->text_renderer, "NAME", text_pos, 2.0f);
  text_pos[0] = 420.0f;
  TextRenderer_RenderString(highscores->text_renderer, "SCORE", text_pos, 2.0f);

  text_pos[1] += 16.0f;
  for (int i = 0; i < NUM_HIGHSCORES; ++i)
  {
    Highscores_Entry_t *entry = &highscores->scores[i];
    sprintf(str, "%d. %s", i + 1, entry->name);
    text_pos[0] = 220.0f;
    text_pos[1] += 16.0f;
    TextRenderer_RenderString(highscores->text_renderer, str, text_pos, 2.0f);
    if (entry->score > 0)
      sprintf(str, "%d", entry->score);
    else
      sprintf(str, "%s", "-");
    text_pos[0] = 420.0f;
    TextRenderer_RenderString(highscores->text_renderer, str, text_pos, 2.0f);
  }

}

static void mode_add(Highscores_t *highscores)
{
  vec2 text_pos = { 400.0f - 104.0f, 300.0f };
  char str[16];
  char name[NAME_LENGTH];
  strcpy(name, highscores->new.name);
  memset(name+strlen(highscores->new.name), '-', NAME_LENGTH - strlen(highscores->new.name - 1));
  sprintf(str, "NAME: %s", name);
  TextRenderer_RenderString(highscores->text_renderer, str, text_pos, 2.0f);
}

static int calc_checksum(Highscores_t *highscores)
{
  int checksum = 0;
  char *p = (char *)&highscores->scores;
  for (int i = 0; i < sizeof(highscores->scores); ++i)
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

  int checksum = calc_checksum(highscores);

  fwrite(&checksum, sizeof(int), 1, fp);
  fwrite(highscores->scores, sizeof(Highscores_Entry_t), NUM_HIGHSCORES, fp);
  fclose(fp);
}

Highscores_t *Highscores_Init(void)
{
  Highscores_t *highscores = malloc(sizeof(Highscores_t));

  float vertices[] = {
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
    #include "shaders/title.vs.data"
    ,0
  };

  const char fs[] =
  {
    #include "shaders/title.fs.data"
    ,0
  };
  Shader_Load(highscores->title_shader, vs, fs);
  highscores->title_renderer = TextRenderer_Init_With_Shader(highscores->title_shader);


  highscores->text_renderer = TextRenderer_Init();

  FILE *fp = open_file("rb");
  bool read_success = false;
  if (fp)
  {
    int checksum = -1;
    size_t checksum_read = fread(&checksum, sizeof(int), 1, fp);
    size_t hs_read = fread(highscores->scores, sizeof(Highscores_Entry_t), NUM_HIGHSCORES, fp);
    if (checksum_read == 1 &&
        hs_read == NUM_HIGHSCORES &&
        checksum == calc_checksum(highscores))
    {
      read_success = true;
      LOG("%s\n", "Read highscore file ok");
    }
  }

  if (!read_success)
  {
    strcpy(highscores->scores[0].name, "JOE");
    highscores->scores[0].score = 100000;

    strcpy(highscores->scores[1].name, "MIKE");
    highscores->scores[1].score = 10000;

    strcpy(highscores->scores[2].name, "SVEN");
    highscores->scores[2].score = 1000;

    for (int i = 3; i < NUM_HIGHSCORES; ++i)
    {
      strcpy(highscores->scores[i].name, "-------");
      highscores->scores[i].score = 0;
    }
  }

  highscores->counter = 0.0f;
  highscores->mode = HIGHSCORES_MODE_VIEW;

  return highscores;
}

void Highscores_Render(Highscores_t *highscores)
{
  vec2 pos = {200.0f, 250.0f};
  vec2 size = { 400.0f, 200.0f };
  vec4 color = { 1.0f, 1.0f, 1.0f, 0.1f };
  Renderer_RenderObject(highscores->renderer, pos, size, color);
  pos[0] += 5;
  pos[1] += 5;
  size[0] -= 10;
  size[1] -= 10;
  color[0] = 1.0f;
  color[1] = 0.2f;
  color[2] = 0.0f;
  Renderer_RenderObject(highscores->renderer, pos, size, color);

  vec2 text_pos;
  static const char *title = "HIGH SCORES";
  text_pos[0] = 400.0f - strlen(title) * 8 * 8 / 2;
  text_pos[1] = 100.0f;
  Shader_Use(highscores->title_shader);
  Shader_SetFloat(highscores->title_shader, "time", (1.0f + sin(highscores->counter += 0.01f))/2.5f);
  TextRenderer_RenderString(highscores->title_renderer, title, text_pos, 8.0f);

  switch (highscores->mode)
  {
  case HIGHSCORES_MODE_VIEW:
    mode_view(highscores);
    break;

  case HIGHSCORES_MODE_ADD:
    mode_add(highscores);
    break;

  default:
    break;
  }
}

bool Highscores_CheckScore(Highscores_t *highscores, int score)
{
  for (int i = 0; i < NUM_HIGHSCORES; ++i)
  {
    if (score > highscores->scores[i].score)
    {
      return true;
    }
  }

  return false;
}

void Highscores_Add(Highscores_t *highscores, int score)
{
  highscores->mode = HIGHSCORES_MODE_ADD;
  highscores->new.score = score;
  strcpy(highscores->new.name, "");
}

Highscores_Mode_t Highscores_GetMode(Highscores_t *highscores)
{
  return highscores->mode;
}
void Highscores_EnterKey(Highscores_t *highscores, int key)
{
  if (highscores->mode != HIGHSCORES_MODE_ADD)
  {
    return;
  }

  if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
  {
    if (strlen(highscores->new.name) < NAME_LENGTH - 1)
    {
      char n[] = { key, '\0' };
      strcat(highscores->new.name, n);
    }
  }

  else if (key == GLFW_KEY_ENTER)
  {
    for (int i = 0; i < NUM_HIGHSCORES; ++i)
    {
      if (highscores->new.score > highscores->scores[i].score)
      {
        memcpy(&highscores->scores[i + 1], &highscores->scores[i], sizeof(Highscores_Entry_t) * (NUM_HIGHSCORES - i - 1));
        strcpy(highscores->scores[i].name, highscores->new.name);
        highscores->scores[i].score = highscores->new.score;
        break;
      }
    }

    write_file(highscores);
    highscores->mode = HIGHSCORES_MODE_VIEW;
  }

  else if (key == GLFW_KEY_BACKSPACE)
  {
    int last = strlen(highscores->new.name);
    if (last > 0)
    {
      highscores->new.name[last - 1] = '\0';
    }
  }
}

