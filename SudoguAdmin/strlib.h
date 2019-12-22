/**
 * @file strlib.h
 *
 * This interface defines a general library for dynamically allocated strings.  The major differences between
 * traditional C strings and those defined using this interface are:
 *
 *
 * The strlib.h interface takes care of memory allocation, ensuring that there is sufficient space to hold the result of
 * each string operation.
 *
 * Clients of the strlib.h interface are expected to treat all strings as immutable and refrain from writing into the
 * character array.
 *
 * Cautionary note:
 *
 * Although this interface provides an extremely convenient abstraction for working with strings, it is not appropriate
 * for all applications. In this interface, the functions that return string values (such as concat and substring) do so
 * by allocating new memory. Over time, a program that uses this package will consume increasing amounts of memory and
 * eventually exhaust the available supply. If you are writing a program that runs for a short time and stops, the fact
 * that the package consumes memory is not a problem. If, however, you are writing an application that must run for an
 * extended period of time, using this package requires that you make some provision for freeing any allocated storage.
 */

#ifndef _strlib_h
#define _strlib_h

#include <stdarg.h>
#include "cslib.h"
#include "generic.h"

 /* Section 1 -- Basic string operations */

 /**
  * @brief Concatenates two strings by joining them end to end.  For example, concat("ABC", "DE") returns the string
  * "ABCDE".
  *
  * Usage: @code s = concat(s1, s2); @endcode
  */

string concat(string s1, string s2);

/**
 * @brief Returns the character at position i in the string s.  This function is included in the library to make the
 * type string a true abstract type in the sense that all of the necessary operations can be invoked using functions.
 * Calling charAt(s, i) is like selecting s[i], except that charAt checks to see if i is within the range of legal index
 * positions, which extend from 0 to stringLength(s).  Calling charAt(s, stringLength(s))</code returns the null
 * character at the end of the string.
 *
 * Usage: @code ch = charAt(s, i); @endcode
 */

char charAt(string s, int i);

/**
 * @brief Returns a copy of the substring of s consisting of the characters between index positions p1 and p2,
 * inclusive.  The following special cases apply:
 *  -If p1 is less than 0, it is assumed to be 0.
 *  -If p2 is greater than or equal to the length of the string, p2 is set to stringLength(s) - 1.
 *  -If p2 &lt; p1, substring returns the empty string.
 *
 * Usage: @code t = substring(s, p1, p2); @endcode
 */

string substring(string s, int p1, int p2);

/**
 * @brief Takes a single character and returns a one-character string consisting of that character.  The charToString
 * function is useful, for example, if you need to concatenate a string and a character.  Since concat requires two
 * strings, you must first convert the character into a string.
 *
 * Usage: @code s = charToString(ch); @endcode
 */

string charToString(char ch);

/**
 * @brief Returns the length of the string s.
 *
 * Usage: @code len = stringLength(s); @endcode
 */

int stringLength(string s);

/**
 * @brief Copies the string s into dynamically allocated storage and returns the new string.  This function is not
 * ordinarily required when this package is used on its own but is often necessary when you are working with more than
 * one string package.
 *
 * Usage: @code newstr = copyString(s); @endcode
 */

string copyString(string s);

/* Section 2 -- String comparison functions */

/**
 * @brief Returns true if the strings s1 and s2 are equal.  For the strings to be considered equal, every character in
 * one string must precisely match the corresponding character in the other. Uppercase and lowercase characters are
 * considered to be different.
 *
 * Usage: @code if (stringEqual(s1, s2)) ... @endcode
 */

bool stringEqual(string s1, string s2);

/**
 * @brief Returns true if the strings s1 and s2 are equal, ignoring differences in case.
 *
 * Usage: @code if (stringEqualIgnoreCase(s1, s2)) ... @endcode
 */

bool stringEqualIgnoreCase(string s1, string s2);

/**
 * @brief Returns -1 if string s1 comes before s2 in lexicographic order, 0 if they are equal, and +1 if s1 comes after
 * s2.
 *
 * Usage: @code if (stringCompare(s1, s2) < 0) ... @endcode
 */

int stringCompare(string s1, string s2);

/**
 * @brief Returns true if s1 starts with s2.
 *
 * Usage: @code if (startsWith(s1, s2)) ... @endcode
 */

bool startsWith(string s1, string s2);

/**
 * @brief Returns true if s1 ends with s2.
 *
 * Usage: @code if (endsWith(s1, s2)) ... @endcode
 */

bool endsWith(string s1, string s2);

/* Section 3 -- Search functions */

/**
 * @brief Searches for the character ch beginning at position start in the string text and returns the first index at
 * which it appears or -1 if no match is found.
 *
 * Usage: @code p = findChar(ch, text, start); @endcode
 */

int findChar(char ch, string text, int start);

/**
 * @brief Searches for the string str beginning at position start in the string text and returns the first index at
 * which it appears or -1 if no match is found.
 *
 * Usage: @code p = findString(str, text, start); @endcode
 */

int findString(string str, string text, int start);

/**
 * @brief Returns the last index of ch in text, or -1 if the search value does not appear.
 *
 * Usage: @code p = findLastChar(ch, text); @endcode
 */

int findLastChar(char ch, string text);

/**
 * @brief Returns the last index of str in text, or -1 if the search value does not appear.
 *
 * Usage: @code p = findLastString(str, text); @endcode
 */

int findLastString(string str, string text);

/* Section 4 -- Conversion functions */

/**
 * @brief Returns a new string with all alphabetic characters converted to lowercase.
 *
 * Usage: @code s = toLowerCase(s); @endcode
 */

string toLowerCase(string s);

/**
 * @brief Returns a new string with all alphabetic characters converted to uppercase.
 *
 * Usage: @code s = toUpperCase(s); @endcode
 */

string toUpperCase(string s);

/**
 * @brief Converts an integer into the corresponding string of digits. For example, integerToString(123) returns "123"
 * as a string.
 *
 * Usage: @code s = integerToString(n); @endcode
 */

string integerToString(int n);

/**
 * @brief Converts a string of digits into an integer.  If the string is not a legal integer or contains extraneous
 * characters, stringToInteger signals an error condition.
 *
 * Usage: @code n = stringToInteger(s); @endcode
 */

int stringToInteger(string s);

/**
 * @brief Converts a floating-point number into the corresponding string form.  For example, calling realToString(23.45)
 * returns "23.45".  The conversion is the same as that used for "%G" format in printf.
 *
 * Usage: @code string  s = realToString(d); @endcode
 */

string realToString(double d);

/**
 * @brief Converts a string representing a real number into its corresponding value.  If the string is not a legal
 * floating-point number or if it contains extraneous characters, stringToReal signals an error condition.
 *
 * Usage: @code d = stringToReal(s); @endcode
 */

double stringToReal(string s);

/* Section 5 - Miscellaneous functions */

/**
 * @brief Returns a new string after removing any whitespace characters from the beginning and end of the argument.
 *
 * Usage: @code trimmed = trim(str); @endcode
 */

string trim(string str);

/**
 * @brief Returns a quoted string that can be read by the C parser. This string includes double quotes around the entire
 * value, and uses standard escape sequences to indicate special characters.
 *
 * Usage: @code quoted = quoteString(str); @endcode
 */

string quoteString(string str);

/**
 * @brief Returns the length of a NULL-terminated string array.
 *
 * Usage: @code len = stringArrayLength(array); @endcode
 */

int stringArrayLength(string array[]);

/**
 * @brief Finds a string in a NULL-terminated string array and returns the first index at which the string appears, or
 * -1 if it is not found.
 *
 * Usage: @code index = searchStringArray(str, array); @endcode
 */

int searchStringArray(string str, string array[]);

#endif
