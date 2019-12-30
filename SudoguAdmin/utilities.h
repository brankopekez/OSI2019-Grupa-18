#ifndef _utilities_h
#define _utilites_h

#include <Windows.h>
#include "cslib.h"

void ErrorExit(LPTSTR lpszFunction);

int clear(int startY, int endY);

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

int PrintToConsole(const string format, ...);

#endif // !_utilities_h
