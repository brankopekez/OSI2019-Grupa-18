/**
 * @file	EventCategory.h.
 *
 * @brief	Declares the event category interface.
 */

#ifndef _event_category_h
#define _event_category_h

#include "cslib.h"

/**
 * @typedef	EventCategoryCDT*
 *
 * @brief	An event category type.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 */

typedef struct EventCategoryCDT* EventCategory;

/**
 * @fn	EventCategory newEventCategory(void);
 *
 * @brief	Creates a new event category.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @returns	An EventCategory.
 */

EventCategory newEventCategory(void);

/**
 * @fn	void freeEventCategory(EventCategory category);
 *
 * @brief	Frees already created event category.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	category	The category.
 */

void freeEventCategory(EventCategory category);

/**
 * @fn	string getEventCategoryName(EventCategory category);
 *
 * @brief	Gets event category name.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	category	The category.
 *
 * @returns	The event category name.
 */

string getEventCategoryName(EventCategory category);

/**
 * @fn	void setEventCategoryName(EventCategory category, string name);
 *
 * @brief	Sets event category name.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	category	The category.
 * @param 	name		The name.
 */

void setEventCategoryName(EventCategory category, string name);

int CompareEventCategoryName(const void* p1, const void* p2);

#endif // !_event_category_h
