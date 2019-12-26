/**
 * @file	Event.c.
 *
 * @brief	Event interface implementation.
 */

#include "Event.h"
#include "cslib.h"

/**
 * @struct	EventCDT
 *
 * @brief	An event data structure.
 *
 * @author	Pynikleois
 * @date	26.12.2019.
 */

struct EventCDT
{
	string name;
	string description;
	string location;
	string category;
	time_t time;
};

Event newEvent(void)
{
	Event event;
	event = newBlock(Event);
	event->name = NULL;
	event->description = NULL;
	event->location = NULL;
	event->category = NULL;
	event->time = 0;
}

void freeEvent(Event event)
{
	freeBlock(event);
}

string getEventName(Event event)
{
	return event->name;
}

void setEventName(Event event, string name)
{
	event->name = name;
}

string getEventDescription(Event event)
{
	return event->description;
}

void setEventDescription(Event event, string desc)
{
	event->description = desc;
}

string getEventLocation(Event event)
{
	return event->location;
}

void setEventLocation(Event event, string location)
{
	event->location = location;
}

string getEventCategory(Event event)
{
	return event->category;
}

void setEventCategory(Event event, string category)
{
	event->category = category;
}

time_t getEventTime(Event event)
{
	return event->time;
}

void setEventTime(Event event, time_t time)
{
	event->time = time;
}
