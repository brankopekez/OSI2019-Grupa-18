/**
 * @file generic.h
 *
 * This interface defines those functions that apply to more than one type.
 */

#ifndef _generic_h
#define _generic_h

#include <stdarg.h>
#include "cmpfn.h"
#include "cslib.h"

#define toString(arg) toStringGeneric(sizeof arg, arg)

 /**
  * @brief Union capable of holding any of the primitive types.
  */

typedef union {
    int intRep;
    short shortRep;
    long longRep;
    float floatRep;
    double doubleRep;
    char charRep;
    bool boolRep;
    unsigned unsignedRep;
    unsigned short unsignedShortRep;
    unsigned long unsignedLongRep;
    unsigned char unsignedCharRep;
    void* pointerRep;
} GenericType;

/**
 * @brief This function type fetches an argument from an argument list into the
 * address indicated by dst.
 */

typedef void (*FetchFn)(va_list args, GenericType* dst);

/**
 * @brief This function type stores a value from the generic object into the
 * address indicated by dst.
 */

typedef void (*StoreFn)(GenericType src, void* dst);

/**
 * @brief This function type converts a generic argument to a string.
 */

typedef string(*ToStringFn)(GenericType dst);

/**
 * @brief Converts the argument to a string, if possible.
 *
 * Usage: @code str = toString(arg); @endcode
 */

string toStringGeneric(int size, ...);

/**
 * @brief Returns the size in bytes for the specified type.
 *
 * Usage: @code size = getTypeSizeForType(type); @endcode
 */

int getTypeSizeForType(string type);

/**
 * @brief Returns the comparison function appropriate to the type.
 *
 * Usage: @code fn = getCompareFnForType(type); @endcode
 */

CompareFn getCompareFnForType(string type);

/**
 * @brief Returns a function that can fetch an argument of the specified type from a va_list parameter.  The pattern for
 * calling that function is
 * @verbatim
 *    fn(args, &arg);
 * @endverbatim
 * where arg is a GenericType variable.
 *
 * Usage: @code fn = getFetchFnForType(type); @endcode
 */

FetchFn getFetchFnForType(string type);

/**
 * @brief Returns a function that can store a generic argument through a destination pointer.  The pattern for calling
 * that function is
 * @verbatim
 *    fn(src, dst);
 * @endverbatim
 * where src is a GenericType variable and dst is a pointer to the destination.
 *
 * Usage: @code fn = getStoreFnForType(type); @endcode
 */

StoreFn getStoreFnForType(string type);

/**
 * @brief Returns a function that can convert a generic value of the specified type to a string.  This function is used
 * primarily for debugging.
 *
 * Usage: @code fn = getToStringFn(type); @endcode
 */

ToStringFn getToStringFn(string type);

#endif
