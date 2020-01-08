/**
 * @file	EventCategory.c.
 *
 * @brief	Event category implementation.
 */

#include "EventCategory.h"
#include "cslib.h"
#include "strlib.h"

/**
 * @struct	EventCategoryCDT
 *
 * @brief	An event category structure.
 *
 * @author	Pynikleois
 * @date	12/26/2019
 */

struct EventCategoryCDT
{
	/** @brief	The name of the category. */
	string name;
};

EventCategory newEventCategory(void)
{
	EventCategory category;
	category = newBlock(EventCategory);
	category->name = NULL;
    return category;
}

void freeEventCategory(EventCategory category)
{
	freeBlock(category);
}

string getEventCategoryName(EventCategory category)
{
	return category->name;
}

void setEventCategoryName(EventCategory category, string name)
{
	category->name = name;
}

int CompareEventCategoryName(const void* p1, const void* p2) {
	EventCategory first = (EventCategory) p1;
	EventCategory second = (EventCategory) p2;
	string firstName = getEventCategoryName(first);
	string secondName = getEventCategoryName(second);
	return stringCompare(firstName, secondName);
}
