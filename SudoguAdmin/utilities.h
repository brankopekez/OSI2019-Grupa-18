#ifndef _utilities_h
#define _utilites_h

#include <Windows.h>
#include "cslib.h"

enum FormatOption {
	CENTER_ALIGN = 0x0001,
	HIGHLIGHT = 0x0010,
	MIDDLE = 0x0100
};

void ErrorExit(LPTSTR lpszFunction);

int clear(int startY, int endY);

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

int PrintToConsole(const string format, ...);

int PrintToConsoleFormatted(unsigned short options, const string format, ...);

void hideCursor(void);

void showCursor(void);

#endif // !_utilities_h
