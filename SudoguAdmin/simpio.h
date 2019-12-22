/**
 * @file simpio.h
 *
 * This interface exports several functions that simplify the reading of input data.
 */

#ifndef _simpio_h
#define _simpio_h

#include "cslib.h"

 /**
  * @brief Reads a line of text from standard input and scans it as an integer.
  * If an integer cannot be scanned or if extraneous characters follow
  * the number, the user is given a chance to retry.
  *
  * Usage: @code n = getInteger(); @endcode
  */

int getInteger(void);

/**
 * @brief Reads a line of text from standard input and scans it as a
 * long.  If an integer cannot be scanned or if
 * extraneous characters follow the number, the user is given
 * a chance to retry.
 *
 * Usage: @code l = getLong(); @endcode
 */

long getLong(void);

/**
 * @brief Reads a line of text from standard input and scans it as a
 * double.  If an number cannot be scanned or if
 * extraneous characters follow the number, the user is given
 * a chance to retry.
 *
 * Usage: @code d = getReal(); @endcode
 */

double getReal(void);

/**
 * @brief Reads a line of text from standard input and returns the line
 * as a string.  The newline character that terminates the input
 * is not stored as part of the result.
 *
 * Usage: @code s = getLine(); @endcode
 */

string getLine(void);

/**
 * @brief Reads a line of text from the input file and returns the line
 * as a string.  The newline character that terminates the input
 * is not stored as part of the result.  The readLine
 * function returns NULL if infile is
 * at the end-of-file position.
 *
 * Usage: @code s = readLine(infile); @endcode
 */

string readLine(FILE* infile);

/**
 * @brief Reads an entire file into a NULL-terminated array of lines.
 * Opening and closing the file stream is the responsibility of the caller.
 *
 * Usage: @code string *array = readLinesFromStream(infile); @endcode
 */

string* readLinesFromStream(FILE* infile);

/**
 * @brief Reads an entire file into a NULL-terminated array of lines.
 * This version opens the file, reads it, and closes it at the end.  If the
 * file name is "-", the function reads from stdin.
 *
 * Usage: @code string *array = readLinesFromFile(filename); @endcode
 */

string* readLinesFromFile(string filename);

#endif
