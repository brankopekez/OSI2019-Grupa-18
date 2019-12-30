#include "Menu.h"
#include "cslib.h"
#include "vector.h"
#include <Windows.h>
#include "utilities.h"

// Extern variables.
extern HANDLE hStdout;
extern HANDLE hStdin;

struct MenuCDT {
	Vector options;
	int posX;
	int posY;
	int optionsPerLine;
	int spacingPerLine;
	bool canCancel;
	WORD highlightAttributes;
};

Menu newMenu(void) {
	Menu m = newBlock(Menu);
	m->options = newVector();
	m->posX = m->posY = 0;
	m->optionsPerLine = 1;
	m->spacingPerLine = 20;
	m->canCancel = FALSE;
	m->highlightAttributes = 0;
	return m;
}

void addMenu(Menu menu, string option) {
	addVector(menu->options, option);
}

Vector getMenuOptions(Menu m) {
	return m->options;
}

void setMenuOptions(Menu m, Vector v) {
	m->options = v;
}

void setPosY(Menu menu, int newPosY) {
	menu->posY = newPosY;
}

int getPosY(Menu menu) {
	return menu->posY;
}

void setPosX(Menu menu, int newPosX) {
	menu->posX = newPosX;
}

int getPosX(Menu menu) {
	return menu->posX;
}

int centerMenu(Menu menu) {
	const int optionsPerLine = getOptionsPerLine(menu);
	const int spacingPerLine = getSpacingPerLine(menu);
	const int totalMenu = getTotalMenu(menu);
	int startX, windowSizeX;
	int startY, windowSizeY;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD sizeConsole;

	// Get the screen buffer size. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return 0;
	}
	sizeConsole = csbi.dwSize;
	windowSizeX = sizeConsole.X;
	windowSizeY = sizeConsole.Y;

	startX = (windowSizeX - optionsPerLine * spacingPerLine) / 2;
	startY = (windowSizeY - totalMenu) / 2;

	setPosX(menu, startX);
	setPosY(menu, startY);
	return 1;
}

string getMenu(Menu menu, int index) {
	return getVector(menu->options, index);
}

void setMenu(Menu menu, string option, int index) {
	setVector(menu->options, index, option);
}

void setOptionsPerLine(Menu menu, int count) {
	menu->optionsPerLine = count;
}

int getOptionsPerLine(Menu menu) {
	return menu->optionsPerLine;
}

void setSpacingPerLine(Menu menu, int spacing) {
	menu->spacingPerLine = spacing;
}

int getSpacingPerLine(Menu menu) {
	return menu->spacingPerLine;
}

int getTotalMenu(Menu menu) {
	return sizeVector(menu->options);
}

void setCancelMenu(Menu menu, bool status) {
	menu->canCancel = status;
}

bool canCancelMenu(Menu menu) {
	return menu->canCancel;
}

WORD getHighlightAttributes(Menu menu) {
	return menu->highlightAttributes;
}

void setHighlightAttributes(Menu menu, WORD attributes) {
	menu->highlightAttributes = attributes;
}

int drawMenu(Menu menu, int currentSelection) {
	const int optionsPerLine = getOptionsPerLine(menu);
	const int spacingPerLine = getSpacingPerLine(menu);
	const int totalMenu = getTotalMenu(menu);
	const int startX = getPosX(menu);
	const int startY = getPosY(menu);
	COORD here;
	for (int i = 0; i < totalMenu; i++) {
		here.X = startX + (i % optionsPerLine) * spacingPerLine;
		here.Y = startY + i / optionsPerLine;

		SetConsoleCursorPosition(hStdout, here);

		string buffer = getMenu(menu, i);

		if (i == currentSelection) {
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			WORD wOldColor;

			// Save the current text colors. 
			if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
				return 0;
			}

			wOldColor = csbi.wAttributes;

			// Set the text attributes. 
			if (!SetConsoleTextAttribute(hStdout, getHighlightAttributes(menu))) {
				return 0;
			}


			DWORD cWritten;
			if (!WriteFile(
				hStdout,               // output handle 
				buffer,                // string buffer 
				lstrlenA(buffer),      // string length 
				&cWritten,             // bytes written 
				NULL))                 // not overlapped 
			{
				return 0;
			}

			// Restore the original text colors. 
			SetConsoleTextAttribute(hStdout, wOldColor);
		}
		else {
			DWORD cWritten;
			if (!WriteFile(
				hStdout,               // output handle 
				buffer,                // string buffer 
				lstrlenA(buffer),      // string length 
				&cWritten,             // bytes written 
				NULL))                 // not overlapped 
			{
				return 0;
			}
		}
	}
	return 1;
}

int mainMenu(Menu menu, int* selection) {
	// Hide the cursor inside the menu.
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &info);

	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode)) {
		// Show the cursor.
		info.bVisible = TRUE;
		SetConsoleCursorInfo(hStdout, &info);
		return 0;
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode)) {
		// Show the cursor.
		info.bVisible = TRUE;
		SetConsoleCursorInfo(hStdout, &info);
		return 0;
	}

	// Variable for registering end.
	BOOL done = FALSE;

	// Keeping track of the current option selection inside the menu.
	int currentSelection = 0;

	// Total number of options inside the menu.
	int totalOptions = getTotalMenu(menu);

	// Can cancel the menu?
	bool canCancel = canCancelMenu(menu);

	// Number of options per one line;
	int optionsPerLine = getOptionsPerLine(menu);

	// Starting line
	int startY = getPosY(menu);

	// Finding size info.
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		// Restore the original console mode. 
		SetConsoleMode(hStdin, fdwOldMode);

		// Show the cursor.
		info.bVisible = TRUE;
		SetConsoleCursorInfo(hStdout, &info);
		return 0;
	}
	// End line for clearing.
	int endY = csbi.dwSize.Y - 1;

	// Registered event that has happend.
	INPUT_RECORD event;

	// Number of read characters.
	DWORD cRead;

	// Loop for showing the menu, until done.
	while (!done) {
		if (!clear(startY, endY)) {
			return 0;
		}
		if (!drawMenu(menu, currentSelection)) {
			return 0;
		}
		system("pause>nul");
		if (WaitForSingleObject(hStdin, INFINITE) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &event, 1, &cRead);

			/* Only respond to key release events */
			if ((event.EventType == KEY_EVENT) && !event.Event.KeyEvent.bKeyDown)
				switch (event.Event.KeyEvent.wVirtualKeyCode) {
				case VK_LEFT:
					if (currentSelection % optionsPerLine > 0)
						currentSelection--;
					break;
				case VK_RIGHT:
					if (currentSelection % optionsPerLine < optionsPerLine - 1)
						currentSelection++;
					break;
				case VK_UP:
					if (currentSelection >= optionsPerLine)
						currentSelection -= optionsPerLine;
					break;
				case VK_DOWN:
					if (currentSelection + optionsPerLine < totalOptions)
						currentSelection += optionsPerLine;
					break;
				case VK_ESCAPE:
					if (canCancel) {
						currentSelection = -1;
						done = TRUE;
					}
					break;
				case VK_RETURN:
					done = TRUE;
					break;
				default:
					break;
				}
		}
	}

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	// Show the cursor.
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hStdout, &info);

	*selection = currentSelection;
	return 1;
}

// Private functions
