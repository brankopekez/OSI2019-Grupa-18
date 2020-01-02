#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "cslib.h"
#include "map.h"
#include "iterator.h"
#include "simpio.h"
#include "cmpfn.h"
#include "strlib.h"
#include "strbuf.h"
#include "generic.h"
#include "EventCategory.h"
#include "Event.h"
#include "vector.h"
#include <time.h>
#include <locale.h>
#include "Menu.h"
#include "utilities.h"
#include "Table.h"

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

string logo[6] = {
	"   _____           __                 ",
	"  / ___/__  ______/ /___  ____ ___  __",
	"  \\__ \\/ / / / __  / __ \\/ __ `/ / / /",
	" ___/ / /_/ / /_/ / /_/ / /_/ / /_/ / ",
	"/____/\\__,_/\\__,_/\\____/\\__, /\\__,_/  ",
	"                       /____/         "
};

/** @brief	The array of menu options. */
string menuOptions[] = {
	" Upravljanje događajima ",
	" Upravljanje kategorijama ",
	" Odjava ",
	" Izlaz "
};

enum M_MENU {
	EVENT_HANDLING,
	CATEGORY_HANDLING,
	LOGOUT,
	EXIT
};

/** @brief	The events table header (column names). */
string eventsHeader[] = {
	"Naziv",
	"Lokacija",
	"Kategorija",
	"Datum i vrijeme"
};

/** @brief	The events table footer. */
string eventsFooter[] = {
	"ESC: Izlaz.",
	"RETURN: Detalji.",
	"DELETE: Obriši.",
	"F9: Novi događaj.",
	"F10: Sortiraj listu."
};

string categoriesHeader[] = {
	"Naziv kategorije događaja"
};

string categoriesFooter[] = {
	"ESC: Izlaz.",
	"DELETE: Obriši kategoriju.",
	"F9: Dodaj novu kategoriju.",
};

enum EVENTS_HEADER_OPTIONS {
	EVENTS_HEADER_NAME,
	EVENTS_HEADER_LOCATION,
	EVENTS_HEADER_CATEGORY,
	EVENTS_HEADER_TIME
};

string eventFields[] = {
	"Naziv",
	"Lokacija",
	"Kategorija",
	"Datum i vrijeme",
	"Opis"
};

enum EDIT_EVENT_MENU {
	EDIT_EVENT_NAME,
	EDIT_EVENT_LOCATION,
	EDIT_EVENT_CATEGORY,
	EDIT_EVENT_TIME,
	EDIT_EVENT_DESCRIPTION
};

/** @brief	Global variable for accounts config filename */
const string fileAccounts = "accounts.txt";

/** @brief	The city config filename  */
const string fileCity = "city.txt";

/** @brief	Name of the program (used on error) */
const string programName = "SudoguAdmin";

/** @brief	The username */
string username = NULL;

/** @brief	Handle to the stdout */
HANDLE hStdout;

/** @brief	Handle to the stdin */
HANDLE hStdin;

/** @brief	Variable to save the original console mode */
DWORD fdwSaveOldMode;

/** @brief	The original color attributes */
WORD wOldColorAttrs;

/** @brief	Information describing the console screen sb */
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

WORD HIGHLIGHT_ATTRIBUTES = F_RED | B_WHITE | COMMON_LVB_REVERSE_VIDEO;

const int windowSizeX = 121;
const int windowSizeY = 33;

void error_msg(const string format, ...) {
	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);
	ErrorExit(buf);
}


/**
 * @fn	int fileToMap(string filename, Map map)
 *
 * @brief	Reads a file and maps key:value pairs.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	filename	Filename of the file.
 * @param 	map			The map.
 *
 * @returns	An int. 1 on success, 0 otherwise.
 */

int fileToMap(string filename, Map map) {
	FILE* inFile = fopen(filename, "r");
	if (!inFile) {
		error_msg("fopen()");
	}
	string line;
	while ((line = readLine(inFile)) != NULL) {
		int delimPos = findChar(':', line, 0);

		string key = substring(line, 0, delimPos - 1);
		string value = substring(line, delimPos + 1, stringLength(line) - 1);
		if (stringLength(key) == 0 || stringLength(value) == 0) {
			return 0;
		}
		else {
			if (containsKeyMap(map, key) == true) {
				return 0;
			}
			putMap(map, key, value);
		}
		freeBlock(line);
	}
	fclose(inFile);
	return 1;
}

/**
 * @fn	void freeMapFields(Map Map)
 *
 * @brief	Frees accounts map and all of its allocated fields.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	Map	The accounts map.
 */

void freeMapFields(Map Map) {
	string key;
	Iterator iterator = newIterator(Map);
	while (stepIterator(iterator, &key)) {
		freeBlock(getMap(Map, key));
	}
	freeIterator(iterator);
	iterator = newIterator(Map);
	while (stepIterator(iterator, &key)) {
		freeBlock(key);
	}
	freeIterator(iterator);

	freeMap(Map);
}

/**
 * @fn	StringBuffer readPassword(void)
 *
 * @brief	Reads the password from a stdin.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @returns	The StringBuffer containing a read password.
 */

StringBuffer readPassword(void) {
	StringBuffer password;
	password = newStringBuffer();
	int ch;
	int i = -1; // this variable counts input characters

	do {
		++i;
		ch = _getch();
		if (ch == 0x08) // backspace
		{
			if (i > 0) {
				_putch('\b');
				_putch(' ');
				_putch('\b');
				popChar(password);
				i -= 2;
			}
			else --i;
			continue;
		}
		if (ch == 0x0d) // Enter
		{
			_putch('\n');
			return password;
		}
		if (ch == 0x00) // Extended key pressed (F1, F2, etc.)
		{
			// extended keys have two parts ...
			_getch(); // ignore it
			--i;
			continue;
		}
		pushChar(password, ch);
		_putch('*');
	} while (ch != '\n');
}

void clearCordinates(int startX, int startY, int height, int width) {
	COORD here;
	DWORD cWritten;
	for (int i = 0; i < height; i++) {
		here.Y = startY + i;
		for (int j = 0; j < width; j++) {
			here.X = startX + j;
			WriteConsoleOutputCharacter(hStdout, L" ", 1, here, &cWritten);
		}
	}

}

void cls(HANDLE hConsole) {
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	COORD oldCordinates;

	// Get the number of character cells in the current buffer. 

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	oldCordinates = csbi.dwCursorPosition;

	// Fill the entire screen with blanks.

	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer 
		(TCHAR) ' ',     // Character to write to the buffer
		dwConSize,       // Number of cells to write 
		coordScreen,     // Coordinates of first cell 
		&cCharsWritten))// Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		return;
	}

	// Set the buffer's attributes accordingly.

	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer 
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute 
		coordScreen,      // Coordinates of first cell 
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.

	SetConsoleCursorPosition(hConsole, oldCordinates);
}

void advanceCursor(int count) {
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
		error_msg("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}
	cursorPosition = csbiInfo.dwCursorPosition;
	cursorPosition.Y += count;
	SetConsoleCursorPosition(hStdout, cursorPosition);
}

void PrintLogo(void) {
	// 38
	COORD cursorPosition = { 0, 4 };
	SetConsoleCursorPosition(hStdout, cursorPosition);
	HIGHLIGHT_ATTRIBUTES = F_RED | B_WHITE;
	for (int i = 0; i < 6; i++) {
		PrintToConsoleFormatted(CENTER_ALIGN | HIGHLIGHT, logo[i]);
		advanceCursor(1);
	}
	HIGHLIGHT_ATTRIBUTES = F_RED | B_WHITE | COMMON_LVB_REVERSE_VIDEO;
	//advanceCursor(1);
	//PrintToConsoleFormatted(CENTER_ALIGN, "Sistem upravljanja događajima");
}

void loginAttempt(string* inUsername, string* inPassword) {
	LPSTR prompt1 = "Username: ";
	LPSTR prompt2 = "Password: ";
	const int promptCount = 2;
	DWORD cWritten;
	COORD cursorPosition;

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD sizeConsole;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
		error_msg("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}

	sizeConsole = csbiInfo.dwSize;

	cursorPosition.X = (sizeConsole.X - lstrlenA(prompt1) * 2) / 2;
	cursorPosition.Y = (sizeConsole.Y - promptCount) / 2;

	//system("cls");
	clearCordinates(0, cursorPosition.Y, 3, sizeConsole.X);

	SetConsoleCursorPosition(hStdout, cursorPosition);

	cWritten = PrintToConsole(prompt1);

	*inUsername = readLine(stdin);

	++cursorPosition.Y;
	SetConsoleCursorPosition(hStdout, cursorPosition);

	PrintToConsole(prompt2);

	StringBuffer passwordBuffer = NULL;
	passwordBuffer = readPassword();
	*inPassword = copyString(getString(passwordBuffer));
	freeStringBuffer(passwordBuffer);
}

/**
 * @fn	int login(string* username)
 *
 * @brief	Logins a user.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param [in,out]	username	If non-null, the username.
 *
 * @returns	An int 1 if successful, 0 otherwise.
 */

void login(void) {
	Map accountsMap = NULL;
	string inUsername = NULL, inPassword = NULL;

	cls(hStdout);
	while (true) {
		if (accountsMap != NULL) {
			freeMapFields(accountsMap);
		}
		if (inUsername != NULL) {
			freeBlock(inUsername);
		}
		if (inPassword != NULL) {
			freeBlock(inPassword);
		}

		accountsMap = newMap();
		if (fileToMap(fileAccounts, accountsMap) == 0) {
			freeMapFields(accountsMap);
			error_msg("Konfiguracioni fajl %s nije ispravan.\n", fileAccounts);
		}

		PrintLogo();

		loginAttempt(&inUsername, &inPassword);

		cls(hStdout);
		advanceCursor(3);

		if (stringLength(inUsername) == 0 || stringLength(inPassword) == 0) {
			PrintToConsoleFormatted(CENTER_ALIGN, "Pogresan unos.");
			continue;
		}

		if (containsKeyMap(accountsMap, inUsername) == true) {
			string correctPassword = getMap(accountsMap, inUsername);
			if (stringCompare(inPassword, correctPassword) == 0) {
				username = inUsername;
				break;
			}
			else {
				PrintToConsoleFormatted(CENTER_ALIGN, "Pogresna lozinka.");
				continue;
			}
		}
		else {
			PrintToConsoleFormatted(CENTER_ALIGN, "Ne postoji nalog sa imenom '%s'.\n", inUsername);
			continue;
		}
	}

	if (accountsMap != NULL) {
		freeMapFields(accountsMap);
	}
	if (inUsername != NULL) {
		freeBlock(inUsername);
	}
	if (inPassword != NULL) {
		freeBlock(inPassword);
	}
}

/**
 * @fn	void logout(string username)
 *
 * @brief	Logout
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	username	The username.
 */

void logout(void) {
	freeBlock(username);
}

void SetConsoleWindowSize(int x, int y);

void windowSetup(void);

Vector EventToVector(Event event) {
	Vector vector = newVector();
	addVector(vector, getEventName(event));
	addVector(vector, getEventLocation(event));
	addVector(vector, getEventCategory(event));

	struct tm newtime;
	char buff[BUFSIZ];
	time_t long_time = getEventTime(event);
	errno_t err;

	// Convert to local time.
	err = localtime_s(&newtime, &long_time);
	if (err) {
		return NULL;
	}

	if (!strftime(buff, sizeof buff, "%A %x %R", &newtime)) {
		return NULL;
	}

	string tmp = copyString(buff);
	addVector(vector, tmp);

	return vector;
}

void FreeEventStringVector(Vector vector) {
	freeBlock(getVector(vector, 3)); //todo:magic num
	freeVector(vector);
}

Vector EventCategoryToVector(EventCategory category) {
	Vector vector = newVector();
	addVector(vector, getEventCategoryName(category));
	return vector;
}

void FreeEventCategoryStringVector(Vector vector) {
	freeVector(vector);
}

void PrintTitle(const string title) {
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 
	// 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return;
	}

	dwConSize = csbi.dwSize.X;

	// Fill the title line with attributes.
	// 
	if (!FillConsoleOutputAttribute(hStdout,         // Handle to console screen buffer 
		HIGHLIGHT_ATTRIBUTES, // Character attributes to use
		dwConSize,            // Number of cells to set attribute 
		coordScreen,          // Coordinates of first cell 
		&cCharsWritten))      // Receive number of characters written
	{
		return;
	}

	PrintToConsoleFormatted(CENTER_ALIGN | HIGHLIGHT, title);
	++coordScreen.Y;
	SetConsoleCursorPosition(hStdout, coordScreen);
}

void PrintStatusLine(const string format) {
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD position;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 
	// 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return;
	}

	dwConSize = csbi.dwSize.X;
	position.X = 0;
	position.Y = csbi.dwSize.Y - 1;
	SetConsoleCursorPosition(hStdout, position);

	// Fill the title line with attributes.
	// 
	if (!FillConsoleOutputAttribute(hStdout,         // Handle to console screen buffer 
		HIGHLIGHT_ATTRIBUTES, // Character attributes to use
		dwConSize,            // Number of cells to set attribute 
		position,          // Coordinates of first cell 
		&cCharsWritten))      // Receive number of characters written
	{
		return;
	}

	PrintToConsoleFormatted(HIGHLIGHT, format);

	SetConsoleCursorPosition(hStdout, position);
}

string ShowPrompt(const string title, const string footer, const string format, ...) {
	// Hide the cursor inside the table.
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &info);

	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);

	system("cls");

	PrintTitle(title);
	PrintStatusLine(footer);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD sizeConsole;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		freeBlock(buf);
		return NULL;
	}
	sizeConsole = csbi.dwSize;
	cursorPosition.X = 0;
	cursorPosition.Y = (sizeConsole.Y - 3) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);
	PrintToConsoleFormatted(CENTER_ALIGN, buf);

	// Variable for registering end.
	BOOL done = FALSE;

	string inputString = NULL;

	while (!done) {
		inputString = readLine(stdin);
		if (strlen(inputString) != 0) {
			done = TRUE;
		}
	}

	// Show the cursor.
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hStdout, &info);

	return inputString;
}

int YesNoPrompt(const string title, const string format, ...) {
	// Hide the cursor inside the table.
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &info);

	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);

	system("cls");

	PrintTitle(title);
	PrintStatusLine(" F9: Da. | F10: Ne.");

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD sizeConsole;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		freeBlock(buf);
		return -1;
	}
	sizeConsole = csbi.dwSize;
	cursorPosition.X = 0;
	cursorPosition.Y = (sizeConsole.Y - 3) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);
	PrintToConsoleFormatted(CENTER_ALIGN, buf);

	// Variable for registering end.
	BOOL done = FALSE;

	// Registered event that has happend.
	INPUT_RECORD event;

	// Number of read characters.
	DWORD cRead;

	// Keeping track of the current option selection.
	int selection = 0;

	while (!done) {
		system("pause>nul");
		if (WaitForSingleObject(hStdin, INFINITE) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &event, 1, &cRead);

			/* Only respond to key release events */
			if (event.EventType == KEY_EVENT)
				switch (event.Event.KeyEvent.wVirtualKeyCode) {
				case VK_F9:
					selection = 1;
					done = TRUE;
					break;
				case VK_F10:
					selection = 0;
					done = TRUE;
					break;
				default:
					break;
				}
		}
	}

	// Show the cursor.
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hStdout, &info);

	return selection;
}

CHAR_INFO* SaveScreenBuffer(void) {
	CHAR_INFO* chiBuffer;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT srctRect;
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;

	// Get the console screen buffer info. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return NULL;
	}

	coordBufSize = csbi.dwSize;

	chiBuffer = newArray(coordBufSize.X * coordBufSize.Y, CHAR_INFO);

	// Set the source rectangle.

	srctRect.Top = 0;
	srctRect.Left = 0;
	srctRect.Bottom = coordBufSize.Y - 1;
	srctRect.Right = coordBufSize.X - 1;

	// The top left destination cell of the temporary buffer is 
	// row 0, col 0. 

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	// Copy the block from the screen buffer to the temp. buffer. 

	fSuccess = ReadConsoleOutput(
		hStdout,        // screen buffer to read from 
		chiBuffer,      // buffer to copy into 
		coordBufSize,   // col-row size of chiBuffer 
		coordBufCoord,  // top left dest. cell in chiBuffer 
		&srctRect); // screen buffer source rectangle 
	if (!fSuccess) {
		return NULL;
	}
	return chiBuffer;
}

int RecoverScreenBuffer(CHAR_INFO* chiBuffer) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT srctRect;
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;

	// Get the console screen buffer info. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return 0;
	}

	coordBufSize = csbi.dwSize;

	// Set the destination rectangle. 

	srctRect.Top = 0;
	srctRect.Left = 0;
	srctRect.Bottom = coordBufSize.Y - 1;
	srctRect.Right = coordBufSize.X - 1;

	// The top left destination cell of the temporary buffer is 
	// row 0, col 0. 

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	// Copy from the temporary buffer to the new screen buffer. 

	fSuccess = WriteConsoleOutput(
		hStdout, // screen buffer to write to 
		chiBuffer,        // buffer to copy from 
		coordBufSize,     // col-row size of chiBuffer 
		coordBufCoord,    // top left src cell in chiBuffer 
		&srctRect);  // dest. screen buffer rectangle 
	if (!fSuccess) {
		return 0;
	}
	return 1;
}

int CompareEventNames(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstName = getEventName(first);
	string secondName = getEventName(second);
	return stringCompare(firstName, secondName);
}

int CompareEventLocations(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstLocation = getEventLocation(first);
	string secondLocation = getEventLocation(second);
	int res = stringCompare(firstLocation, secondLocation);
	if (res == 0) {
		res = CompareEventNames(p1, p2);
	}
	return res;
}

int CompareEventCategories(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstCategory = getEventCategory(first);
	string secondCategory = getEventCategory(second);
	int res = stringCompare(firstCategory, secondCategory);
	if (res == 0) {
		res = CompareEventNames(p1, p2);
	}
	return res;
}

int CompareEventTimes(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	time_t firstTime = getEventTime(first);
	time_t secondTime = getEventTime(second);
	if (firstTime == secondTime) {
		return CompareEventNames(p1, p2);
	}
	else {
		return (firstTime < secondTime) ? -1 : +1;
	}
}

int CompareEventCategoryName(const void* p1, const void* p2) {
	EventCategory first = (EventCategory) p1;
	EventCategory second = (EventCategory) p2;
	string firstName = getEventCategoryName(first);
	string secondName = getEventCategoryName(second);
	return stringCompare(firstName, secondName);
}

string InputEventCategory(Table categories) {
	// Save the cursor info.
	CONSOLE_CURSOR_INFO oldInfo;
	GetConsoleCursorInfo(hStdout, &oldInfo);

	string categoryName;

	Table cpyTable = CloneTable(categories);

	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode)) {
		return 0;
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode)) {
		return 0;
	}

	hideCursor();

	// Variable for registering end.
	BOOL done = FALSE;

	// Selected index inside the table.
	int tableSelection = 0;

	// Key code that was registered inside the table.
	WORD registeredKeyCode;

	while (!done) {
		if (!MainTable(cpyTable, &tableSelection, &registeredKeyCode)) {
			// Restore the original console mode. 
			SetConsoleMode(hStdin, fdwOldMode);
			// Restore the cursor.
			SetConsoleCursorInfo(hStdout, &oldInfo);
			return NULL;
		}
		switch (registeredKeyCode) {
		case VK_RETURN:
			done = TRUE;
			break;
		default:
			break;
		}
	}
	EventCategory chosenCategory = getVector(GetDataTable(categories), tableSelection);
	categoryName = getEventCategoryName(chosenCategory);
	FreeTable(cpyTable);

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	// Restore the cursor.
	SetConsoleCursorInfo(hStdout, &oldInfo);

	return categoryName;
}

time_t InputEventTime(void) {
	int day, month, year, hour, minute;
	PrintToConsole("\tDatum (mora biti u obliku \"dan.mjesec.godina.\"): ");
	string inputString = readLine(stdin);
	sscanf(inputString, "%d.%d.%d.", &day, &month, &year);
	//scanf("%d.%d.%d.", &day, &month, &year);
	PrintToConsole("\tVrijeme (mora biti u obliku \"sati:minuti\"): ");
	freeBlock(inputString);
	inputString = readLine(stdin);
	sscanf(inputString, "%d:%d", &hour, &minute);
	//scanf("%d:%d", &hour, &minute);
	// 
	time_t eventTime;
	struct tm dateTime = {
		.tm_mday = day,
		.tm_mon = month - 1,
		.tm_year = year - 1900,
		.tm_hour = hour,
		.tm_min = minute,
		.tm_isdst = -1
	};
	eventTime = mktime(&dateTime);
	return eventTime;
}

int NewEventScreen(Table events, Table categories) {
	string title = "Unos novog događaja";
	system("cls");
	PrintTitle(title);
	advanceCursor(3);
	PrintToConsole("\tNaziv događaja: ");
	string eventName = readLine(stdin);
	PrintToConsole("\tLokacija: ");
	string eventLocation = readLine(stdin);

	time_t eventTime = InputEventTime();

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD oldCordinates; // Save original coordinates.

	// Get the console screen buffer info. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return 0;
	}
	oldCordinates = csbi.dwCursorPosition;

	CHAR_INFO* chiBuffer = SaveScreenBuffer();

	string categoryName = InputEventCategory(categories);

	RecoverScreenBuffer(chiBuffer);
	SetConsoleCursorPosition(hStdout, oldCordinates);

	PrintToConsole("\tKategorija: %s\n", categoryName);

	PrintToConsole("\tOpis: ");
	string eventDescription = readLine(stdin);

	Event temp = newEvent();
	setEventName(temp, eventName);
	setEventLocation(temp, eventLocation);
	setEventTime(temp, eventTime);
	setEventCategory(temp, categoryName);
	setEventDescription(temp, eventDescription);

	Vector eventsVector = GetDataTable(events);
	addVector(eventsVector, temp);

	return 1;
}

void SortEventsTable(Table t) {
	Vector header = GetHeaderTable(t);
	Menu menu = newMenu();
	Vector menuVector = getMenuOptions(menu);
	freeVector(menuVector);
	setMenuOptions(menu, cloneVector(header));
	centerMenu(menu);
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);

	// Selected option inside the main menu.
	int menuOption;

	system("cls");
	PrintTitle("Odaberite naziv kolone po kojoj želite da sortirate podatke");
	if (!mainMenu(menu, &menuOption)) {
		error_msg("mainMenu");
	}
	switch (menuOption) {
	case EVENTS_HEADER_NAME:
		SetCompareFnTable(t, CompareEventNames);
		break;
	case EVENTS_HEADER_LOCATION:
		SetCompareFnTable(t, CompareEventLocations);
		break;
	case EVENTS_HEADER_CATEGORY:
		SetCompareFnTable(t, CompareEventCategories);
		break;
	case EVENTS_HEADER_TIME:
		SetCompareFnTable(t, CompareEventTimes);
		break;
	default:
		break;
	}

}

int EditEvent(Table events, Table categories, int index) {
	Vector data = GetDataTable(events);
	Event event = getVector(data, index);

	Menu menu = newMenu();
	Vector menuVector = getMenuOptions(menu);
	for (int i = 0; i < 5; i++) {
		addVector(menuVector, eventFields[i]);
	}
	centerMenu(menu);
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);
	setCancelMenu(menu, TRUE);

	hideCursor();

	// Variable for registering end.
	BOOL done = FALSE;

	// Selected option inside the main menu.
	int menuOption;

	// String for input.
	string inputString;

	// For input time.
	time_t inputTime;

	while (!done) {
		system("cls");
		PrintTitle("Koje polje želite da mijenjate?");
		PrintStatusLine(" ESC: Povratak. ");
		if (!mainMenu(menu, &menuOption)) {
			return 0;
		}
		switch (menuOption) {
		case EDIT_EVENT_NAME:
			inputString = ShowPrompt("Unesite novi naziv", " RETURN: Potvrdi.", "Naziv: ");
			setEventName(event, inputString);
			break;
		case EDIT_EVENT_LOCATION:
			inputString = ShowPrompt("Unesite novu lokaciju", " RETURN: Potvrdi.", "Lokacija: ");
			setEventLocation(event, inputString);
			break;
		case EDIT_EVENT_CATEGORY:
			inputString = InputEventCategory(categories);
			setEventCategory(event, inputString);
			break;
		case EDIT_EVENT_TIME:
			inputTime = InputEventTime();
			setEventTime(event, inputTime);
			break;
		case EDIT_EVENT_DESCRIPTION:
			inputString = ShowPrompt("Unesite novi opis", " RETURN: Potvrdi.", "Opis: ");
			setEventDescription(event, inputString);
			break;
		case MENU_CANCEL:
			done = TRUE;
			break;
		default:
			break;
		}
	}
	return 1;
}

int ShowEventDetails(Table events, Table categories, int index) {
	Vector data = GetDataTable(events);
	Event event = getVector(data, index);

	struct tm newtime;
	char buff[BUFSIZ];
	time_t long_time = getEventTime(event);
	errno_t err;

	// Convert to local time.
	err = localtime_s(&newtime, &long_time);
	if (err) {
		return 0;
	}

	if (!strftime(buff, sizeof buff, "%A %x %R", &newtime)) {
		return 0;
	}

	string title = "Pregled detalja događaja";

	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode)) {
		showCursor();
		return 0;
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode)) {
		showCursor();
		return 0;
	}

	// Variable for registering end.
	BOOL done = FALSE;

	// Registered event that has happend.
	INPUT_RECORD inputEvent;

	// Number of read characters.
	DWORD cRead;

	while (!done) {
		system("cls");

		PrintTitle(title);
		advanceCursor(3);
		PrintToConsole("\tNaziv: %s\n", getEventName(event));
		PrintToConsole("\tLokacija: %s\n", getEventLocation(event));
		PrintToConsole("\tKategorija: %s\n", getEventCategory(event));
		PrintToConsole("\tDatum i vrijeme: %s\n", buff);
		PrintToConsole("\tOpis: %s", getEventDescription(event));
		PrintStatusLine(" ESC: Povratak. | F9: Izmjena događaja. ");

		hideCursor();
		system("pause>nul");
		if (WaitForSingleObject(hStdin, INFINITE) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &inputEvent, 1, &cRead);

			/* Only respond to key release events */
			if (inputEvent.EventType == KEY_EVENT)
				switch (inputEvent.Event.KeyEvent.wVirtualKeyCode) {
				case VK_ESCAPE:
					done = TRUE;
					break;
				case VK_F9:
					if (!EditEvent(events, categories, index)) {
						return 0;
					}
					break;
				default:
					break;
				}
		}
	}

	showCursor();
	return 1;
}

int EventsHandling(Table events, Table categories) {
	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode)) {
		return 0;
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode)) {
		return 0;
	}

	hideCursor();

	// Variable for registering end.
	BOOL done = FALSE;

	// Selected index inside the table.
	int tableSelection = 0;

	// Key code that was registered inside the table.
	WORD registeredKeyCode;

	while (!done) {
		if (!MainTable(events, &tableSelection, &registeredKeyCode)) {
			showCursor();
			return 0;
		}
		switch (registeredKeyCode) {
		case VK_ESCAPE: // Exit from table.
			done = TRUE;
			break;
		case VK_RETURN: // Show details of the selected event.
			if (isEmptyVector(GetDataTable(events))) {
				break;
			}
			if (!ShowEventDetails(events, categories, tableSelection)) {
				showCursor();
				return 0;
			}
			break;
		case VK_DELETE: // Delete selected event;
			if (isEmptyVector(GetDataTable(events))) {
				break;
			}
			if (YesNoPrompt("Brisanje događaja", "Izbrisati odabrani događaj?")) {
				removeVector(GetDataTable(events), tableSelection);
			}
			tableSelection = 0;
			break;
		case VK_F9: // New event.
			if (isEmptyVector(GetDataTable(categories))) {
				system("cls");
				PrintToConsoleFormatted(CENTER_ALIGN | MIDDLE, "Mora postojati bar jedna kategorija događaja u evidenciji.");
				system("pause>nul");
				break;
			}
			NewEventScreen(events, categories);
			break;
		case VK_F10: // Sort the list.
			SortEventsTable(events);
			break;
		default:
			break;
		}
	}

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	showCursor();
	return 1;
}

void NewCategoryScreen(Table table) {
	string title = "Unos nove kategorije događaja";
	system("cls");
	PrintTitle(title);
	advanceCursor(3);
	PrintToConsole("\tNaziv nove kategorije događaja: ");

	string categoryName = readLine(stdin);

	EventCategory cat = newEventCategory();
	setEventCategoryName(cat, categoryName);

	Vector categories = GetDataTable(table);
	addVector(categories, cat);
}

int CategoriesHandling(Table table) {
	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode)) {
		return 0;
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode)) {
		return 0;
	}
	hideCursor();

	// Variable for registering end.
	BOOL done = FALSE;

	// Selected index inside the table.
	int tableSelection = 0;

	// Key code that was registered inside the table.
	WORD registeredKeyCode;

	while (!done) {
		if (!MainTable(table, &tableSelection, &registeredKeyCode)) {
			showCursor();
			return 0;
		}
		switch (registeredKeyCode) {
		case VK_ESCAPE: // Exit from table.
			done = TRUE;
			break;
		case VK_DELETE: // Delete selected event category;
			if (isEmptyVector(GetDataTable(table))) {
				break;
			}
			if (YesNoPrompt("Brisanje kategorije događaja", "Izbrisati odabranu kategoriju događaja?")) {
				removeVector(GetDataTable(table), tableSelection);
			}
			tableSelection = 0;
			break;
		case VK_F9: // New event.
			NewCategoryScreen(table);
			break;
		default:
			break;
		}
	}
	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	showCursor();
	return 1;
}

int main(void) {
	windowSetup();
	Vector events = newVector();
	Event e;

	/*for (int i = 0; i < 45; i++) {
		e = newEvent();
		StringBuffer sb = newStringBuffer();
		appendString(sb, "name");
		char buffer[10];
		_itoa(i, buffer, 10);
		appendString(sb, buffer);
		setEventName(e, getString(sb));
		setEventLocation(e, "location");
		setEventCategory(e, "category");
		time_t long_time;
		time(&long_time);
		long_time += 10000 * i;
		setEventTime(e, long_time);
		addVector(events, e);
	}*/

	Menu menu = newMenu();
	Vector menuVector = getMenuOptions(menu);
	freeVector(menuVector);
	Vector tmp = arrayToVector(menuOptions, 4);
	setMenuOptions(menu, tmp);
	centerMenu(menu);
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);


	Table eventsTable = NewTable();
	SetDataTable(eventsTable, events);
	/*Vector columns = newVector();
	for (int i = 0; i < 45; i++) {
		Vector record = newVector();
		addVector(record, getEventName(getVector(events, i)));
		addVector(record, getEventLocation(getVector(events, i)));
		addVector(record, getEventCategory(getVector(events, i)));

		struct tm newtime;
		char buff[70];
		time_t long_time = getEventTime(getVector(events, i));
		errno_t err;

		// Convert to local time.
		err = localtime_s(&newtime, &long_time);
		if (err) {
			error_msg("Invalid argument to localtime_s.");
		}

		if (!strftime(buff, sizeof buff, "%A %x %R", &newtime)) {
			error_msg("strftime");
		}

		string tmp = copyString(buff);

		addVector(record, tmp);

		addVector(columns, record);
	}
	SetColumnsTable(eventsTable, columns);
	*/
	Vector header = newVector();
	for (int i = 0; i < 4; i++) {
		string tmp = copyString(eventsHeader[i]);
		addVector(header, tmp);
	}
	SetHeaderTable(eventsTable, header);
	header = newVector();
	for (int i = 0; i < 5; i++) {
		string tmp = copyString(eventsFooter[i]);
		addVector(header, tmp);
	}
	SetFooterTable(eventsTable, header);

	SetHighAttrTable(eventsTable, HIGHLIGHT_ATTRIBUTES);
	SetToStringVectorFnTable(eventsTable, EventToVector);
	SetFreeStringVectorFnTable(eventsTable, FreeEventStringVector);
	SetCompareFnTable(eventsTable, CompareEventNames);


	Table categoriesTable = NewTable();
	SetHighAttrTable(categoriesTable, HIGHLIGHT_ATTRIBUTES);
	SetToStringVectorFnTable(categoriesTable, EventCategoryToVector);
	SetFreeStringVectorFnTable(categoriesTable, FreeEventCategoryStringVector);
	SetCompareFnTable(categoriesTable, CompareEventCategoryName);
	header = newVector();
	for (int i = 0; i < 1; i++) {
		string tmp = copyString(categoriesHeader[i]);
		addVector(header, tmp);
	}
	SetHeaderTable(categoriesTable, header);
	header = newVector();
	for (int i = 0; i < 3; i++) {
		string tmp = copyString(categoriesFooter[i]);
		addVector(header, tmp);
	}
	SetFooterTable(categoriesTable, header);

	// Variable for registering end.
	BOOL done = FALSE;

	// Variable for registering when a user has logged out.
	BOOL loggedOut = FALSE;

	// Selected option inside the main menu.
	int menuOption;

	while (!done) {
		login();
		loggedOut = FALSE;
		while (!loggedOut && !done) {
			system("cls");
			if (!mainMenu(menu, &menuOption)) {
				error_msg("mainMenu");
			}
			switch (menuOption) {
			case EVENT_HANDLING:
				if (!EventsHandling(eventsTable, categoriesTable)) {
					error_msg("EventsHandling");
				}
				break;
			case CATEGORY_HANDLING:
				if (!CategoriesHandling(categoriesTable)) {
					error_msg("CategoriesHandling");
				}
				break;
			case LOGOUT:
				logout();
				loggedOut = TRUE;
				break;
			case EXIT:
				done = TRUE;
				break;
			default:
				break;
			}
		}
	}

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwSaveOldMode);

	// Restore the original text colors. 
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);

	return 0;
}

void windowSetup(void) {
	// Set codepage. Needed for Serbian Latin chars.
	// Also added additional options in project property pages:
	// /source-charset:windows-1250 /execution-charset:windows-1250
	// 
	if (!SetConsoleCP(1250)) {
		error_msg("SetConsoleCP");
	}
	if (!SetConsoleOutputCP(1250)) {
		error_msg("SetConsoleOutputCP");
	}

	// Set system locale.
	// Serbian Latin (Bosnia and Herzegowina)
	// 
	setlocale(LC_ALL, "sr-Latn-BA");

	// Set time zone from TZ environment variable. If TZ is not set,
	// the operating system is queried to obtain the default value
	// for the variable.
	//
	_tzset();

	// Disable window resizing and maximizing.
	// 
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	SetConsoleWindowSize(windowSizeX, windowSizeY);

	// Get handles to STDIN and STDOUT.
	// 
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE || hStdout == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("GetStdHandle"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	// Save the current input mode, to be restored on exit.
	// 
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
		MessageBox(NULL, TEXT("GetConsoleMode"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	// Save the current text colors.
	// 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
		MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	wOldColorAttrs = csbiInfo.wAttributes;

	// Set text attributes.
	// 
	if (!SetConsoleTextAttribute(hStdout, B_WHITE)) {
		MessageBox(NULL, TEXT("SetConsoleTextAttribute"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	// Show cursor.
	// 
	showCursor();

	// Clear screen.
	// 
	system("cls");
}

void SetConsoleWindowSize(int x, int y) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	if (h == INVALID_HANDLE_VALUE)
		error_msg("Unable to get stdout handle.");

	// If either dimension is greater than the largest console window we can have,
	// there is no point in attempting the change.
	{
		COORD largestSize = GetLargestConsoleWindowSize(h);
		if (x > largestSize.X)
			error_msg("The x dimension is too large.");
		if (y > largestSize.Y)
			error_msg("The y dimension is too large.");
	}


	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	if (!GetConsoleScreenBufferInfo(h, &bufferInfo))
		error_msg("Unable to retrieve screen buffer info.");

	SMALL_RECT winInfo = bufferInfo.srWindow;
	COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1 };

	if (windowSize.X > x || windowSize.Y > y) {
		// window size needs to be adjusted before the sb size can be reduced.
		SMALL_RECT info =
		{
			0,
			0,
			x < windowSize.X ? x - 1 : windowSize.X - 1,
			y < windowSize.Y ? y - 1 : windowSize.Y - 1
		};

		if (!SetConsoleWindowInfo(h, TRUE, &info))
			error_msg("Unable to resize window before resizing buffer.");
	}

	COORD size = { x, y };
	if (!SetConsoleScreenBufferSize(h, size))
		error_msg("Unable to resize screen buffer.");


	SMALL_RECT info = { 0, 0, x - 1, y - 1 };
	if (!SetConsoleWindowInfo(h, TRUE, &info))
		error_msg("Unable to resize window after resizing buffer.");
}
