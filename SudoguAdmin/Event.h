/**
 * @file	Event.h.
 *
 * @brief	Declares the event interface.
 */

#ifndef _event_h
#define _event_h

#include "cslib.h"
#include <time.h>

/**
 * @struct	EventCDT*
 *
 * @brief	An Event type.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 */

typedef struct EventCDT* Event;

/**
 * @fn	Event newEvent(void);
 *
 * @brief	Creates a new event
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @returns	An Event.
 */

Event newEvent(void);

/**
 * @fn	void freeEvent(Event event);
 *
 * @brief	Free event
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 */

void freeEvent(Event event);

/**
 * @fn	string getEventName(Event event);
 *
 * @brief	Gets event name
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 *
 * @returns	The event name.
 */

string getEventName(Event event);

/**
 * @fn	void setEventName(Event event, string name);
 *
 * @brief	Sets event name
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 * @param 	name 	The name.
 */

void setEventName(Event event, string name);

/**
 * @fn	string getEventDescription(Event event);
 *
 * @brief	Gets event description
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 *
 * @returns	The event description.
 */

string getEventDescription(Event event);

/**
 * @fn	void setEventDescription(Event event, string desc);
 *
 * @brief	Sets event description
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 * @param 	desc 	The description.
 */

void setEventDescription(Event event, string desc);

/**
 * @fn	string getEventLocation(Event event);
 *
 * @brief	Gets event location
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 *
 * @returns	The event location.
 */

string getEventLocation(Event event);

/**
 * @fn	void setEventLocation(Event event, string location);
 *
 * @brief	Sets event location
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event   	The event.
 * @param 	location	The location.
 */

void setEventLocation(Event event, string location);

/**
 * @fn	string getEventCategory(Event event);
 *
 * @brief	Gets event category
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 *
 * @returns	The event category.
 */

string getEventCategory(Event event);

/**
 * @fn	void setEventCategory(Event event, string category);
 *
 * @brief	Sets event category
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event   	The event.
 * @param 	category	The category.
 */

void setEventCategory(Event event, string category);

/**
 * @fn	time_t getEventTime(Event event);
 *
 * @brief	Gets event time
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 *
 * @returns	The event time.
 */

time_t getEventTime(Event event);

/**
 * @fn	void setEventTime(Event event, time_t time);
 *
 * @brief	Sets event time
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 *
 * @param 	event	The event.
 * @param 	time 	The time.
 */

void setEventTime(Event event, time_t time);

#endif // !_event_h
