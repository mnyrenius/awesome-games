#include <stdlib.h>
#include "menu.h"
#include "renderer.h"
#include "text_renderer.h"
#include "shader.h"
#include "math.h"
#include "util.h"
#include "build_info.h"

typedef struct Menu_t
{
  Menu_Item_t item;
  Renderer_t *renderer;
  TextRenderer_t *text_renderer;
  TextRenderer_t *title_renderer;
  Shader_t *title_shader;
  float counter;
  float size;
} Menu_t;

Menu_t *Menu_Init(void)
{
  Menu_t *menu = malloc(sizeof(Menu_t));

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  menu->item = MENU_ITEM_PLAY;
  menu->renderer = Renderer_Init(vertices, sizeof(vertices));
  menu->text_renderer = TextRenderer_Init();

  menu->title_shader = Shader_Init();
  menu->counter = 0.0f;
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
  Shader_Load(menu->title_shader, vs, fs);
  menu->title_renderer = TextRenderer_Init_With_Shader(menu->title_shader);

  return menu;
}

void Menu_Up(Menu_t *menu)
{
  if (menu->item == MENU_ITEM_PLAY)
  {
    menu->item = MENU_ITEM_QUIT;
  }
  else
  {
    menu->item--;
  }
}

void Menu_Down(Menu_t *menu)
{
  if (menu->item == MENU_ITEM_QUIT)
  {
    menu->item = MENU_ITEM_PLAY;
  }
  else
  {
    menu->item++;
  }
}

void Menu_Render(Menu_t *menu)
{
  vec2 pos = { 200.0f, 250.0f };
  vec2 size = { 400.0f, 200.0f };
  vec4 color = { 1.0f, 1.0f, 1.0f, 0.1f };
  Renderer_RenderObject(menu->renderer, pos, size, color);
  pos[0] += 5;
  pos[1] += 5;
  size[0] -= 10;
  size[1] -= 10;
  color[0] = 1.0f;
  color[1] = 0.2f;
  color[2] = 0.0f;
  Renderer_RenderObject(menu->renderer, pos, size, color);

  vec2 text_pos;
  const char *title = "ACTION SNAKE";
  text_pos[0] = 400.0f - strlen(title) * 8 * 8 / 2;
  text_pos[1] = 100.0f;
  Shader_Use(menu->title_shader);
  Shader_SetFloat(menu->title_shader, "time", (1.0f + sin(menu->counter += 0.01f))/2.5f);
  TextRenderer_RenderString(menu->title_renderer, title, text_pos, 8.0f);

  const char *play = menu->item == MENU_ITEM_PLAY ?             "[PLAY GAME]" :  "PLAY GAME";
  const char *highscores = menu->item == MENU_ITEM_HIGHSCORES ? "[HIGH SCORES]" : "HIGH SCORES";
  const char *quit = menu->item == MENU_ITEM_QUIT ?             "[QUIT]" :    "QUIT";
  text_pos[0] = 400.0f - (strlen(play) * 8);
  text_pos[1] = 310.0f;
  TextRenderer_RenderString(menu->text_renderer, play, text_pos, 2.0f);
  text_pos[0] = 400.0f - (strlen(highscores) * 8);
  text_pos[1] += 30.0f;
  TextRenderer_RenderString(menu->text_renderer, highscores, text_pos, 2.0f);
  text_pos[0] = 400.0f - (strlen(quit) * 8);
  text_pos[1] += 30.0f;
  TextRenderer_RenderString(menu->text_renderer, quit, text_pos, 2.0f);

  const char *author = "Hagaberg Game Studios 2023";
  text_pos[0] = 400.0f - (strlen(author) * 8.0f / 1.5f);
  text_pos[1] = 570.0f;
  TextRenderer_RenderString(menu->text_renderer, author, text_pos, 1.5f);

  text_pos[0] = 0.0f;
  text_pos[1] = 0.0f;
  TextRenderer_RenderString(menu->text_renderer, ASNAKE_BUILD, text_pos, 1.0f);
}

Menu_Item_t Menu_Get(Menu_t *menu)
{
  return menu->item;
}
