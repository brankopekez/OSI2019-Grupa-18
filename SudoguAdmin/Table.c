#include "Table.h"
#include "vector.h"
#include <Windows.h>
#include "utilities.h"
#include "cslib.h"
#include "strbuf.h"
#include "strlib.h"

// Extern variables.
extern HANDLE hStdout;
extern HANDLE hStdin;

struct TableCDT {
	Vector data;
	int startX;
	int startY;
	int endX;
	int endY;
	WORD highlightAttributes;
	Vector columns;
	Vector header;
	Vector footer;
};

Table NewTable(void) {
	Table t = newBlock(Table);
	t->data = NULL;
	t->startX = 0;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD windowSize;
	// Get the console screen buffer info. 
	if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
		//return 0;
	}
	windowSize = csbi.dwSize;

	t->endX = windowSize.X - 1;
	t->startY = 0;
	t->endY = windowSize.Y - 1;
	t->highlightAttributes = 0;
	t->columns = NULL;
	t->header = NULL;
	t->footer = NULL;
	return t;
}

void FreeTable(Table t) {
	freeVector(t->data);
	freeVector(t->columns);
	freeBlock(t);
}

Vector GetDataTable(Table t) {
	return t->data;
}

void SetDataTable(Table t, Vector vec) {
	t->data = vec;
}

int GetStartXTable(Table t) {
	return t->startX;
}

void SetStartXTable(Table t, int posX) {
	t->startX = posX;
}

int GetStartYTable(Table t) {
	return t->startY;
}

void SetStartYTable(Table t, int posY) {
	t->startY = posY;
}

int GetEndXTable(Table t) {
	return t->endX;
}

void SetEndXTable(Table t, int posX) {
	t->endX = posX;
}

int GetEndYTable(Table t) {
	return t->endY;
}

void SetEndYTable(Table t, int posY) {
	t->endY = posY;
}

WORD GetHighAttrTable(Table t) {
	return t->highlightAttributes;
}

void SetHighAttrTable(Table t, WORD attributes) {
	t->highlightAttributes = attributes;
}

Vector GetColumnsTable(Table t) {
	return t->columns;
}

void SetColumnsTable(Table t, Vector columns) {
	t->columns = columns;
}

Vector GetHeaderTable(Table t) {
	return t->header;
}

void SetHeaderTable(Table t, Vector header) {
	t->header = header;
}

Vector GetFooterTable(Table t) {
	return t->footer;
}

void SetFooterTable(Table t, Vector footer) {
	t->footer = footer;
}

int GetTableHeight(Table t) {
	return (t->endY - t->startY + 1);
}

int GetTableWidth(Table t) {
	return (t->endX - t->startX + 1);
}

int GetTotalTable(Table t) {
	return sizeVector(t->data);
}

static string AlignCentre(string text, int width) {
	int textLength = stringLength(text);
	StringBuffer sb = newStringBuffer();
	if (textLength > width) {
		appendString(sb, substring(text, 0, width - 3));
		appendString(sb, "...");
	}
	else {
		appendString(sb, text);
	}

	if (sizeStringBuffer(sb) == 0) {
		string ret = copyString(getString(sb));
		freeStringBuffer(sb);
		return ret;
	}
	else {
		StringBuffer newString = newStringBuffer();
		for (int i = 0; i < (width - textLength) / 2; i++) {
			pushChar(newString, ' ');
		}
		appendString(newString, getString(sb));
		for (int i = sizeStringBuffer(newString); i < width; i++) {
			pushChar(newString, ' ');
		}
		string ret = copyString(getString(newString));
		freeStringBuffer(newString);
		freeStringBuffer(sb);
		return ret;
	}
}

static void PrintRow(Table t, Vector columns) {
	int tableWidth = GetTableWidth(t);
	int columnsCount = sizeVector(columns);
	int width = (tableWidth - columnsCount - 1) / columnsCount;
	StringBuffer row = newStringBuffer();

	appendString(row, " ");
	for (int i = 0; i < sizeVector(columns); i++) {
		string column = getVector(columns, i);
		string alignedColumn = AlignCentre(column, width);
		appendString(row, alignedColumn);
		freeBlock(alignedColumn);
		if (i != sizeVector(columns) - 1) {
			appendString(row, "|");
		}
		else {
			while (sizeStringBuffer(row) < tableWidth) {
				appendString(row, " ");
			}
		}
	}

	string strToPrint = copyString(getString(row));
	PrintToConsole("%s", strToPrint);
	freeBlock(strToPrint);
	freeStringBuffer(row);
}

static void PrintHighlightedRow(Table t, Vector columns) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	WORD wOldColor;
	WORD wAttributes = GetHighAttrTable(t);

	// Save the current text colors. 
	if (!GetConsoleScreenBufferInfo(hStdout, &info)) {
		return 0;
	}

	wOldColor = info.wAttributes;

	// Set the text attributes. 
	if (!SetConsoleTextAttribute(hStdout, wAttributes)) {
		return 0;
	}

	PrintRow(t, columns);

	// Restore the original text colors. 
	SetConsoleTextAttribute(hStdout, wOldColor);
}

static int DrawTable(Table table, int currentSelection, int startIndex) {
	// Start line.
	int startY = GetStartYTable(table);

	// End line.
	int endY = GetEndYTable(table);

	// Start console column.
	int startX = GetStartXTable(table);

	// End console column.
	int endX = GetEndXTable(table);

	// Total number of options inside the table.
	int totalOptions = GetTotalTable(table);

	// Table height.
	int tableHeight = GetTableHeight(table);

	// Coordinates of the current line.
	COORD here = { startX, startY };

	// Highlighting attributes.
	WORD wAttributes = GetHighAttrTable(table);

	// Set the coordinates.
	SetConsoleCursorPosition(hStdout, here);

	// Print the header first.
	PrintHighlightedRow(table, GetHeaderTable(table));

	// Increment Y coordinate for one because of the header.
	++startY;

	for (int i = 0; i < tableHeight - 2 && startIndex + i < totalOptions; i++) {
		here.Y = startY + i;

		SetConsoleCursorPosition(hStdout, here);

		if (startIndex + i == currentSelection) {
			PrintHighlightedRow(table, getVector(GetColumnsTable(table), startIndex + i));
		}
		else {
			PrintRow(table, getVector(GetColumnsTable(table), startIndex + i));
		}
	}

	// Print the footer.
	PrintHighlightedRow(table, GetFooterTable(table));

	return 1;
}

int MainTable(Table table, int* selection) {
	// Hide the cursor inside the table.
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

	// Keeping track of the current option selection inside the table.
	int currentSelection = 0;

	// Total number of options inside the table.
	int totalOptions = GetTotalTable(table);

	// Start line.
	int startY = GetStartYTable(table);

	// End line.
	int endY = GetEndYTable(table);

	// Start console column.
	int startX = GetStartXTable(table);

	// End console column.
	int endX = GetEndXTable(table);

	// Index of an element that lies on the top of the shown table.
	int startIndex = 0;

	// Table height.
	int tableHeight = GetTableHeight(table);

	// Registered event that has happend.
	INPUT_RECORD event;

	// Number of read characters.
	DWORD cRead;

	// Loop for showing the menu, until done.
	while (!done) {
		//if (!clear(startY, endY)) {
		//	return 0;
		//}
		system("cls");
		if (!DrawTable(table, currentSelection, startIndex)) {
			return 0;
		}
		system("pause>nul");
		if (WaitForSingleObject(hStdin, INFINITE) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &event, 1, &cRead);

			/* Only respond to key release events */
			if (event.EventType == KEY_EVENT)
				switch (event.Event.KeyEvent.wVirtualKeyCode) {
				case VK_UP:
					if (currentSelection != 0) {
						--currentSelection;
						if (currentSelection < startIndex) {
							--startIndex;
						}
					}
					break;
				case VK_DOWN:
					if (currentSelection < totalOptions - 1) {
						++currentSelection;
						if (currentSelection >= startIndex + tableHeight - 2) {
							++startIndex;
						}
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
