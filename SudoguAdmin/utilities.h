#ifndef _utilities_h
#define _utilites_h

#include <Windows.h>
#include "cslib.h"
#include "vector.h"
#include "Event.h"
#include "EventCategory.h"
#include <io.h>

/**
 * @enum	F_COLOR
 *
 * @brief	Values that represent foreground colors.
 */

enum F_COLOR {
	F_BLACK = 0,
	F_DARKBLUE = FOREGROUND_BLUE,
	F_DARKGREEN = FOREGROUND_GREEN,
	F_DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	F_DARKRED = FOREGROUND_RED,
	F_DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	F_DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	F_DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	F_GRAY = FOREGROUND_INTENSITY,
	F_BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	F_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	F_CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	F_RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	F_MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	F_YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	F_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};

/**
 * @enum	B_COLOR
 *
 * @brief	Values that represent background colors.
 */

enum B_COLOR {
	B_BLACK = 0,
	B_DARKBLUE = BACKGROUND_BLUE,
	B_DARKGREEN = BACKGROUND_GREEN,
	B_DARKCYAN = BACKGROUND_GREEN | BACKGROUND_BLUE,
	B_DARKRED = BACKGROUND_RED,
	B_DARKMAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
	B_DARKYELLOW = BACKGROUND_RED | BACKGROUND_GREEN,
	B_DARKGRAY = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	B_GRAY = BACKGROUND_INTENSITY,
	B_BLUE = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	B_GREEN = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	B_CYAN = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	B_RED = BACKGROUND_INTENSITY | BACKGROUND_RED,
	B_MAGENTA = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	B_YELLOW = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	B_WHITE = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
};

enum FormatOption {
	CENTER_ALIGN = 0x0001,
	HIGHLIGHT = 0x0010,
	MIDDLE = 0x0100
};

void ErrorExit(LPSTR lpszFunction);

void error_msg(const string format, ...);

int clear(int startY, int endY);

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

int PrintToConsole(const string format, ...);

int PrintToConsoleFormatted(unsigned short options, const string format, ...);

void hideCursor(void);

void showCursor(void);

void QuickSortVector(Vector vector, int begin, int end, CompareFn compareFn);

void advanceCursor(int count);

char* ReadString(FILE* inf);

Event ReadEvent(FILE* filepoint);

bool fileExists(string path);

Vector ReadEventsFromFile(string fileName);

size_t WriteStringToFile(FILE* filepoint, string outString);

void WriteEventToFile(FILE* filepoint, Event e);

void SaveEventsToFile(Vector events, string fileName);

EventCategory ReadCategory(FILE* filepoint);

Vector ReadCategoriesFromFile(string fileName);

void WriteCategoryToFile(FILE* filepoint, EventCategory cat);

void SaveCategoriesToFile(Vector categories, string fileName);

void clearCordinates(int startX, int startY, int height, int width);

void cls(HANDLE hConsole);

Vector EventToVector(Event event);

void FreeEventStringVector(Vector vector);

Vector EventCategoryToVector(EventCategory category);

void FreeEventCategoryStringVector(Vector vector);

void PrintTitle(const string title);

void PrintStatusLine(const string format);

void SetCursorPositionMiddle();

string ShowPrompt(const string title, const string footer, const string format, ...);

int YesNoPrompt(const string title, const string format, ...);

CHAR_INFO* SaveScreenBuffer(void);

int RecoverScreenBuffer(CHAR_INFO* chiBuffer);

void SetConsoleWindowSize(int x, int y);

#endif // !_utilities_h
