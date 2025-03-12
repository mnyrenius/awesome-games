#include <stdlib.h>
#include "level_selection.h"
#include "renderer.h"
#include "text_renderer.h"
#include "shader.h"
#include "math.h"
#include "util.h"
#include "build_info.h"
#include "highscores.h"


typedef struct LevelSelection_t
{
  i32 current_level;
  Renderer_t *renderer;
  TextRenderer_t *text_renderer;
  TextRenderer_t *title_renderer;
  Shader_t *title_shader;
  float counter;
  float size;
  Highscores_t *highscores;
} LevelSelection_t;

static i32 get_view_start(i32 current)
{
  i32 start = 0;
  if (current > 4)
  {
    start = current - 4;
  }

  return start;
}

LevelSelection_t *LevelSelection_Init(Highscores_t *highscores)
{
  LevelSelection_t *level_selection = malloc(sizeof(LevelSelection_t));

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  level_selection->current_level = 0;
  level_selection->renderer = Renderer_Init(vertices, sizeof(vertices));
  level_selection->text_renderer = TextRenderer_Init();

  level_selection->title_shader = Shader_Init();
  level_selection->counter = 0.0f;
  const char vs[] =
      {
#include "shaders/title.vs.data"
          , 0};

  const char fs[] =
      {
#include "shaders/title.fs.data"
          , 0};
  Shader_Load(level_selection->title_shader, vs, fs);
  level_selection->title_renderer = TextRenderer_Init_With_Shader(level_selection->title_shader);
  level_selection->highscores = highscores;

  return level_selection;
}

void LevelSelection_Up(LevelSelection_t *level_selection)
{
  i32 next = level_selection->current_level - 1;
  if (next >= 0)
  {
    level_selection->current_level = next;
  }
}

void LevelSelection_Down(LevelSelection_t *level_selection)
{
  i32 next = level_selection->current_level + 1;
  if (next < NUM_LEVELS && Highscores_GetHighscores(level_selection->highscores)[next - 1] != -1.0f)
  {
    level_selection->current_level = next;
  }
}

void LevelSelection_Render(LevelSelection_t *level_selection)
{
  vec2 pos = {200.0f, 250.0f};
  vec2 size = {400.0f, 300.0f};
  vec4 color = {1.0f, 1.0f, 1.0f, 0.2f};
  Renderer_RenderObject(level_selection->renderer, pos, size, color);
  pos[0] += 5.0f;
  pos[1] += 5.0f;
  size[0] -= 10.0f;
  size[1] -= 10.0f;
  color[0] = 1.0f;
  color[1] = 0.2f;
  color[2] = 0.0f;
  Renderer_RenderObject(level_selection->renderer, pos, size, color);

  vec2 text_pos;
  const char *title = "HOPPSPEL";
  text_pos[0] = 400.0f - strlen(title) * 8.0f * 8.0f / 2.0f;
  text_pos[1] = 100.0f;
  Shader_Use(level_selection->title_shader);
  Shader_SetFloat(level_selection->title_shader, "time", (1.0f + sin(level_selection->counter += 0.01f)) / 2.5f);
  TextRenderer_RenderString(level_selection->title_renderer, title, text_pos, 8.0f);

  text_pos[1] = 300.0f;

  text_pos[0] = 300.0f - (strlen("Level") * 8.0f);
  TextRenderer_RenderString(level_selection->text_renderer, "Level", text_pos, 2.0f);
  text_pos[0] += 150.0f;
  TextRenderer_RenderString(level_selection->text_renderer, "Best time", text_pos, 2.0f);
  text_pos[0] = 300.0f - (strlen("Level") * 8.0f);
  text_pos[1] = 330.0f;
  TextRenderer_RenderString(level_selection->text_renderer, "-------------------", text_pos, 2.0f);

  text_pos[1] = 360.0f;
  char level_str[16];
  char time_str[16];
  f32 *highscores = Highscores_GetHighscores(level_selection->highscores);
  i32 view_start = get_view_start(level_selection->current_level);
  for (i32 i = view_start; i < (view_start + 5); ++i)
  {
    text_pos[0] = 300.0f - 8.0f;

    if (i == level_selection->current_level)
    {
      Renderer_RenderObject(level_selection->renderer, (vec2){text_pos[0] - 20.0f, text_pos[1] + 4.0f}, (vec2){8.0f, 8.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
    }

    vec3 text_color;
    f32 best_time = highscores[i];
    if (best_time != -1.0f)
    {
      u32 mins = best_time / 60.0f;
      f32 secs = fmod(best_time, 60);
      u32 hundreds = (secs - floor(secs)) * 100;
      sprintf(time_str, "%02u:%02u:%02u", mins, (u32)secs, hundreds);
      text_color[0] = 1.0f;
      text_color[1] = 1.0f;
      text_color[2] = 1.0f;
    }
    else
    {
      strcpy(time_str, "NOT FINISHED");
      text_color[0] = 0.5f;
      text_color[1] = 0.5f;
      text_color[2] = 0.5f;
    }

    sprintf(level_str, "%d", i + 1);
    TextRenderer_RenderString_WithColor(level_selection->text_renderer, level_str, text_pos, 2.0f, text_color);
    text_pos[0] += 70.0f;
    TextRenderer_RenderString_WithColor(level_selection->text_renderer, time_str, text_pos, 2.0f, text_color);
    text_pos[1] += 30.0f;
  }

  const char *author = "Hagaberg Game Studios 2024";
  text_pos[0] = 400.0f - (strlen(author) * 8.0f / 1.5f);
  text_pos[1] = 570.0f;
  TextRenderer_RenderString(level_selection->text_renderer, author, text_pos, 1.5f);

  text_pos[0] = 0.0f;
  text_pos[1] = 0.0f;
  TextRenderer_RenderString(level_selection->text_renderer, HOPPSPEL_BUILD, text_pos, 1.0f);
}

u32 LevelSelection_Get(LevelSelection_t *level_selection)
{
  return level_selection->current_level;
}
