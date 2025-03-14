#include <stdlib.h>
#include "menu.h"
#include "renderer.h"
#include "text_renderer.h"
#include "shader.h"
#include "math.h"
#include "util.h"
#include "build_info.h"

#define MENU_ITEM_STR_LEN 12
#define NUM_ITEMS 2
typedef struct Menu_Item_Int_t
{
  Menu_Item_t item;
  char str[MENU_ITEM_STR_LEN];
} Menu_Item_Int_t;

typedef struct Menu_t
{
  Menu_Item_Int_t items[NUM_ITEMS];
  i32 current_item;
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

  menu->items[0].item = MENU_ITEM_LEVEL_SELECT;
  strcpy(menu->items[0].str, "LEVEL SELECT");
  menu->items[1].item = MENU_ITEM_QUIT;
  strcpy(menu->items[1].str, "QUIT");

  menu->current_item = 0;

  menu->renderer = Renderer_Init(vertices, sizeof(vertices));
  menu->text_renderer = TextRenderer_Init();

  menu->title_shader = Shader_Init();
  menu->counter = 0.0f;
  const char vs[] =
      {
#include "shaders/title.vs.data"
          , 0};

  const char fs[] =
      {
#include "shaders/title.fs.data"
          , 0};
  Shader_Load(menu->title_shader, vs, fs);
  menu->title_renderer = TextRenderer_Init_With_Shader(menu->title_shader);

  return menu;
}

void Menu_Up(Menu_t *menu)
{
  menu->current_item = (menu->current_item - 1 + NUM_ITEMS) % NUM_ITEMS;
}

void Menu_Down(Menu_t *menu)
{
  menu->current_item = (menu->current_item + 1 + NUM_ITEMS) % NUM_ITEMS;
}

void Menu_Render(Menu_t *menu)
{
  vec2 pos = {200.0f, 250.0f};
  vec2 size = {400.0f, 200.0f};
  vec4 color = {1.0f, 1.0f, 1.0f, 0.2f};
  Renderer_RenderObject(menu->renderer, pos, size, color);
  pos[0] += 5.0f;
  pos[1] += 5.0f;
  size[0] -= 10.0f;
  size[1] -= 10.0f;
  color[0] = 1.0f;
  color[1] = 0.2f;
  color[2] = 0.0f;
  Renderer_RenderObject(menu->renderer, pos, size, color);

  vec2 text_pos;
  const char *title = "HOPPSPEL";
  text_pos[0] = 400.0f - strlen(title) * 8.0f * 8.0f / 2.0f;
  text_pos[1] = 100.0f;
  Shader_Use(menu->title_shader);
  Shader_SetFloat(menu->title_shader, "time", (1.0f + sin(menu->counter += 0.01f)) / 2.5f);
  TextRenderer_RenderString(menu->title_renderer, title, text_pos, 8.0f);

  text_pos[1] = 310.0f;
  for (u32 i = 0; i < NUM_ITEMS; ++i)
  {
    text_pos[0] = 400.0f - (strlen(menu->items[i].str) * 8.0f);

    if (i == menu->current_item)
    {
      Renderer_RenderObject(menu->renderer, (vec2){text_pos[0] - 20.0f, text_pos[1] + 4.0f}, (vec2){8.0f, 8.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f});
    }

    TextRenderer_RenderString(menu->text_renderer, menu->items[i].str, text_pos, 2.0f);
    text_pos[1] += 30.0f;
  }

  const char *author = "Hagaberg Game Studios 2025";
  text_pos[0] = 400.0f - (strlen(author) * 8.0f / 1.5f);
  text_pos[1] = 570.0f;
  TextRenderer_RenderString(menu->text_renderer, author, text_pos, 1.5f);

  text_pos[0] = 0.0f;
  text_pos[1] = 0.0f;
  TextRenderer_RenderString(menu->text_renderer, HOPPSPEL_BUILD, text_pos, 1.0f);
}

Menu_Item_t Menu_Get(Menu_t *menu)
{
  return menu->items[menu->current_item].item;
}
