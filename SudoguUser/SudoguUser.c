/**
 * @file	SudoguUser.c.
 *
 * @brief	Sudogu user application. Main file.
 */

 // System headers
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <locale.h>
#include <io.h>

// Custom headers
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
#include "../SudoguAdmin/Menu.h"
#include "../SudoguAdmin/utilities.h"
#include "../SudoguAdmin/Table.h"

/** @brief	The array of menu options. */
string menuOptions[5] = {
	" Pregled današnjih događaja ",
	" Pregled događaja određene kategorije ",
	" Pregled svih budućih događaja ",
	" Pregled događaja koji su prošli ",
	" Izlaz "
};

/**
 * @enum	M_MENU
 *
 * @brief	Values that represent options in main menu.
 */

enum M_MENU {
	///< An enum constant representing the todays events option in the main menu.
	MENU_TODAYS_EVENTS,
	///< An enum constant representing the category events option in the main menu.
	MENU_CATEGORY_EVENTS,
	///< An enum constant representing the future events option in the main menu.
	MENU_FUTURE_EVENTS,
	///< An enum constant representing the past events option in the main menu.
	MENU_PAST_EVENTS,
	///< An enum constant representing the exit option in the main menu.
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

/** @brief	The categories header[ 1] */
string categoriesHeader[1] = {
	"Naziv kategorije događaja"
};

/** @brief	The categories footer[ 2] */
string categoriesFooter[2] = {
	"ESC: Nazad.",
	"RETURN: Izaberi kategoriju."
};

/**
 * @enum	EVENTS_HEADER_OPTIONS
 *
 * @brief	Values that represent events table column names (when sorting).
 */

enum EVENTS_HEADER_OPTIONS {
	///< An enum constant representing the events name column
	EVENTS_HEADER_NAME,
	///< An enum constant representing the events location column
	EVENTS_HEADER_LOCATION,
	///< An enum constant representing the events category column
	EVENTS_HEADER_CATEGORY,
	///< An enum constant representing the events time column
	EVENTS_HEADER_TIME
};

/** @brief	The city config file name */
const string fileCity = "city.txt";

/** @brief	The events data file name */
const string fileEvents = "events.dat";

/** @brief	The event categories data file name */
const string fileCategories = "categories.dat";

/** @brief	Name of the program (used on error) */
const string programName = "SudoguUser";

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

/** @brief	The highlight attributes */
WORD HIGHLIGHT_ATTRIBUTES = F_RED | B_WHITE | COMMON_LVB_REVERSE_VIDEO;

/** @brief	The window size x coordinate */
const int windowSizeX = 121;

/** @brief	The window size y coordinate */
const int windowSizeY = 33;

void windowSetup(void);

/**
 * @fn	string InputEventCategory(Table categories, int* tableSelection)
 *
 * @brief	Displays the categories table and enables user to select one of them.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 		  	categories	  	The categories table.
 * @param [in,out]	tableSelection	If non-null, the current table selection.
 *
 * @returns	A string that contains name of the chosen category.
 */

string InputEventCategory(Table categories, int* tableSelection) {
	// Save the cursor info.
	CONSOLE_CURSOR_INFO oldInfo;
	GetConsoleCursorInfo(hStdout, &oldInfo);

	string categoryName;

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

	// Key code that was registered inside the table.
	WORD registeredKeyCode;

	while (!done) {
		if (!MainTable(categories, tableSelection, &registeredKeyCode)) {
			// Restore the original console mode. 
			SetConsoleMode(hStdin, fdwOldMode);
			// Restore the cursor.
			SetConsoleCursorInfo(hStdout, &oldInfo);
			error_msg("InputEventCategory::MainTable");
		}
		switch (registeredKeyCode) {
		case VK_ESCAPE:
			*tableSelection = -1;
			done = TRUE;
			break;
		case VK_RETURN:
			if (!isEmptyVector(GetDataTable(categories))) {
				done = TRUE;
			}
			break;
		default:
			break;
		}
	}
	if (*tableSelection != -1) {
		EventCategory chosenCategory = getVector(GetDataTable(categories), *tableSelection);
		categoryName = getEventCategoryName(chosenCategory);
	}
	else {
		categoryName = NULL;
	}

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	// Restore the cursor.
	SetConsoleCursorInfo(hStdout, &oldInfo);

	return categoryName;
}

/**
 * @fn	void SortEventsTable(Table t)
 *
 * @brief	Sort events table according to the chosen column.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	t	A Table of events to process.
 */

void SortEventsTable(Table t) {
	// Table header contents
	Vector header = GetHeaderTable(t);

	// Menu for holding column names as options
	Menu menu = newMenu();

	// Already allocated vector for menu options 
	Vector menuVector = getMenuOptions(menu);

	// Free already allocated vector because we will be using another one
	freeVector(menuVector);

	// Clone header vector into the menu options vector of the new menu
	setMenuOptions(menu, cloneVector(header));

	// Center new menu
	centerMenu(menu);

	// Choose attributes for highlighting inside the menu
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);

	// Selected option inside the main menu.
	int menuOption;

	// Clear the screen
	system("cls");

	// Print the title
	PrintTitle("Odaberite naziv kolone po kojoj želite da sortirate podatke");

	// Call mainMenu function to display menu and return a selected option
	if (!mainMenu(menu, &menuOption)) {
		error_msg("mainMenu");
	}

	// Register and process the option
	switch (menuOption) {
	case EVENTS_HEADER_NAME: // Sorting by event names
		SetCompareFnTable(t, CompareEventNames);
		break;
	case EVENTS_HEADER_LOCATION: // Sorting by event locations
		SetCompareFnTable(t, CompareEventLocations);
		break;
	case EVENTS_HEADER_CATEGORY: // Sorting by event categories
		SetCompareFnTable(t, CompareEventCategories);
		break;
	case EVENTS_HEADER_TIME: // Sorting by event times
		SetCompareFnTable(t, CompareEventTimes);
		break;
	default:
		break;
	}

}

/**
 * @fn	int ShowEventDetails(Table events, int index)
 *
 * @brief	Shows the event details
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	events	The events table.
 * @param 	index 	Zero-based index of the chosen event.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

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
		system("pause>nul");
		if (WaitForSingleObject(hStdin, INFINITE) == WAIT_OBJECT_0)  /* if kbhit */
		{
			/* Get the input event */
			ReadConsoleInput(hStdin, &inputEvent, 1, &cRead);

			/* Only respond to key events */
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

/**
 * @fn	int EventsHandling(Table events)
 *
 * @brief	Events handling that involves displaying the events table and proccessing the user
 *  input.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	events	The events table.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

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

/**
 * @fn	int IsTodaysEvent(const void* p1, const void* p2)
 *
 * @brief	Check if the event is happening today.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	p1	The pointer to the event.
 * @param 	p2	The ponter to the struct tm that is initialized to current time.
 *
 * @returns	An int. 0 if the event is happening today; 1 otherwise.
 */

int IsTodaysEvent(const void* p1, const void* p2) {
	Event event = (Event) p1;
	struct tm* tmNow = (struct tm*) p2;
	time_t eventTime = getEventTime(event);
	struct tm tmEvent;
	tmEvent = *localtime(&eventTime);

	if (tmNow->tm_year == tmEvent.tm_year
		&& tmNow->tm_yday == tmEvent.tm_yday) {
		return 0;
	}
	else {
		return 1;
	}
}

/**
 * @fn	int IsFutureEvent(const void* p1, const void* p2)
 *
 * @brief	Is future event
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	p1	The pointer to the event.
 * @param 	p2	The pointer to the time_t variable initialized to current time.
 *
 * @returns	An int.
 */

int IsFutureEvent(const void* p1, const void* p2) {
	Event event = (Event) p1;
	time_t now = *(time_t*) p2;
	time_t eventTime = getEventTime(event);

	if (difftime(eventTime, now) > 0) {
		return 0;
	}
	else {
		return 1;
	}
}

/**
 * @fn	int IsPastEvent(const void* p1, const void* p2)
 *
 * @brief	Checks if the event is in the past.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	p1	The pointer to the event.
 * @param 	p2	The pointer to the time_t variable initialized to current time.
 *
 * @returns	An int.
 */

int IsPastEvent(const void* p1, const void* p2) {
	Event event = (Event) p1;
	time_t now = *(time_t*) p2;
	time_t eventTime = getEventTime(event);

	if (difftime(eventTime, now) < 0) {
		return 0;
	}
	else {
		return 1;
	}
}

/**
 * @fn	int IsCategoryEvent(const void* p1, const void* p2)
 *
 * @brief	Is category event
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	p1	The pointer to the event.
 * @param 	p2	The pointer to the string that holds category name to compare to.
 *
 * @returns	An int.
 */

int IsCategoryEvent(const void* p1, const void* p2) {
	Event event = (Event) p1;
	string categoryToCompareTo = (string) p2;
	string eventCategory = getEventCategory(event);
	if (stringCompare(eventCategory, categoryToCompareTo) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

/**
 * @fn	Vector FilterVector(Vector old, CompareFn cmpFn, void* toCmpTo)
 *
 * @brief	Filter vector
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 		  	old	   	The vector to filter.
 * @param 		  	cmpFn  	The compare function.
 * @param [in,out]	toCmpTo	If non-null, second argument of the compare function. Argument to
 * 							compare to.
 *
 * @returns	A filtered Vector.
 */

Vector FilterVector(Vector old, CompareFn cmpFn, void* toCmpTo) {
	Vector new = newVector();
	for (int i = 0; i < sizeVector(old); i++) {
		void* record = getVector(old, i);
		if (cmpFn(record, toCmpTo) == 0) {
			addVector(new, record);
		}
	}
	return new;
}

/**
 * @fn	int ShowTodaysEvents(Table eventsTable)
 *
 * @brief	Shows the todays events
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	eventsTable	The events table.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

int ShowTodaysEvents(Table eventsTable) {
	Table filteredTable = CloneTable(eventsTable);
	Vector eventsVector = GetDataTable(filteredTable);

	time_t now;
	time(&now);
	struct tm tmNow;
	tmNow = *localtime(&now);

	Vector filteredVector = FilterVector(eventsVector, IsTodaysEvent, &tmNow);
	freeVector(eventsVector);
	SetDataTable(filteredTable, filteredVector);
	int res = EventsHandling(filteredTable);
	FreeTable(filteredTable);
	return res;
}

/**
 * @fn	int ShowFutureEvents(Table eventsTable)
 *
 * @brief	Shows the future events
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	eventsTable	The events table.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

int ShowFutureEvents(Table eventsTable) {
	Table filteredTable = CloneTable(eventsTable);
	Vector eventsVector = GetDataTable(filteredTable);

	time_t now;
	time(&now);

	Vector filteredVector = FilterVector(eventsVector, IsFutureEvent, &now);
	freeVector(eventsVector);
	SetDataTable(filteredTable, filteredVector);
	int res = EventsHandling(filteredTable);
	FreeTable(filteredTable);
	return res;
}

/**
 * @fn	int ShowPastEvents(Table eventsTable)
 *
 * @brief	Shows the past events
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	eventsTable	The events table.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

int ShowPastEvents(Table eventsTable) {
	Table filteredTable = CloneTable(eventsTable);
	Vector eventsVector = GetDataTable(filteredTable);

	time_t now;
	time(&now);

	Vector filteredVector = FilterVector(eventsVector, IsPastEvent, &now);
	freeVector(eventsVector);
	SetDataTable(filteredTable, filteredVector);
	int res = EventsHandling(filteredTable);
	FreeTable(filteredTable);
	return res;
}

/**
 * @fn	int ShowCategoryEvents(Table eventsTable, Table categoriesTable)
 *
 * @brief	Shows the events of the chosen category.
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @param 	eventsTable	   	The events table.
 * @param 	categoriesTable	The categories table.
 *
 * @returns	An int. 1 on success; 0 otherwise.
 */

int ShowCategoryEvents(Table eventsTable, Table categoriesTable) {
	Table filteredTable = CloneTable(eventsTable);
	Vector eventsVector = GetDataTable(filteredTable);

	// Variable for registering end.
	BOOL done = FALSE;

	// Selected index inside the table.
	int tableSelection = 0;

	// Returning value.
	int returnValue = 1;

	while (!done) {
		string selectedCategory = InputEventCategory(categoriesTable, &tableSelection);
		if (selectedCategory != NULL && tableSelection != -1) {
			Vector filteredVector = FilterVector(eventsVector, IsCategoryEvent, selectedCategory);
			SetDataTable(filteredTable, filteredVector);
			returnValue = EventsHandling(filteredTable);
			freeVector(filteredVector);
		}
		else {
			done = TRUE;
		}
	}

	SetDataTable(filteredTable, eventsVector);
	FreeTable(filteredTable);
	return returnValue;
}

/**
 * @fn	int main(void)
 *
 * @brief	Main entry-point for this application
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 *
 * @returns	Exit-code for the process - 0 for success, else an error code.
 */

int main(void) {

	// Setup the window
	windowSetup();

	// Vector to hold all events
	Vector events;

	// Check if the events data file exists, and read it if it does. 
	// Otherwise, create new vector.
	// 
	if (fileExists(fileEvents)) {
		events = ReadEventsFromFile(fileEvents);
	}
	else {
		events = newVector();
	}

	// Vector to hold all categories
	Vector categories;

	// Check if the categories data file exists, and read it if it does. 
	// Otherwise, create new vector.
	// 
	if (fileExists(fileCategories)) {
		categories = ReadCategoriesFromFile(fileCategories);
	}
	else {
		categories = newVector();
	}

	// Main menu
	Menu menu = newMenu();

	// Initialize menu with options
	// 
	Vector menuVector = getMenuOptions(menu);
	freeVector(menuVector);
	Vector tmp = arrayToVector(menuOptions, 5);
	setMenuOptions(menu, tmp);

	// Set spacing per line in the new menu and center it.
	// 
	setSpacingPerLine(menu, 35);
	centerMenu(menu);

	// Set main menu attributes for highlighting.
	// 
	setHighlightAttributes(menu, HIGHLIGHT_ATTRIBUTES);
	
	// Table for all events
	// 
	Table eventsTable = NewTable();
	freeVector(GetDataTable(eventsTable));
	SetDataTable(eventsTable, events);

	// Set the header and the footer of the new table.
	// 
	Vector tmpVector = newVector();
	for (int i = 0; i < 4; i++) {
		string tmp = copyString(eventsHeader[i]);
		addVector(tmpVector, tmp);
	}
	SetHeaderTable(eventsTable, tmpVector);
	tmpVector = newVector();
	for (int i = 0; i < 3; i++) {
		string tmp = copyString(eventsFooter[i]);
		addVector(tmpVector, tmp);
	}
	SetFooterTable(eventsTable, tmpVector);

	// Set attributes for highlighting inside the table.
	// 
	SetHighAttrTable(eventsTable, HIGHLIGHT_ATTRIBUTES);

	// Set function for converting the event to a vector of strings
	// 
	SetToStringVectorFnTable(eventsTable, EventToVector);

	// Set function for deallocating the vector of strings
	// 
	SetFreeStringVectorFnTable(eventsTable, FreeEventStringVector);

	// Set funciton for comparing the events.
	// Used for sorting the table before drawing it.
	// 
	SetCompareFnTable(eventsTable, CompareEventTimesDescending);

	// Table for all categories
	// 
	Table categoriesTable = NewTable();
	freeVector(GetDataTable(categoriesTable));
	SetDataTable(categoriesTable, categories);
	
	// Set attributes for highlighting inside the table.
	// 
	SetHighAttrTable(categoriesTable, HIGHLIGHT_ATTRIBUTES);
	
	// Set function for converting the event category to a vector of strings
	// 
	SetToStringVectorFnTable(categoriesTable, EventCategoryToVector);
	
	// Set function for deallocating the vector of strings
	// 
	SetFreeStringVectorFnTable(categoriesTable, FreeEventCategoryStringVector);
	
	// Set funciton for comparing the event categories.
	// Used for sorting the table before drawing it.
	// 
	SetCompareFnTable(categoriesTable, CompareEventCategoryName);

	// Set the header and the footer of the new table.
	// 
	tmpVector = newVector();
	for (int i = 0; i < 1; i++) {
		string tmp = copyString(categoriesHeader[i]);
		addVector(tmpVector, tmp);
	}
	SetHeaderTable(categoriesTable, tmpVector);
	tmpVector = newVector();
	for (int i = 0; i < 2; i++) {
		string tmp = copyString(categoriesFooter[i]);
		addVector(tmpVector, tmp);
	}
	SetFooterTable(categoriesTable, tmpVector);

	// City name, read from a config file
	string cityName = NULL;
	Map cityMap = newMap();
	if (fileToMap(fileCity, cityMap) != 1) {
		freeMapFields(cityMap);
		error_msg("Neispravna konfiguracija parametara grada unutar datoteke %s.\n", fileCity);
	}
	else {
		cityName = getMap(cityMap, "name");
	}

	// Variable for registering end.
	BOOL done = FALSE;

	// Variable for registering when a user has logged out.
	BOOL loggedOut = FALSE;

	// Selected option inside the main menu.
	int menuOption;

	while (!done) {
		// Clear the screen
		system("cls");

		// Print city name
		advanceCursor(3);
		PrintToConsole("\tMjesto: %s\n", cityName);

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
			if (!ShowCategoryEvents(eventsTable, categoriesTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}
			break;
		case MENU_FUTURE_EVENTS:
			if (!ShowFutureEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}
			break;
		case MENU_PAST_EVENTS:
			if (!ShowPastEvents(eventsTable)) {
				error_msg("Nije moguce prikazati dogadjaje.");
			}
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

/**
 * @fn	void windowSetup(void)
 *
 * @brief	Window setup
 *
 * @author	Pynikleois
 * @date	8.1.2020.
 */

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
