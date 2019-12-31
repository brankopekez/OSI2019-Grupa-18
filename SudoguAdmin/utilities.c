#include "utilities.h"
#include <stdarg.h>
#include "cslib.h"
#include <stdlib.h>
#include <windows.h>
#include <strsafe.h>
#include <stdarg.h>

// Extern variables.
extern HANDLE hStdout;
extern HANDLE hStdin;

void ErrorExit(LPTSTR lpszFunction) {
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR) lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
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
