#ifndef _MENU_H_
#define _MENU_H_

typedef struct Menu_t Menu_t;

typedef enum Menu_Item_t
{
  MENU_ITEM_LEVEL_SELECT,
  MENU_ITEM_QUIT,
  MENU_ITEM_LAST
} Menu_Item_t;

Menu_t *Menu_Init(void);
void Menu_Up(Menu_t *menu);
void Menu_Down(Menu_t *menu);
void Menu_Render(Menu_t *menu);
Menu_Item_t Menu_Get(Menu_t *menu);

#endif /* _MENU_H_ */
