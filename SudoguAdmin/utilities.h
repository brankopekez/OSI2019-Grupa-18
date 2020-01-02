#ifndef _utilities_h
#define _utilites_h

#include <Windows.h>
#include "cslib.h"
#include "vector.h"

enum FormatOption {
	CENTER_ALIGN = 0x0001,
	HIGHLIGHT = 0x0010,
	MIDDLE = 0x0100
};

void ErrorExit(LPSTR lpszFunction);

int clear(int startY, int endY);

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

int PrintToConsole(const string format, ...);

int PrintToConsoleFormatted(unsigned short options, const string format, ...);

void hideCursor(void);

void showCursor(void);

void QuickSortVector(Vector vector, int begin, int end, CompareFn compareFn);

#endif // !_utilities_h
