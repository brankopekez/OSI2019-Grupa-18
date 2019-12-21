/**
 * @file cslib.h
 *
 * This interface defines a basic set of definitions that are shared among all interfaces in the package.  These basic
 * definitions include:
 *  - Declarations for several new primitive types used throughout the libraries as fundamental types.
 *  - A function for error handling.
 *  - A flexible set of functions for memory allocation.
 */

#ifndef _cslib_h
#define _cslib_h

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

 /* Section 1 -- Define new "primitive" types */

 /**
  * @brief This type defines the space of Boolean data using the constants
  * false and true.
  */

#ifdef bool
#  undef bool
#endif

#ifdef false
#  undef false
#endif

#ifdef true
#  undef true
#endif

typedef enum {
    false, true
} bool;

/**
 * @brief This type is defined to be identical with char *.
 */

typedef char* string;

/**
 * @brief This function type represents an arbitrary procedure that can be passed to an abstraction and then called back
 * from the implementation.
 */

typedef void (*proc)();

/* Section 2 -- Memory allocation */

/*
 * General notes
 * -------------
 * These functions provide a common interface for memory allocation. All functions in the library that allocate memory
 * do so using getBlock and freeBlock.  Even though the ANSI standard defines malloc and free for the same purpose,
 * using getBlock and freeBlock provides greater compatibility with non-ANSI implementations, automatic out-of-memory
 * error detection, limited type checking, and the possibility of installing a garbage-collecting allocator.
 */

 /**
  * @brief Allocates a block of memory of the given size.  If no memory is available, getBlock generates an error.
  * Usage: @code ptr = getBlock(nbytes); @endcode
  */

void* getBlock(size_t nbytes);

/**
 * @brief eturns a block with the indicated type marker.  This function is called from the newBlock and newArray macros
 * and should not be invoked by clients.
 */

void* getTypedBlock(size_t nbytes, string type);

/**
 * @brief Frees the memory associated with ptr, which must have been allocated using getBlock, newBlock, or newArray.
 * If the block appears to be in static memory or allocated by malloc, the call to freeBlock has no effect.
 * Usage: @code freeBlock(ptr); @endcode
 */

void freeBlock(void* ptr);

/**
 * Returns a string indicating the type of the block.  If the block is created using the newBlock macro, the string is
 * the type argument.  If the block is created using newArray, the string consists of the base type followed by the
 * string "[]". In all other cases, the type is returned as "?".  This string is constant and should not be freed.
 * Usage: @code type = getBlockType(ptr); @endcode
 */

string getBlockType(void* ptr);

/**
 * @brief Sets the data field inside the block to the specified value.
 * Usage: @code setBlockData(ptr, value); @endcode
 */

void setBlockData(void* ptr, void* value);

/**
 * @brief Returns the data field inside the block.
 * Usage: @code value = getBlockData(ptr); @endcode
 */

void* getBlockData(void* ptr);

/**
 * @brief Allocates enough space to hold an object of the type to which type points (note that type must be a pointer
 * type).  Note that newBlock is different from the new operator used in C++; the former takes a pointer type and the
 * latter takes the target type.
 * Usage: @code ptr = (type) newBlock(type); @endcode
 */

#define newBlock(type) ((type) getTypedBlock(sizeof *((type) NULL), #type))

 /**
  * @brief Allocates enough space to hold an array of n values of the specified type.
  * Usage: @code ptr = newArray(n, type); @endcode
  */

#define newArray(n, type) \
   ((type *) getTypedBlock((n) * sizeof(type), #type "[]"))

  /* Section 3 -- error handling */

  /**
   * @brief Generates an error string, expanding % constructions appearing in the error message string just as printf
   * does.
   *
   * Calling error prints the message to the standard error channel and exits with a status code indicating failure (as
   * given by the constant ERROR_EXIT_STATUS).
   *
   * Usage: @code error(msg, . . .); @endcode
   */

void error(string msg, ...);

/* Section 4 -- Redefine main */

#define main main_

/**
 * @brief Returns the argument count for the main program.
 * Usage: @code count = getMainArgCount(); @endcode
 */

int getMainArgCount(void);

/**
 * @brief Returns a NULL-terminated copy of the argument array passed to the main program.
 * Usage: @code array = getMainArgArray(); @endcode
 */

string* getMainArgArray(void);

#endif
