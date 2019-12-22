/**
 * @file cmpfn.h
 *
 * This interface exports several comparison functions for use with the other library packages.
 */

#ifndef _cmpfn_h
#define _cmpfn_h

#include "cslib.h"

 /**
  * @brief This type defines the type space of comparison functions, each of which take the addresses of their arguments
  * and return an integer from the set {-1, 0, +1} depending on whether the first argument is less than, equal to, or
  * greater than the second.
  */

typedef int (*CompareFn)(const void* p1, const void* p2);

/**
 * @brief Compares two values of type int whose addresses are supplied by the pointer arguments p1 and p2.
 */

int intCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type short whose addresses are supplied by the pointer arguments p1 and p2.
 */

int shortCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type long whose addresses are supplied by the pointer arguments p1 and p2.
 */

int longCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type char whose addresses are supplied by the pointer arguments p1 and p2.
 */

int charCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type float whose addresses are supplied by the pointer arguments p1 and p2.
 */

int floatCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type double whose addresses are supplied by the pointer arguments p1 and p2.
 */

int doubleCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type unsigned whose addresses are supplied by the pointer arguments p1 and p2.
 */

int unsignedCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type unsignedshort whose addresses are supplied by the pointer arguments p1 and p2.
 */

int unsignedShortCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type unsignedlong whose addresses are supplied by the pointer arguments p1 and p2.
 */

int unsignedLongCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type unsignedChar whose addresses are supplied by the pointer arguments p1 and p2.
 */

int unsignedCharCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type string whose addresses are supplied by the pointer arguments p1 and p2.
 */

int stringCmpFn(const void* p1, const void* p2);

/**
 * @brief Compares two values of type pointer whose addresses are supplied by the pointer arguments p1 and p2. Pointer
 * comparison is based on the numeric equivalent of the pointer.
 */

int pointerCmpFn(const void* p1, const void* p2);

#endif
