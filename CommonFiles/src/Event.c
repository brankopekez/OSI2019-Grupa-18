/**
 * @file	Event.c.
 *
 * @brief	Event interface implementation.
 */

#include "Event.h"
#include "cslib.h"
#include "strlib.h"

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
	return event;
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

int CompareEventNames(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstName = getEventName(first);
	string secondName = getEventName(second);
	return stringCompare(firstName, secondName);
}

int CompareEventLocations(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstLocation = getEventLocation(first);
	string secondLocation = getEventLocation(second);
	int res = stringCompare(firstLocation, secondLocation);
	if (res == 0) {
		res = CompareEventNames(p1, p2);
	}
	return res;
}

int CompareEventCategories(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	string firstCategory = getEventCategory(first);
	string secondCategory = getEventCategory(second);
	int res = stringCompare(firstCategory, secondCategory);
	if (res == 0) {
		res = CompareEventNames(p1, p2);
	}
	return res;
}

int CompareEventTimes(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	time_t firstTime = getEventTime(first);
	time_t secondTime = getEventTime(second);
	if (firstTime == secondTime) {
		return CompareEventNames(p1, p2);
	}
	else {
		return (firstTime < secondTime) ? -1 : +1;
	}
}

int CompareEventTimesDescending(const void* p1, const void* p2) {
	Event first = (Event) p1;
	Event second = (Event) p2;
	time_t firstTime = getEventTime(first);
	time_t secondTime = getEventTime(second);
	if (firstTime == secondTime) {
		return CompareEventNames(p1, p2);
	}
	else {
		return (firstTime > secondTime) ? -1 : +1;
	}
}
