#include <stdio.h>
#include "cslib.h"
#include "map.h"
#include "iterator.h"
#include "simpio.h"

const char* fileAccounts = "accounts.txt";

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

int main(void)
{
    Map accountsMap = newMap();
    if (validateAccounts(fileAccounts, accountsMap) == 0)
    {
        freeAccountsMap(accountsMap);
        error("Konfiguracioni fajl %s nije ispravan.", fileAccounts);
    }
    else {
        fprintf(stderr, "Konfiguracioni fajl %s je ispravan.", fileAccounts);
    }
	return 0;
}
