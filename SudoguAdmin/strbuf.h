/**
 * @file strbuf.h
 *
 * This interface defines a class that allows strings to grow by concatenation.
 */

#ifndef _strbuf_h
#define _strbuf_h

#include <stdarg.h>
#include "cslib.h"

 /**
  * @brief This type provides an efficient, memory-safe mechanism for strings that grow by the addition of characters.
  */

typedef struct StringBufferCDT* StringBuffer;

/**
 * @brief Creates a new string buffer that expands dynamically if needed. The buffer is initially empty.
 *
 * Usage: @code sb = newStringBuffer(); @endcode
 */

StringBuffer newStringBuffer();

/**
 * @brief Frees the storage associated with the string buffer.
 *
 * Usage: @code freeStringBuffer(sb); @endcode
 */

void freeStringBuffer(StringBuffer sb);

/**
 * @brief Appends (pushes) the character ch onto the end of the string buffer.
 *
 * Usage: @code pushChar(sb, ch); @endcode
 */

void pushChar(StringBuffer sb, char ch);

/**
 * @brief Pops and removes the last character from the string buffer.
 *
 * Usage: @code ch = popChar(sb); @endcode
 */

char popChar(StringBuffer sb);

/**
 * @brief Appends the string str to the end of the string buffer.
 *
 * Usage: @code appendString(sb, str); @endcode
 */

void appendString(StringBuffer sb, string str);

/**
 * @brief Expands a printf-style format string and arguments onto the end of the string buffer.
 *
 * Usage: @code sbprintf(sb, format, ...); @endcode
 */

void sbprintf(StringBuffer sb, string format, ...);

/**
 * @brief Returns true if the string buffer is empty.
 *
 * Usage: @code if (isEmpty(sb)) . . . @endcode
 */

bool isEmptyStringBuffer(StringBuffer vec);

/**
 * @brief Returns the number of characters in the string buffer.
 *
 * Usage: @code n = size(sb); @endcode
 */

int sizeStringBuffer(StringBuffer vector);

/**
 * @brief Removes all characters from the string buffer.
 *
 * Usage: @code clear(sb); @endcode
 */

void clearStringBuffer(StringBuffer sb);

/**
 * @brief Returns the string stored inside the buffer.  Clients must copy this string if they need to retain it.
 *
 * Usage: @code str = getString(sb); @endcode
 */

string getString(StringBuffer sb);

/**
 * @brief Returns a character count that will be sufficient to hold the result of printing format with arguments
 * substituted from the args list.  This bound is guaranteed to be adequate but is not necessarily tight.
 *
 * Usage: @code capacity = printfCapacity(format, args); @endcode
 */

int printfCapacity(string format, va_list args);

/**
 * @brief Works like sbprintf except that the list is an argument list as in stdarg.h.  The capacity argument is the
 * required capacity, as returned by printfCapacity.
 *
 * Usage: @code sbFormat(sb, capacity, format, list); @endcode
 */

void sbFormat(StringBuffer sb, int capacity, string format, va_list args);

#endif
