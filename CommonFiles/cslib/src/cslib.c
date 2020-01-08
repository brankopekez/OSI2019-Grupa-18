/**
 * @file cslib.c
 *
 * This file implements the general C library package. See the interface description in cslib.h for details.
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include "cslib.h"

 /* Constants */

#define MAX_MESSAGE 500
#define ERROR_EXIT_STATUS 1

/* Section 2 -- Memory allocation */

/**
 * @brief The smallest structure within the allocator is called a block and consists of the actual allocated memory
 * returned to the client, plus the following header information:
 * @verbatim
 *                  +---------------------+
 *     base addr -> | password/chain link |
 *                  +---------------------+
 *                  |  block type string  |
 *                  +---------------------+
 *                  | size of client data |
 *                  +---------------------+
 *                  | client data pointer |
 *                  +---------------------+
 *   client addr -> |          .          |
 *                  |          .          |
 *                  |          .          |
 *                  +---------------------+
 * @endverbatim
 *
 * The password is a special value unlikely to appear as a data value.
 */

typedef struct BlockHeader {
    union {
        size_t _password;              /**< A special value unlikely to appear as a data value     */
        struct BlockHeader* _chain;    /**< Pointer to the next link in a chain of blocks.         */
    } block_union;                     /**< Union that represents either a password or a link.     */
    char* type;                        /**< String containing the type of block data.              */
    size_t size;                       /**< Size of client data.                                   */
    void* data;                        /**< Client data pointer.                                   */
} BlockHeader;

#define password block_union._password
#define chain block_union._chain
#define PASSWORD 314159265L // pi (3.14159265) times 100 million

/* Memory allocation implementation */

void* getBlock(size_t nbytes) {
    return getTypedBlock(nbytes, "?");
}

void* getTypedBlock(size_t nbytes, string type) {
    BlockHeader* base;

    base = (BlockHeader*)malloc(nbytes + sizeof(BlockHeader));
    if (base == NULL) {
        error("No memory available");
    }
    base->password = PASSWORD;
    base->type = type;
    base->size = nbytes;
    base->data = NULL;
    return (void*)((char*)base + sizeof(BlockHeader));
}

void freeBlock(void* ptr) {
    BlockHeader* base;

    base = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if (base->password == PASSWORD) {
        base->password = 0;
        free(base);
    }
}

string getBlockType(void* ptr) {
    BlockHeader* base;

    base = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    return (base->password == PASSWORD) ? base->type : "?";
}

void setBlockData(void* ptr, void* value) {
    BlockHeader* base;

    base = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if (base->password != PASSWORD) {
        error("setBlockData: Block has not been allocated");
    }
    base->data = value;
}

void* getBlockData(void* ptr) {
    BlockHeader* base;

    base = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if (base->password != PASSWORD) {
        error("getBlockData: Block has not been allocated");
    }
    return base->data;
}

/* Section 3 -- error handling */

void error(string msg, ...) {
    va_list args;
    char errbuf[MAX_MESSAGE + 1];
    string errmsg;
    size_t errlen;

    va_start(args, msg);
    vsnprintf(errbuf, MAX_MESSAGE, msg, args);
    va_end(args);
    errlen = strlen(errbuf);
    if (errlen >= MAX_MESSAGE) {
        fprintf(stderr, "error: %s\n", "error message too long");
        exit(ERROR_EXIT_STATUS);
    }
    errmsg = (string)malloc(errlen + 1);
    if (errmsg == NULL) {
        errmsg = "No memory available";
    }
    else {
        strcpy(errmsg, errbuf);
    }
    fprintf(stderr, "error: %s\n", errmsg);
    exit(ERROR_EXIT_STATUS);
}

/* Section 4 -- Redefine main */

#undef main

extern int main_(int argc, string argv[]);

/**
 * @brief Holds a number of arguments of a function main.
 */
static int argCount;

/**
 * @brief Holds a copy of all arguments of a function main.
 */
static string* argArray;

int main(int argc, string argv[]) {
    int i;

    argCount = argc;
    argArray = newArray(argc + 1, string);
    for (i = 0; i < argc; i++) {
        argArray[i] = argv[i];
    }
    argArray[argc] = NULL;
    main_(argc, argv);
    freeBlock(argArray);
}

int getMainArgCount(void) {
    return argCount;
}

string* getMainArgArray(void) {
    return argArray;
}
