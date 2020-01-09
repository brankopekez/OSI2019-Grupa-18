/**
 * @file	SudoguAdmin.c.
 *
 * @brief	Sudogu admin application. Main file.
 */

#define _WIN32_WINNT 0x0500

 // System headers
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <locale.h>

// CSLib header
#include "cslib.h"
#include "map.h"
#include "iterator.h"
#include "simpio.h"
#include "cmpfn.h"
#include "strlib.h"
#include "strbuf.h"
#include "generic.h"
#include "utilities.h"
#include "vector.h"

// Custom headers
#include "EventCategory.h"
#include "Event.h"
#include "Menu.h"
#include "Table.h"

/** @brief	The logo */
string logo[6] = {
	"   _____           __                 ",
	"  / ___/__  ______/ /___  ____ ___  __",
	"  \\__ \\/ / / / __  / __ \\/ __ `/ / / /",
	" ___/ / /_/ / /_/ / /_/ / /_/ / /_/ / ",
	"/____/\\__,_/\\__,_/\\____/\\__, /\\__,_/  ",
	"                       /____/         "
};

/** @brief	The array of menu options. */
string menuOptions[4] = {
	" Upravljanje događajima ",
	" Upravljanje kategorijama ",
	" Odjava ",
	" Izlaz "
};

/**
 * @enum	M_MENU
 *
 * @brief	Values that represent options inside the main menu.
 */

enum M_MENU {
	///< An enum constant representing the event handling option
	EVENT_HANDLING,
	///< An enum constant representing the category handling option
	CATEGORY_HANDLING,
	///< An enum constant representing the logout option
	LOGOUT,
	///< An enum constant representing the exit option
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
string eventsFooter[5] = {
	"ESC: Izlaz.",
	"RETURN: Detalji.",
	"DELETE: Obriši.",
	"F9: Novi događaj.",
	"F10: Sortiraj listu."
};

/** @brief	The categories header[ 1] */
string categoriesHeader[1] = {
	"Naziv kategorije događaja"
};

/** @brief	The categories footer[ 3] */
string categoriesFooter[3] = {
	"ESC: Izlaz.",
	"DELETE: Obriši kategoriju.",
	"F9: Dodaj novu kategoriju.",
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

/** @brief	The event fields */
string eventFields[5] = {
	"Naziv",
	"Lokacija",
	"Kategorija",
	"Datum i vrijeme",
	"Opis"
};

/**
 * @enum	EDIT_EVENT_MENU
 *
 * @brief	Values that represent options in edit event menu
 */

enum EDIT_EVENT_MENU {
	///< An enum constant representing the edit event name option
	EDIT_EVENT_NAME,
	///< An enum constant representing the edit event location option
	EDIT_EVENT_LOCATION,
	///< An enum constant representing the edit event category option
	EDIT_EVENT_CATEGORY,
	///< An enum constant representing the edit event time option
	EDIT_EVENT_TIME,
	///< An enum constant representing the edit event description option
	EDIT_EVENT_DESCRIPTION
};

/** @brief	The predefined categories */
string categoriesPredefined[3] = {
	"Izložbe",
	"Koncerti",
	"Promocije"
};

/** @brief	Global variable for accounts config file name */
const string fileAccounts = "accounts.txt";

/** @brief	The city config file name */
const string fileCity = "city.txt";

/** @brief	The events data file name */
const string fileEvents = "events.dat";

/** @brief	The event categories data file name */
const string fileCategories = "categories.dat";

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

/** @brief	The original color attributes of the console */
WORD wOldColorAttrs;

/** @brief	Information describing the console screen buffer */
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

/** @brief	The highlighting attributes */
WORD HIGHLIGHT_ATTRIBUTES = F_WHITE | B_BLUE;

/** @brief	The window size x coordinate */
const int windowSizeX = 121;

/** @brief	The window size y coordinate */
const int windowSizeY = 33;

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
	return password;
}

/**
 * @fn	void PrintLogo(void)
 *
 * @brief	Print logo.
 *
 * @author	Pynikleois
 * @date	1/8/2020
 */

void PrintLogo(void) {
	// 38
	COORD cursorPosition = { 0, 4 };
	SetConsoleCursorPosition(hStdout, cursorPosition);
	WORD oldAttributes = HIGHLIGHT_ATTRIBUTES;
	HIGHLIGHT_ATTRIBUTES = F_BLUE | B_WHITE;
	for (int i = 0; i < 6; i++) {
		PrintToConsoleFormatted(CENTER_ALIGN | HIGHLIGHT, logo[i]);
		advanceCursor(1);
	}
	HIGHLIGHT_ATTRIBUTES = oldAttributes;
	//advanceCursor(1);
	//PrintToConsoleFormatted(CENTER_ALIGN, "Sistem upravljanja događajima");
}

/**
 * @fn	void loginAttempt(string* inUsername, string* inPassword)
 *
 * @brief	Login attempt
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param [in,out]	inUsername	If non-null, the pointer to a variable to write the user name to.
 * @param [in,out]	inPassword	If non-null, the pointer to a variable to write the password to.
 */

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

	*inUsername = getLine();

	++cursorPosition.Y;
	SetConsoleCursorPosition(hStdout, cursorPosition);

	PrintToConsole(prompt2);

	StringBuffer passwordBuffer = NULL;
	passwordBuffer = readPassword();
	*inPassword = copyString(getString(passwordBuffer));
	freeStringBuffer(passwordBuffer);
}

/**
 * @fn	void login(void)
 *
 * @brief	Logins a user.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
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
				username = copyString(inUsername);
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
 * @fn	void logout(void)
 *
 * @brief	Logouts a user.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 */

void logout(void) {
	freeBlock(username);
}

void windowSetup(void);

/**
 * @fn	string InputEventCategory(Table categories)
 *
 * @brief	Displays the categories table and enables user to select one of them.
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	categories	The categories table.
 *
 * @returns	A string that contains name of the chosen category.
 */

string InputEventCategory(Table categories) {
	// Save the cursor info.
	CONSOLE_CURSOR_INFO oldInfo;
	GetConsoleCursorInfo(hStdout, &oldInfo);

	string categoryName;

	Table cpyTable = CloneTable(categories);
	freeVector(GetFooterTable(cpyTable));
	Vector footer = newVector();
	string tmp = copyString(" RETURN: Izaberi kategoriju.");
	addVector(footer, tmp);
	SetFooterTable(cpyTable, footer);

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
	EventCategory chosenCategory = getVector(GetDataTable(cpyTable), tableSelection);
	categoryName = getEventCategoryName(chosenCategory);
	freeBlock(tmp);
	FreeTable(cpyTable);

	// Restore the original console mode. 
	SetConsoleMode(hStdin, fdwOldMode);

	// Restore the cursor.
	SetConsoleCursorInfo(hStdout, &oldInfo);

	return categoryName;
}

/**
 * @fn	time_t InputEventTime(void)
 *
 * @brief	Input event time
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @returns	A time_t.
 */

time_t InputEventTime(void) {
	int day, month, year, hour, minute;
	PrintToConsole("\tDatum (mora biti u obliku \"dan.mjesec.godina.\"): ");
	string inputString = getLine();
	sscanf(inputString, "%d.%d.%d.", &day, &month, &year);
	//scanf("%d.%d.%d.", &day, &month, &year);
	PrintToConsole("\tVrijeme (mora biti u obliku \"sati:minuti\"): ");
	freeBlock(inputString);
	inputString = getLine();
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

/**
 * @fn	int NewEventScreen(Table events, Table categories)
 *
 * @brief	Creates a new event screen
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	events	  	The events.
 * @param 	categories	The categories.
 *
 * @returns	An int: 1 on success, 0 otherwise.
 */

int NewEventScreen(Table events, Table categories) {
	string title = "Unos novog događaja";
	system("cls");
	PrintTitle(title);
	advanceCursor(3);

	PrintToConsole("\tNaziv događaja: ");
	string eventName;
	while ((eventName = getLine()) == NULL || strlen(eventName) == 0) {
		advanceCursor(-1);
		PrintToConsole("\tNaziv događaja: ");
	}

	PrintToConsole("\tLokacija: ");
	string eventLocation;
	while ((eventLocation = getLine()) == NULL || strlen(eventLocation) == 0) {
		advanceCursor(-1);
		PrintToConsole("\tLokacija: ");
	}

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
	string eventDescription;
	while ((eventDescription = getLine()) == NULL || strlen(eventDescription) == 0) {
		advanceCursor(-1);
		PrintToConsole("\tOpis: ");
	}

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

/**
 * @fn	void SortEventsTable(Table t)
 *
 * @brief	Sorts events table.
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	t	A Table of Events to process.
 */

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

/**
 * @fn	int EditEvent(Table events, Table categories, int index)
 *
 * @brief	Edit event
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	events	  	The events.
 * @param 	categories	The categories.
 * @param 	index	  	Index of the chosen event.
 *
 * @returns	An int: 1 on success, 0 otherwise.
 */

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
			system("cls");
			PrintTitle("Unesite novo vrijeme");
			PrintStatusLine(" RETURN: Potvrdi.");
			SetCursorPositionMiddle();
			showCursor();
			inputTime = InputEventTime();
			setEventTime(event, inputTime);
			hideCursor();
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

/**
 * @fn	int ShowEventDetails(Table events, Table categories, int index)
 *
 * @brief	Shows the event details
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	events	  	The events.
 * @param 	categories	The categories.
 * @param 	index	  	Index of the chosen event.
 *
 * @returns	An int: 1 on success, 0 otherwise.
 */

int ShowEventDetails(Table events, Table categories, int index) {
	Vector data = GetDataTable(events);
	Event event = getVector(data, index);

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
					SaveEventsToFile(GetDataTable(events), fileEvents);
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
 * @fn	int EventsHandling(Table events, Table categories)
 *
 * @brief	Events handling
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	events	  	The events.
 * @param 	categories	The categories.
 *
 * @returns	An int.
 */

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

			SaveEventsToFile(GetDataTable(events), fileEvents);

			break;
		case VK_F9: // New event.
			if (isEmptyVector(GetDataTable(categories))) {
				system("cls");
				PrintToConsoleFormatted(CENTER_ALIGN | MIDDLE, "Mora postojati bar jedna kategorija događaja u evidenciji.");
				system("pause>nul");
				break;
			}
			NewEventScreen(events, categories);

			SaveEventsToFile(GetDataTable(events), fileEvents);

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
 * @fn	void NewCategoryScreen(Table table)
 *
 * @brief	Screen for creating a new category.
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	table	The categories table.
 */

void NewCategoryScreen(Table table) {
	string title = "Unos nove kategorije događaja";
	system("cls");
	PrintTitle(title);
	advanceCursor(3);
	PrintToConsole("\tNaziv nove kategorije događaja: ");

	string categoryName = getLine();

	EventCategory cat = newEventCategory();
	setEventCategoryName(cat, categoryName);

	Vector categories = GetDataTable(table);
	addVector(categories, cat);
}

/**
 * @fn	int CategoriesHandling(Table table)
 *
 * @brief	Categories handling
 *
 * @author	Pynikleois
 * @date	1/8/2020
 *
 * @param 	table	The categories table.
 *
 * @returns	An int: 1 on success, 0 otherwise.
 */

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
			SaveCategoriesToFile(GetDataTable(table), fileCategories);
			break;
		case VK_F9: // New event.
			NewCategoryScreen(table);
			SaveCategoriesToFile(GetDataTable(table), fileCategories);
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
 * @fn	int main(void)
 *
 * @brief	Main entry-point for this application
 *
 * @author	Pynikleois
 * @date	1/8/2020
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
		EventCategory tmpCat;
		string tmpString;
		for (size_t i = 0; i < 3; i++) {
			tmpCat = newEventCategory();
			tmpString = copyString(categoriesPredefined[i]);
			setEventCategoryName(tmpCat, tmpString);
			addVector(categories, tmpCat);
		}
		SaveCategoriesToFile(categories, fileCategories);
	}

	// Main menu
	Menu menu = newMenu();
	
	// Initialize menu with options
	// 
	Vector menuVector = getMenuOptions(menu);
	freeVector(menuVector);
	Vector tmp = arrayToVector(menuOptions, 4);
	setMenuOptions(menu, tmp);

	// Center the menu
	// 
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
	for (int i = 0; i < 5; i++) {
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
	for (int i = 0; i < 3; i++) {
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
		login();
		loggedOut = FALSE;
		while (!loggedOut && !done) {
			// Clear the screen
			system("cls");

			// Print city and user name
			advanceCursor(3);
			PrintToConsole("\tMjesto: %s\n", cityName);
			PrintToConsole("\tKorisnik: %s", username);

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

/**
 * @fn	void windowSetup(void)
 *
 * @brief	Window setup
 *
 * @author	Pynikleois
 * @date	1/8/2020
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
