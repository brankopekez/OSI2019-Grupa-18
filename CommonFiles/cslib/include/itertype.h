/**
 * @file itertype.h
 *
 * This interface allows package designers to export new collection types that support iteration. Clients of those
 * collection types who merely want to use iterators do not need to import this interface.
 *
 * In order to create a collection type that supports iteration, the implementor of the type must first allocate space
 * at the beginning of the concrete record for an IteratorHeader containing a function pointer that allows newIterator
 * to make a new iterator for that collection type.  The implementor must initialize this header in the constructor
 * function by calling enableIteration, which takes the collection pointer and a type-specific callback to create the
 * iterator.
 *
 * There are two different strategies for creating the iterator in each type.  The general strategy (which is useful for
 * iterators that are not really collections, such as scanners that return a series of tokens) is to call
 * newStepIterator, which takes a pointer to a stepIterator function specific for that type.  The second strategy
 * creates the entire list of values at the beginning.  Such iterators are created by calling newListIterator to create
 * an empty iterator and the calling addToIteratorList for each element.
 *
 * Elements in the iterator may be sorted or unsorted depending on the comparison function passed to newListIterator.
 */

#ifndef _itertype_h
#define _itertype_h

#include "cslib.h"
#include "iterator.h"
#include "cmpfn.h"

 /**
  * @brief Represents the class of functions that create new iterators.
  */

typedef Iterator(*NewIteratorFn)(void* collection);

/**
 * @brief Represents the class of functions that step from one element to the next.
 */

typedef bool (*StepIteratorFn)(Iterator iterator, void* dst);

/**
 * @brief This structure must appear at the beginning of any concrete structure that supports iteration.
 */

typedef struct {
    unsigned long password;
    NewIteratorFn newFn;
} IteratorHeader;

/**
 * @brief Enables iteration for the collection.  The type-specific function pointer newFn is used to initialize an
 * iterator for this type.
 *
 * Usage: @code enableIteration(collection, newFn); @endcode
 */

void enableIteration(void* collection, NewIteratorFn newFn);

/**
 * @brief Creates a new iterator that uses an implementation-specific step function.
 *
 * Usage: @code iterator = newStepIterator(size, stepFn); @endcode
 */

Iterator newStepIterator(int size, StepIteratorFn stepFn);

/**
 * @brief Creates a new iterator with an empty iterator list.  The first argument is the size of the element type,
 * which makes it possible for the package to allocate storage for values of that type. The cmpFn is the comparison
 * function used to sort the iterator elements.  If cmpFn is the constant NULL, the elements are entered in the order in
 * which addToIteratorList is called.
 *
 * Usage: @code iterator = newListIterator(size, cmpFn); @endcode
 */

Iterator newListIterator(int size, CompareFn cmpFn);

/**
 * @brief Adds a new element to a list-style iterator.  The order of the elements is determined by the comparison
 * function specified when the iterator was created.
 *
 * Usage: @code addToIteratorList(iterator, dst); @endcode
 */

void addToIteratorList(Iterator iterator, void* dst);

/**
 * @brief Sets the collection to which an iterator belongs.
 *
 * Usage: @code setCollection(iterator, collection); @endcode
 */

void setCollection(Iterator iterator, void* collection);

/**
 * @brief Returns the collection to which an iterator belongs.
 *
 * Usage: @code collection = getCollection(iterator); @endcode
 */

void* getCollection(Iterator iterator);

/**
 * @brief Sets a data pointer in the iterator for use by clients.
 *
 * Usage: @code setIteratorData(iterator, data); @endcode
 */

void setIteratorData(Iterator iterator, void* data);

/**
 * @brief Returns the client data pointer from the iterator.
 *
 * Usage: @code data = getIteratorData(iterator); @endcode
 */

void* getIteratorData(Iterator iterator);

#endif
