/**
 * @file iterator.h
 *
 * This interface exports a polymorphic iterator abstraction for C.
 *
 * The newIterator, stepIterator, and freeIterator functions make it possible to iterate over the elements in any
 * collection that supports iteration. In most cases, clients will simply use the foreach macro, which automatically
 * invokes these methods.  They can also be used as shown in the following standalone paradigm:
 *
 * @verbatim
 *    iterator = newIterator(collection);
 *    while (stepIterator(iterator, &element)) {
 *       . . . body of loop . . .
 *    }
 *    freeIterator(iterator);
 * @endverbatim
 *
 * The call to stepIterator advances the iterator and returns the next element using the reference parameter.  The
 * stepIterator function returns true until the elements are exhausted, after which it returns false. The freeIterator
 * function releases any storage associated with the iterator.
 */

#ifndef _iterator_h
#define _iterator_h

#include "cslib.h"

 /**
  * @brief An abstract type used to iterate over the elements of any collection.
  */

typedef struct IteratorCDT* Iterator;

/* Exported entries */

/**
 * @brief Creates a new iterator for the specified collection.
 *
 * Usage: @code iterator = newIterator(collection); @endcode
 */

Iterator newIterator(void* collection);

/**
 * @brief Advances the iterator and stores the next element through the element pointer provided as a reference
 * parameter.  The stepIterator function returns true until the elements are exhausted, after which it returns false.
 *
 * Usage: @code hasMoreElements = stepIterator(iterator, &element); @endcode
 */

bool stepIterator(Iterator iterator, void* dst);

/**
 * @brief Frees any storage associated with the iterator.
 *
 * Usage: @code freeIterator(iterator); @endcode
 */

void freeIterator(Iterator iterator);

#endif
