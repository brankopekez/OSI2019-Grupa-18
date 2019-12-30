#ifndef _table_h
#define _table_h

#include "vector.h"
#include <Windows.h>

typedef struct TableCDT* Table;

Table NewTable(void);

void FreeTable(Table t);

Vector GetDataTable(Table t);

void SetDataTable(Table t, Vector vec);

int GetStartXTable(Table t);

void SetStartXTable(Table t, int posX);

int GetStartYTable(Table t);

void SetStartYTable(Table t, int posY);

int GetEndXTable(Table t);

void SetEndXTable(Table t, int posX);

int GetEndYTable(Table t);

void SetEndYTable(Table t, int posY);

WORD GetHighAttrTable(Table t);

void SetHighAttrTable(Table t, WORD attributes);

Vector GetColumnsTable(Table t);

void SetColumnsTable(Table t, Vector columns);

Vector GetHeaderTable(Table t);

void SetHeaderTable(Table t, Vector header);

Vector GetFooterTable(Table t);

void SetFooterTable(Table t, Vector footer);

int GetTableHeight(Table t);

int GetTableWidth(Table t);

int GetTotalTable(Table t);

int MainTable(Table table, int* selection);

#endif // !_table_h
