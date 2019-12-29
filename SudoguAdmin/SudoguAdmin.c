#define _WIN32_WINNT 0x0500

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

enum {
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

enum {
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

string menu[] = {
	"Upravljanje dogadjajima",
	"Upravljanje kategorijama",
	"Odjava",
	"Izlaz"
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

const int windowSizeX = 120;
const int windowSizeY = 45;

void error_msg(const char* format, ...) {
	va_list args;
	fprintf(stderr, "[%s] ERROR: ", programName);
	if (format != NULL) {
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}
	if (errno != 0) {
		fprintf(stderr, ": %s\n", strerror(errno));
	}
	putc('\n', stderr);

	// Restore input mode on exit.
	SetConsoleMode(hStdin, fdwSaveOldMode);

	// Restore the original text colors. 
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);

	exit(EXIT_FAILURE);
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

int fileToMap(string filename, Map map)
{
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

void freeMapFields(Map Map)
{
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

StringBuffer readPassword(void)
{
	StringBuffer password;
	password = newStringBuffer();
	int ch;
	int i = -1; // this variable counts input characters

	do
	{
		++i;
		ch = _getch();
		if (ch == 0x08) // backspace
		{
			if (i > 0)
			{
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

void clear(int startX, int startY, int height, int width)
{
	COORD here;
	DWORD cWritten;
	for (int i = 0; i < height; i++)
	{
		here.Y = startY + i;
		for (int j = 0; j < width; j++)
		{
			here.X = startX + j;
			WriteConsoleOutputCharacter(hStdout, L" ", 1, here, &cWritten);
		}
	}

}

DWORD printToConsole(string format, ...)
{
	StringBuffer sb = newStringBuffer();
	va_list args;
	int capacity;

	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(sb, capacity, format, args);
	va_end(args);

	DWORD cWritten;
	if (!WriteFile(
		hStdout,               // output handle 
		getString(sb),         // prompt string 
		sizeStringBuffer(sb),  // string length 
		&cWritten,             // bytes written 
		NULL))                 // not overlapped 
	{
		error_msg("WriteFile");
	}
	freeStringBuffer(sb);
	return cWritten;
}

DWORD printTextAttributes(WORD wAttributes, string format, ...)
{
	StringBuffer sb = newStringBuffer();
	DWORD ret;
	va_list args;
	int capacity;

	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(sb, capacity, format, args);
	va_end(args);

	WORD wOldColor;

	// Save the current text colors. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		error_msg("GetConsoleScreenBufferInfo");
	}

	wOldColor = csbiInfo.wAttributes;

	// Set the text attributes. 
	if (!SetConsoleTextAttribute(hStdout, wAttributes))
	{
		error_msg("SetConsoleTextAttribute");
	}

	ret = printToConsole(getString(sb));

	// Restore the original text colors. 
	SetConsoleTextAttribute(hStdout, wOldColor);

	freeStringBuffer(sb);
	return ret;
}

DWORD printCenter(string format, ...)
{
	StringBuffer sb = newStringBuffer();
	DWORD ret;
	va_list args;
	int capacity;

	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(sb, capacity, format, args);
	va_end(args);

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD sizeConsole;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
		return 0;
	}
	sizeConsole = csbiInfo.dwSize;
	cursorPosition = csbiInfo.dwCursorPosition;
	cursorPosition.X = (sizeConsole.X - sizeStringBuffer(sb)) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);

	ret = printToConsole(getString(sb));

	freeStringBuffer(sb);
	return ret;
}

DWORD printCenterAttributes(WORD wAttributes, string format, ...)
{
	StringBuffer sb = newStringBuffer();
	DWORD ret;
	va_list args;
	int capacity;

	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(sb, capacity, format, args);
	va_end(args);

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD sizeConsole;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
		return 0;
	}
	sizeConsole = csbiInfo.dwSize;
	cursorPosition = csbiInfo.dwCursorPosition;
	cursorPosition.X = (sizeConsole.X - sizeStringBuffer(sb)) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);

	ret = printTextAttributes(wAttributes, getString(sb));

	freeStringBuffer(sb);
	return ret;
}

void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	COORD oldCordinates;

	// Get the number of character cells in the current buffer. 

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	oldCordinates = csbi.dwCursorPosition;

	// Fill the entire screen with blanks.

	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer 
		(TCHAR)' ',     // Character to write to the buffer
		dwConSize,       // Number of cells to write 
		coordScreen,     // Coordinates of first cell 
		&cCharsWritten))// Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
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

void advanceCursor(int count)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		error_msg("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}
	cursorPosition = csbiInfo.dwCursorPosition;
	cursorPosition.Y += count;
	SetConsoleCursorPosition(hStdout, cursorPosition);
}

void loginAttempt(string* inUsername, string* inPassword)
{
	LPSTR prompt1 = "Username: ";
	LPSTR prompt2 = "Password: ";
	const int promptCount = 2;
	CHAR chBuffer[256];
	DWORD cRead, cWritten;
	COORD cursorPosition;

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD sizeConsole;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		error_msg("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}

	sizeConsole = csbiInfo.dwSize;

	cursorPosition.X = (sizeConsole.X - lstrlenA(prompt1) * 2) / 2;
	cursorPosition.Y = (sizeConsole.Y - promptCount) / 2;

	//system("cls");
	clear(0, cursorPosition.Y, 3, sizeConsole.X);

	SetConsoleCursorPosition(hStdout, cursorPosition);

	cWritten = printToConsole(prompt1);

	*inUsername = readLine(stdin);

	++cursorPosition.Y;
	SetConsoleCursorPosition(hStdout, cursorPosition);

	printToConsole(prompt2);

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

void login(void)
{
	Map accountsMap = NULL;
	string inUsername = NULL, inPassword = NULL;

	cls(hStdout);
	while (true) {
		if (accountsMap != NULL)
		{
			freeMapFields(accountsMap);
		}
		if (inUsername != NULL)
		{
			freeBlock(inUsername);
		}
		if (inPassword != NULL)
		{
			freeBlock(inPassword);
		}

		accountsMap = newMap();
		if (fileToMap(fileAccounts, accountsMap) == 0)
		{
			freeMapFields(accountsMap);
			error_msg("Konfiguracioni fajl %s nije ispravan.\n", fileAccounts);
		}

		loginAttempt(&inUsername, &inPassword);

		cls(hStdout);
		advanceCursor(3);

		if (stringLength(inUsername) == 0 || stringLength(inPassword) == 0)
		{
			printCenterAttributes(F_RED | B_WHITE, "Pogresan unos.");
			continue;
		}

		if (containsKeyMap(accountsMap, inUsername) == true)
		{
			string correctPassword = getMap(accountsMap, inUsername);
			if (stringCompare(inPassword, correctPassword) == 0)
			{
				printCenterAttributes(F_CYAN | B_WHITE, "Dobrodosli %s.\n", inUsername);
				username = inUsername;
				break;
			}
			else
			{
				printCenterAttributes(F_RED | B_WHITE, "Pogresna lozinka.");
				continue;
			}
		}
		else
		{
			printCenterAttributes(F_RED | B_WHITE, "Ne postoji nalog sa imenom '%s'.\n", inUsername);
			continue;
		}
	}

	if (accountsMap != NULL)
	{
		freeMapFields(accountsMap);
	}
	if (inUsername != NULL)
	{
		freeBlock(inUsername);
	}
	if (inPassword != NULL)
	{
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

void logout(string username)
{
	freeBlock(username);
}

int multipleChoice(bool canCancel, string* options, int count)
{
	int startX;
	int startY;
	const int optionsPerLine = 1;
	const int spacingPerLine = 25;

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	startX = (windowSizeX - optionsPerLine * spacingPerLine) / 2;
	startY = (windowSizeY - count) / 2;

	DWORD cRead;
	COORD here;
	INPUT_RECORD event;
	BOOL done = FALSE;

	// hide cursor
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &info);

	DWORD fdwMode, fdwOldMode;

	// Turn off the line input and echo input modes 
	if (!GetConsoleMode(hStdin, &fdwOldMode))
	{
		error_msg("GetConsoleMode");
	}

	fdwMode = fdwOldMode &
		~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	if (!SetConsoleMode(hStdin, fdwMode))
	{
		error_msg("SetConsoleMode");
	}

	int currentSelection = 0;
	cls(hStdout);
	while(!done)
	{
		for (int i = 0; i < count; i++)
		{
			here.X = startX + (i % optionsPerLine) * spacingPerLine;
			here.Y = startY + i / optionsPerLine;

			SetConsoleCursorPosition(hStdout, here);

			if (i == currentSelection) 
			{
				printTextAttributes(F_RED | B_WHITE | COMMON_LVB_UNDERSCORE, options[i]);
			}
			else
			{
				printToConsole(options[i]);
			}
		}
		system("pause>nul");
		if (WaitForSingleObject(hStdin, 200) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &event, 1, &cRead);

			/* Only respond to key release events */
			if ((event.EventType == KEY_EVENT) && !event.Event.KeyEvent.bKeyDown)
				switch (event.Event.KeyEvent.wVirtualKeyCode)
				{
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
					if (currentSelection + optionsPerLine < count)
						currentSelection += optionsPerLine;
					break;
				case VK_ESCAPE:
					if (canCancel) return -1;
					break;
				case VK_RETURN:
					done = TRUE;
					break;
				default:
					break;
				}
		}
		//Sleep(100); // loop will only start again after 1/10 of a second
	}

	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hStdout, &info);

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	return currentSelection;
}

void SetConsoleWindowSize(int x, int y);

void windowSetup(void);

int main(void)
{
	windowSetup();

	while (true) {
		login();
		multipleChoice(false, menu, 4);
		Map cityMap = newMap();
		if (fileToMap(fileCity, cityMap) != 1) {
			fprintf(stderr, "Neispravna konfiguracija parametara grada unutar datoteke %s.\n", fileCity);
			freeMapFields(cityMap);
		}
		else {
			string cityName = getMap(cityMap, "name");
			printf("Naziv grada: %s.\n", cityName);
		}
		getchar();
		logout(username);
	}

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwSaveOldMode);

	// Restore the original text colors. 
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);

	return 0;
}

void windowSetup(void)
{
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	SetConsoleWindowSize(windowSizeX, windowSizeY);

	// Get handles to STDIN and STDOUT. 

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE || hStdout == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("GetStdHandle"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	// Save the current input mode, to be restored on exit. 

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
	{
		MessageBox(NULL, TEXT("GetConsoleMode"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	// Save the current text colors. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	wOldColorAttrs = csbiInfo.wAttributes;

	if (!SetConsoleTextAttribute(hStdout, B_WHITE))
	{
		MessageBox(NULL, TEXT("SetConsoleTextAttribute"), TEXT("Console Error"), MB_OK);
		exit(EXIT_FAILURE);
	}
	system("cls");
}

void SetConsoleWindowSize(int x, int y)
{
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

	if (windowSize.X > x || windowSize.Y > y)
	{
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
