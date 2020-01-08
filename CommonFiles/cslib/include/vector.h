/**
 * @file vector.h
 * 
 * This interface exports an array-like indexed collection type.
 */

#ifndef _vector_h_
#define _vector_h_

#include "cslib.h"
#include "generic.h"

/**
 * @brief This type defines the abstract vector type.
 */

typedef struct VectorCDT *Vector;

/* Exported entries */

/**
 * @brief Returns an empty Vector.
 * 
 * Usage: @code vector = newVector(); @endcode
 */

Vector newVector(void);

/**
 * @brief Frees the storage associated with vector.
 * 
 * Usage: @code freeVector(vector); @endcode
 */

void freeVector(Vector vector);

/**
 * @brief Creates a vector from an array of void&nbsp;* pointers. If the array argument is NULL, this function returns
 * NULL.
 * 
 * Usage: @code vector = arrayToVector(array, n); @endcode
 */

Vector arrayToVector(void *array[], int n);

/**
 * @brief Returns a NULL-terminated array with the same elements as vector. If vector is NULL, this function returns
 * NULL.
 * 
 * Usage: @code array = vectorToArray(vector); @endcode
 */

void **vectorToArray(Vector vector);

/**
 * @brief Returns true if the vector is empty.
 * 
 * Usage: @code if (isEmpty(vector)) . . . @endcode
 */

bool isEmptyVector(Vector vector);

/**
 * @brief Returns the number of elements in the vector.
 * 
 * Usage: @code n = size(vector); @endcode
 */

int sizeVector(Vector vector);

/**
 * @brief Removes all elements from the vector.
 * 
 * Usage: @code clear(vector); @endcode
 */

void clearVector(Vector vector);

/**
 * @brief Creates a copy of the vector.  The clone function copies only the first level of the structure and does not
 * copy the individual elements.
 * 
 * Usage: @code newvec = clone(vector); @endcode
 */

Vector cloneVector(Vector vector);

/**
 * @brief Gets the element at the specified index position, raising an error if the index is out of range.
 * 
 * Usage: @code value = get(vector, index); @endcode
 */

void *getVector(Vector vector, int index);

/**
 * @brief Sets the element at the specified index position, raising an error if the index is out of range.
 * 
 * Usage: @code set(vector, index, value); @endcode
 */

void setVector(Vector vector, int index, void *value);

/**
 * @brief Adds a new value to the end of the vector.
 * 
 * Usage: @code add(vector, value); @endcode
 */

void addVector(Vector vector, void *value);

/**
 * @brief Inserts a new value before the specified index position.
 * 
 * Usage: @code insert(vector, index, value); @endcode
 */

void insert(Vector vector, int index, void *value);

/**
 * @brief Deletes the element at the specified index position.
 * 
 * Usage: @code remove(vector, index); @endcode
 */

void removeVector(Vector vector, int index);

#endif
