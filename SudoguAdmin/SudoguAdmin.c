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


/** @brief	Global variable for accounts config filename */

const char* fileAccounts = "accounts.txt";

/**
 * @fn	int validateAccounts(string filenameAccounts, Map accountsMap)
 *
 * @brief	Validates the accounts
 *
 * @author	Pynikleois
 *
 * @date	12/21/2019
 *
 * @param 	filenameAccounts	The accounts config filename.
 * @param 	accountsMap			The accounts map.
 *
 * @returns	An int 1 if successful, 0 otherwise.
 */

int validateAccounts(string filenameAccounts, Map accountsMap)
{
	FILE* inFile = fopen(filenameAccounts, "r");
	if (!inFile) exit(EXIT_FAILURE);
	string line;
	while ((line = readLine(inFile)) != NULL) {
		int delimPos = findChar(':', line, 0);

		string username = substring(line, 0, delimPos - 1);
		string password = substring(line, delimPos + 1, stringLength(line) - 1);
		if (stringLength(username) == 0 || stringLength(password) == 0) {
			return 0;
		}
		else {
			if (containsKeyMap(accountsMap, username) == true) {
				return 0;
			}
			putMap(accountsMap, username, password);
		}
		freeBlock(line);
	}
	fclose(inFile);
	return 1;
}

/**
 * @fn	void freeAccountsMap(Map accountsMap)
 *
 * @brief	Frees accounts map and all of its allocated fields.
 *
 * @author	Pynikleois
 * @date	12/21/2019
 *
 * @param 	accountsMap	The accounts map.
 */

void freeAccountsMap(Map accountsMap)
{
	string key;
	Iterator iterator = newIterator(accountsMap);
	while (stepIterator(iterator, &key)) {
		freeBlock(getMap(accountsMap, key));
	}
	freeIterator(iterator);
	iterator = newIterator(accountsMap);
	while (stepIterator(iterator, &key)) {
		freeBlock(key);
	}
	freeIterator(iterator);

	freeMap(accountsMap);
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
	if (validateAccounts((string)fileAccounts, accountsMap) == 0)
	{
		freeAccountsMap(accountsMap);
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
		freeAccountsMap(accountsMap);
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
			freeAccountsMap(accountsMap);
			return 0;
		}
	}
	else {
		fprintf(stderr, "There is no account associated with %s.\n", *username);
		freeStringBuffer(passwordBuffer);
		freeAccountsMap(accountsMap);
		return 0;
	}
	freeStringBuffer(passwordBuffer);
	freeAccountsMap(accountsMap);
	return 1;
}

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
			logout(username);
		}
	}
	return 0;
}
