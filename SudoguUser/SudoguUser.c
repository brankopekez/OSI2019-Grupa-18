#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "../SudoguAdmin/cslib.h"
#include "../SudoguAdmin/map.h"
#include "../SudoguAdmin/iterator.h"
#include "../SudoguAdmin/simpio.h"
#include "../SudoguAdmin/cmpfn.h"
#include "../SudoguAdmin/strlib.h"
#include "../SudoguAdmin/strbuf.h"
#include "../SudoguAdmin/generic.h"
#include "../SudoguAdmin/EventCategory.h"
#include "../SudoguAdmin/Event.h"
#include "../SudoguAdmin/vector.h"
#include <time.h>
#include <locale.h>
#include "../SudoguAdmin/Menu.h"
#include "../SudoguAdmin/utilities.h"
#include "../SudoguAdmin/Table.h"
#include <io.h>

/** @brief	The array of menu options. */
string menuOptions[5] = {
	" Pregled današnjih događaja ",
	" Pregled događaja određene kategorije ",
	" Pregled svih budućih događaja ",
	" Pregled događaja koji su prošli ",
	" Izlaz "
};

enum M_MENU {
	MENU_TODAYS_EVENTS,
	MENU_CATEGORY_EVENTS,
	MENU_FUTURE_EVENTS,
	MENU_PAST_EVENTS,
	EXIT
};

/** @brief	The events table header (column names). */
string eventsHeader[4] = {
	"Naziv",
	"Lokacija",
	"Kategorija",
	"Datum i vrijeme"
};

/** @brief	The events table footer. */
string eventsFooter[3] = {
	"ESC: Izlaz.",
	"RETURN: Detalji.",
	"F10: Sortiraj listu."
};

string categoriesHeader[1] = {
	"Naziv kategorije događaja"
};

string categoriesFooter[3] = {
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

/** @brief	The city config file name  */
const string fileCity = "city.txt";

/** @brief	The events data file name  */
const string fileEvents = "events.dat";

/** @brief	The event categories data file name  */
const string fileCategories = "categories.dat";

/** @brief	Name of the program (used on error) */
const string programName = "SudoguUser";

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

void windowSetup(void);

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

int ShowEventDetails(Table events, int index) {
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
		PrintStatusLine(" ESC: Povratak. ");

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
				default:
					break;
				}
		}
	}

	showCursor();
	return 1;
}

int EventsHandling(Table events) {
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
			if (!ShowEventDetails(events, tableSelection)) {
				showCursor();
				return 0;
			}
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

typedef int (*FilterFn)(void*);

int IsTodaysEvent(Event event) {
	time_t now;
	time(&now);

	struct tm tmNow;
	tmNow = *localtime(&now);

	time_t eventTime = getEventTime(event);
	struct tm tmEvent;
	tmEvent = *localtime(&eventTime);

	if (tmNow.tm_year == tmEvent.tm_year
		&& tmNow.tm_yday == tmEvent.tm_yday) {
		return 1;
	}
	else {
		return 0;
	}
}

Vector FilterVector(Vector old, FilterFn filterFn) {
	Vector new = newVector();
	for (size_t i = 0; i < sizeVector(old); i++) {
		void* record = getVector(old, i);
		if (filterFn(record)) {
			addVector(new, record);
		}
	}
	return new;
}

int ShowTodaysEvents(Table eventsTable) {
	Table filteredTable = CloneTable(eventsTable);
	Vector eventsVector = GetDataTable(filteredTable);
	Vector filteredVector = FilterVector(eventsVector, IsTodaysEvent);
	freeVector(eventsVector);
	SetDataTable(filteredTable, filteredVector);
	return EventsHandling(filteredTable);
}

int main(void) {
	windowSetup();
	Vector events;
	if (fileExists(fileEvents)) {
		events = ReadEventsFromFile(fileEvents);
	}
	else {
		events = newVector();
	}

	Vector categories;
	if (fileExists(fileCategories)) {
		categories = ReadCategoriesFromFile(fileCategories);
	}
	else {
		categories = newVector();
	}

	Menu menu = newMenu();
	Vector menuVector = getMenuOptions(menu);
	freeVector(menuVector);
	Vector tmp = arrayToVector(menuOptions, 5);
	setMenuOptions(menu, tmp);
	centerMenu(menu);
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);

	Table eventsTable = NewTable();
	freeVector(GetDataTable(eventsTable));
	SetDataTable(eventsTable, events);

	Vector header = newVector();
	for (int i = 0; i < 4; i++) {
		string tmp = copyString(eventsHeader[i]);
		addVector(header, tmp);
	}
	SetHeaderTable(eventsTable, header);
	header = newVector();
	for (int i = 0; i < 3; i++) {
		string tmp = copyString(eventsFooter[i]);
		addVector(header, tmp);
	}
	SetFooterTable(eventsTable, header);

	SetHighAttrTable(eventsTable, HIGHLIGHT_ATTRIBUTES);
	SetToStringVectorFnTable(eventsTable, EventToVector);
	SetFreeStringVectorFnTable(eventsTable, FreeEventStringVector);
	SetCompareFnTable(eventsTable, CompareEventNames);


	Table categoriesTable = NewTable();
	freeVector(GetDataTable(categoriesTable));
	SetDataTable(categoriesTable, categories);
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
		system("cls");
		if (!mainMenu(menu, &menuOption)) {
			error_msg("mainMenu");
		}
		switch (menuOption) {
		case MENU_TODAYS_EVENTS:
			if (!ShowTodaysEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}
			break;
		case MENU_CATEGORY_EVENTS:
			/*if (!ShowCategoryEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}*/
			break;
		case MENU_FUTURE_EVENTS:
			/*if (!ShowFutureEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}*/
			break;
		case MENU_PAST_EVENTS:
			/*if (!ShowPastEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}*/
			break;
		case EXIT:
			done = TRUE;
			break;
		default:
			break;
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
