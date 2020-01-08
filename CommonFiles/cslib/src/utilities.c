#include "utilities.h"
#include <stdarg.h>
#include "cslib.h"
#include <stdlib.h>
#include <windows.h>
#include <strsafe.h>
#include <stdarg.h>
#include "vector.h"
#include "Event.h"
#include "EventCategory.h"
#include "simpio.h"
#include "strlib.h"
#include "map.h"

// Extern variables.
extern HANDLE hStdout;
extern HANDLE hStdin;

void ErrorExit(LPSTR lpszFunction) {
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf = NULL;
	LPVOID lpDisplayBuf = NULL;
	DWORD dw = GetLastError();

	if (dw) {
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR) &lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
			(lstrlenA((LPSTR) lpMsgBuf) + lstrlenA((LPSTR) lpszFunction) + 40) * sizeof(CHAR));

		StringCchPrintfA((LPSTR) lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(CHAR),
			("%s failed with error %d: %s"),
			lpszFunction, dw, lpMsgBuf);
	}
	else {
		lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
			((lstrlenA((LPSTR) lpszFunction)) * sizeof(CHAR)));

		StringCchPrintfA((LPSTR) lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(CHAR),
			("%s"),
			lpszFunction);
	}
	MessageBoxA(NULL, (LPSTR) lpDisplayBuf, ("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(EXIT_FAILURE);
}

void error_msg(const string format, ...) {
	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);
	ErrorExit(buf);
}

int clear(int startY, int endY) {
	COORD coordScreen = { 0, startY };    // Home for the cursor.
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize; // Number of cells to overwrite.
	COORD oldCordinates; // Save original coordinates.

	// Get the console screen buffer info. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return 0;
	}

	dwConSize = csbi.dwSize.X * (endY - startY + 1);
	oldCordinates = csbi.dwCursorPosition;

	// Fill the entire screen with blanks.

	if (!FillConsoleOutputCharacter(hStdout,        // Handle to console screen buffer 
		(TCHAR) ' ',     // Character to write to the buffer
		dwConSize,       // Number of cells to write 
		coordScreen,     // Coordinates of first cell 
		&cCharsWritten)) // Receive number of characters written
	{
		return 0;
	}

	// Get the current text attribute.

	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		return 0;
	}

	// Set the buffer's attributes accordingly.

	if (!FillConsoleOutputAttribute(hStdout, // Handle to console screen buffer 
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute 
		coordScreen,      // Coordinates of first cell 
		&cCharsWritten))  // Receive number of characters written
	{
		return 0;
	}

	// Put the cursor at its original coordinates.

	SetConsoleCursorPosition(hStdout, oldCordinates);

	return 1;
}

// Formats a message string using the specified message and variable
// list of arguments.
LPWSTR GetFormattedMessage(LPWSTR pMessage, ...) {
	LPWSTR pBuffer = NULL;

	va_list args = NULL;
	va_start(args, pMessage);

	FormatMessage(FORMAT_MESSAGE_FROM_STRING |
		FORMAT_MESSAGE_ALLOCATE_BUFFER,
		pMessage,
		0,
		0,
		(LPWSTR) &pBuffer,
		0,
		&args);

	va_end(args);

	return pBuffer;
}

int PrintToConsole(const string format, ...) {
	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);

	DWORD cWritten;
	if (!WriteFile(
		hStdout,               // output handle 
		buf,                   // string buffer
		lstrlenA(buf),         // string length 
		&cWritten,             // bytes written 
		NULL))                 // not overlapped 
	{
		freeBlock(buf);
		return -1;
	}
	freeBlock(buf);

	return cWritten;
}

extern WORD HIGHLIGHT_ATTRIBUTES;

int PrintToConsoleFormatted(unsigned short options, const string format, ...) {
	WORD wOldColor;

	va_list args;
	va_start(args, format);
	int len = _vscprintf(format, args) + 1;
	string buf = newArray(len, char);
	vsprintf(buf, format, args);

	if (options & CENTER_ALIGN || options & MIDDLE) {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		COORD sizeConsole;
		COORD cursorPosition;

		// Get the current screen sb size and window position. 

		if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
			freeBlock(buf);
			return -1;
		}
		sizeConsole = csbi.dwSize;
		cursorPosition = csbi.dwCursorPosition;
		if (options & CENTER_ALIGN) {
			cursorPosition.X = (sizeConsole.X - len) / 2;
		}
		if (options & MIDDLE) {
			cursorPosition.Y = sizeConsole.Y / 2;
		}
		SetConsoleCursorPosition(hStdout, cursorPosition);
	}
	if (options & HIGHLIGHT) {
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		// Save the current text colors. 
		if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
			freeBlock(buf);
			return -1;
		}

		wOldColor = csbi.wAttributes;

		// Set the text attributes. 
		if (!SetConsoleTextAttribute(hStdout, HIGHLIGHT_ATTRIBUTES)) {
			freeBlock(buf);
			return -1;
		}
	}

	DWORD cWritten;
	if (!WriteFile(
		hStdout,               // output handle 
		buf,                   // string buffer
		lstrlenA(buf),         // string length 
		&cWritten,             // bytes written 
		NULL))                 // not overlapped 
	{
		freeBlock(buf);
		return -1;
	}
	freeBlock(buf);

	if (options & HIGHLIGHT) {
		// Restore the original text colors. 
		SetConsoleTextAttribute(hStdout, wOldColor);
	}

	return cWritten;
}

void hideCursor(void) {
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &info);
}

void showCursor(void) {
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hStdout, &info);
}

static int split(Vector vector, int begin, int end, CompareFn compareFn) {
	int i = begin, j = end;
	void* pivot = getVector(vector, begin);
	while (i < j) {
		while (compareFn(getVector(vector, i), pivot) <= 0 && i < j) {
			i++;
		}
		while (compareFn(getVector(vector, j), pivot) > 0) {
			j--;
		}
		if (i < j) {
			void* temp = getVector(vector, i);
			setVector(vector, i, getVector(vector, j));
			setVector(vector, j, temp);
		}
	}
	setVector(vector, begin, getVector(vector, j));
	setVector(vector, j, pivot);
	return j;
}

void QuickSortVector(Vector vector, int begin, int end, CompareFn compareFn) {
	if (begin < end) {
		int pivot = split(vector, begin, end, compareFn);
		QuickSortVector(vector, begin, pivot - 1, compareFn);
		QuickSortVector(vector, pivot + 1, end, compareFn);
	}
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

char* ReadString(FILE* inf) {
	if (inf == NULL)
		return NULL;
	size_t size = 1;
	char* buf = malloc(size);
	if (buf == NULL) {
		error_msg("Alokacija memorije neuspješna");
	}
	size_t i = 0;
	int ch;
	while ((ch = fgetc(inf)) != EOF && ch != '\0') {
		if (i + 1 >= size) {
			size_t newsize = (size + 1) * 4 - 1;
			char* newbuf = realloc(buf, newsize);
			if (newbuf == NULL) {
				error_msg("Alokacija memorije neuspješna");
			}
			buf = newbuf;
			size = newsize;
		}
		buf[i++] = ch;
	}
	buf[i++] = '\0';
	// If a final re-size is desired...
	char* newbuf = realloc(buf, i);
	if (newbuf == NULL) {
		error_msg("Alokacija memorije neuspješna");
	}
	buf = newbuf;
	return buf;
}

Event ReadEvent(FILE* filepoint) {
	string eventName = ReadString(filepoint);
	string eventDescription = ReadString(filepoint);
	string eventLocation = ReadString(filepoint);
	string eventCategory = ReadString(filepoint);
	time_t eventTime;
	fread(&eventTime, sizeof(time_t), 1, filepoint);

	Event e = newEvent();
	setEventName(e, eventName);
	setEventDescription(e, eventDescription);
	setEventLocation(e, eventLocation);
	setEventCategory(e, eventCategory);
	setEventTime(e, eventTime);

	return e;
}

bool fileExists(string path) {
	int status;

	if (path == NULL) error("fileExists: NULL filename");
	status = _access(path, 0);
	if (status == 0) return true;
	if (errno == EINVAL) error("fileExists: Invalid parameter");
	return false;
}

Vector ReadEventsFromFile(string fileName) {
	FILE* filepoint;
	errno_t err;

	if ((err = fopen_s(&filepoint, fileName, "rb")) != 0) {
		// File could not be opened. filepoint was set to NULL
		// error code is returned in err.
		// error message can be retrieved with strerror(err);

		error_msg("Nije moguće otvoriti fajl %s", fileName);
	}
	else {
		// File was opened, filepoint can be used to read the stream.

		Vector events = newVector();
		size_t count;
		fread(&count, sizeof count, 1, filepoint);
		for (size_t i = 0; i < count; i++) {
			Event e = ReadEvent(filepoint);
			addVector(events, e);
		}
		fclose(filepoint);
		return events;
	}
}

size_t WriteStringToFile(FILE* filepoint, string outString) {
	return fwrite(outString, sizeof outString[0], strlen(outString) + 1, filepoint);
}

void WriteEventToFile(FILE* filepoint, Event e) {
	string eventName = getEventName(e);
	string eventDescription = getEventDescription(e);
	string eventLocation = getEventLocation(e);
	string eventCategory = getEventCategory(e);
	time_t eventTime = getEventTime(e);

	WriteStringToFile(filepoint, eventName);
	WriteStringToFile(filepoint, eventDescription);
	WriteStringToFile(filepoint, eventLocation);
	WriteStringToFile(filepoint, eventCategory);
	fwrite(&eventTime, sizeof(time_t), 1, filepoint);
}

void SaveEventsToFile(Vector events, string fileName) {
	FILE* filepoint;
	errno_t err;

	if ((err = fopen_s(&filepoint, fileName, "wb")) != 0) {
		// File could not be opened. filepoint was set to NULL
		// error code is returned in err.
		// error message can be retrieved with strerror(err);

		error_msg("Nije moguće otvoriti fajl %s", fileName);
	}
	else {
		// File was opened, filepoint can be used to read the stream.

		CompareFn cmpFn = CompareEventTimesDescending;
		QuickSortVector(events, 0, sizeVector(events) - 1, cmpFn);
		size_t count = sizeVector(events);
		fwrite(&count, sizeof count, 1, filepoint);
		for (size_t i = 0; i < count; i++) {
			Event e = getVector(events, i);
			WriteEventToFile(filepoint, e);
		}

		fclose(filepoint);
	}
}

EventCategory ReadCategory(FILE* filepoint) {
	string categoryName = ReadString(filepoint);

	EventCategory cat = newEventCategory();
	setEventCategoryName(cat, categoryName);

	return cat;
}

Vector ReadCategoriesFromFile(string fileName) {
	FILE* filepoint;
	errno_t err;

	if ((err = fopen_s(&filepoint, fileName, "rb")) != 0) {
		// File could not be opened. filepoint was set to NULL
		// error code is returned in err.
		// error message can be retrieved with strerror(err);

		error_msg("Nije moguće otvoriti fajl %s", fileName);
	}
	else {
		// File was opened, filepoint can be used to read the stream.

		Vector categories = newVector();
		size_t count;
		fread(&count, sizeof count, 1, filepoint);
		for (size_t i = 0; i < count; i++) {
			EventCategory e = ReadCategory(filepoint);
			addVector(categories, e);
		}
		fclose(filepoint);
		return categories;
	}
}

void WriteCategoryToFile(FILE* filepoint, EventCategory cat) {
	string categoryName = getEventCategoryName(cat);
	WriteStringToFile(filepoint, categoryName);
}

void SaveCategoriesToFile(Vector categories, string fileName) {
	FILE* filepoint;
	errno_t err;

	if ((err = fopen_s(&filepoint, fileName, "wb")) != 0) {
		// File could not be opened. filepoint was set to NULL
		// error code is returned in err.
		// error message can be retrieved with strerror(err);

		error_msg("Nije moguće otvoriti fajl %s", fileName);
	}
	else {
		// File was opened, filepoint can be used to read the stream.

		size_t count = sizeVector(categories);
		fwrite(&count, sizeof count, 1, filepoint);
		for (size_t i = 0; i < count; i++) {
			EventCategory cat = getVector(categories, i);
			WriteCategoryToFile(filepoint, cat);
		}

		fclose(filepoint);
	}
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

void SetCursorPositionMiddle() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD sizeConsole;
	COORD cursorPosition;

	// Get the current screen sb size and window position. 

	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		error_msg("GetConsoleScreenBufferInfo");
	}
	sizeConsole = csbi.dwSize;
	cursorPosition.X = 0;
	cursorPosition.Y = (sizeConsole.Y - 3) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);
}

string ShowPrompt(const string title, const string footer, const string format, ...) {
	showCursor();

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
	cursorPosition.X = sizeConsole.X / 3;
	cursorPosition.Y = (sizeConsole.Y - 3) / 2;
	SetConsoleCursorPosition(hStdout, cursorPosition);
	PrintToConsole(buf);

	// Variable for registering end.
	BOOL done = FALSE;

	string inputString = NULL;

	while (!done) {
		inputString = getLine();
		if (strlen(inputString) != 0) {
			done = TRUE;
		}
	}

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
