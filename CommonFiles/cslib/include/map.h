/**
 * @file map.h
 *
 * This interface defines a map abstraction that associates string-valued keys with values. In contrast to the HashMap
 * type defined in the hashmap.h interface, the implementation of the Map type uses a balanced binary tree, which offers
 * logarithmic performance and sorted iteration.
 *
 * In most applications, the restriction of keys to strings is easy to circumvent. The simplest strategy is to convert
 * key values to strings before inserting them into the map. A more general strategy is to use the bst.h interface
 * instead.
 */

#ifndef _map_h
#define _map_h

#include "cslib.h"
#include "generic.h"
#include "iterator.h"

 /**
  * @brief This type is the ADT used to represent the map.
  */

typedef struct MapCDT* Map;

/* Exported entries */

/**
 * @brief Allocates a new map with no entries.
 *
 * Usage: @code map = newMap(); @endcode
 */

Map newMap();

/**
 * @brief Frees the storage associated with the map.
 *
 * Usage: @code freeMap(map); @endcode
 */

void freeMap(Map map);

/**
 * @brief Returns the number of elements in the map.
 *
 * Usage: @code n = size(map); @endcode
 */

int sizeMap(Map map);

/**
 * @brief Returns true if the map has no entries.
 *
 * Usage: @code if (isEmpty(map)) . . . @endcode
 */

bool isEmptyMap(Map map);

/**
 * @brief Removes all entries from the map.
 *
 * Usage: @code clear(map); @endcode
 */

void clearMap(Map map);

/**
 * @brief Creates a copy of the map.  The clone function copies only the first level of the structure and does not copy
 * the individual elements.
 *
 * Usage: @code newmap = clone(map); @endcode
 */

Map cloneMap(Map map);

/**
 * @brief Associates key with value in the map. Each call to put supersedes any previous definition for key.
 *
 * Usage: @code put(map, key, value); @endcode
 */

void putMap(Map map, string key, void* value);

/**
 * @brief Returns the value associated with key in the map, or NULL, if no such value exists.
 *
 * Usage: @code void *value = get(map, key); @endcode
 */

void* getMap(Map map, string key);

/**
 * @brief Checks to see if the map contains the specified key.
 *
 * Usage: @code if (containsKey(map, key)) . . . @endcode
 */

bool containsKeyMap(Map map, string key);

/**
 * @brief Removes the key and its value from the map.
 *
 * Usage: @code remove(map, key); @endcode
 */

void removeMap(Map map, string key);

/**
 * @brief Iterates through the map and calls the function fn on each entry.  The callback function takes the following
 * arguments:
 *  -The key
 *  -The associated value
 *  -The data pointer
 *
 * The data pointer allows the client to pass state information to the function fn, if necessary.  If no such
 * information is required, this argument should be NULL.
 *
 * Usage: @code map(map, fn, data); @endcode
 */

void mapMap(Map map, proc fn, void* data);

#endif
