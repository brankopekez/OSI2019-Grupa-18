#include <stdio.h>
#include <conio.h>
#include "cslib.h"
#include "map.h"
#include "iterator.h"
#include "simpio.h"
#include "cmpfn.h"
#include "strlib.h"
#include "strbuf.h"
#include "generic.h"
#include "EventCategory.h"


/** @brief	Global variable for accounts config filename */

const string fileAccounts = "accounts.txt";


/** @brief	The city config filename  */
const string fileCity = "city.txt";

/**
 * @fn	int fileToMap(string filename, Map map)
 *
 * @brief	Reads a file and maps key:value pairs.
 *
 * @author	Pynikleois
 * @date	12/22/2019
 *
 * @param 	filename	Filename of the file.
 * @param 	map			The map.
 *
 * @returns	An int. 1 on success, 0 otherwise.
 */

int fileToMap(string filename, Map map)
{
	FILE* inFile = fopen(filename, "r");
	if (!inFile) exit(EXIT_FAILURE);
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
 * @date	12/21/2019
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
 * @date	12/21/2019
 *
 * @returns	The StringBuffer containing a password.
 */

StringBuffer readPassword(void)
{
	StringBuffer password;
	password = newStringBuffer();
	int ch;
	int i = -1; // this variable counts input characters
	printf("Unesite korisnicku sifru: ");

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

/**
 * @fn	int login(string *username)
 *
 * @brief	Logins a user.
 *
 * @author	Pynikleois
 *
 * @date	12/21/2019
 *
 * @param [in,out]	username	If non-null, the username.
 *
 * @returns	An int 1 if successful, 0 otherwise.
 */

int login(string* username)
{
	Map accountsMap = newMap();
	if (fileToMap(fileAccounts, accountsMap) == 0)
	{
		freeMapFields(accountsMap);
		error("Konfiguracioni fajl %s nije ispravan.\n", fileAccounts);
	}
	printf("Unesite korisnicko ime: ");
	*username = readLine(stdin);

	StringBuffer passwordBuffer = readPassword();
	string password = getString(passwordBuffer);

	if (stringLength(*username) == 0 || stringLength(password) == 0)
	{
		printf("Pogresan unos.\n");
		freeStringBuffer(passwordBuffer);
		freeMapFields(accountsMap);
		return 0;
	}

	if (containsKeyMap(accountsMap, *username) == true)
	{
		string correctPassword = getMap(accountsMap, *username);
		if (stringCompare(password, correctPassword) == 0)
		{
			fprintf(stderr, "Welcome %s.\n", *username);
		}
		else {
			fprintf(stderr, "Wrong password.\n");
			freeStringBuffer(passwordBuffer);
			freeMapFields(accountsMap);
			return 0;
		}
	}
	else {
		fprintf(stderr, "There is no account associated with %s.\n", *username);
		freeStringBuffer(passwordBuffer);
		freeMapFields(accountsMap);
		return 0;
	}
	freeStringBuffer(passwordBuffer);
	freeMapFields(accountsMap);
	return 1;
}

/**
 * @fn	void logout(string username)
 *
 * @brief	Logout
 *
 * @author	Pynikleois
 * @date	12/22/2019
 *
 * @param 	username	The username.
 */

void logout(string username)
{
	string tmp = copyString(username);
	freeBlock(username);
	printf("Korisnik %s uspjesno odjavljen.\n", tmp);
	freeBlock(tmp);
}

int main(void)
{
	string username = NULL;
	while (true) {
		if (login(&username) != 0)
		{
			Map cityMap = newMap();
			if (fileToMap(fileCity, cityMap) != 1) {
				fprintf(stderr, "Neispravna konfiguracija parametara grada unutar datoteke %s.\n", fileCity);
				freeMapFields(cityMap);
			}
			else {
				string cityName = getMap(cityMap, "name");
				printf("Naziv grada: %s.\n", cityName);
			}
			logout(username);
		}
	}
	return 0;
}
