#ifndef _menu_h
#define _menu_h

#include "cslib.h"
#include "vector.h"
#include <Windows.h>

typedef struct MenuCDT* Menu;

Menu newMenu(void);

void addMenu(Menu m, string option);

Vector getMenuOptions(Menu m);

void setMenuOptions(Menu m, Vector v);

void setPosY(Menu menu, int newPosY);

int getPosY(Menu menu);

void setPosX(Menu menu, int newPosX);

int getPosX(Menu menu);

int centerMenu(Menu menu);

string getMenu(Menu menu, int index);

void setMenu(Menu menu, string option, int index);

void setOptionsPerLine(Menu menu, int count);

int getOptionsPerLine(Menu menu);

void setSpacingPerLine(Menu menu, int spacing);

int getSpacingPerLine(Menu menu);

int getTotalMenu(Menu menu);

void setCancelMenu(Menu menu, bool status);

bool canCancelMenu(Menu menu);

WORD getHighlightAttributes(Menu menu);

void setHighlightAttributes(Menu menu, WORD attributes);

int drawMenu(Menu menu, int currentSelection);

int mainMenu(Menu menu, int* selection);

#endif // !_menu_h
