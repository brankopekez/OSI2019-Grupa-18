/**
 * @file generic.c
 *
 * This file implements the generic.h interface.
 */

#include <stdio.h>
#include <stdarg.h>
#include "cmpfn.h"
#include "cslib.h"
#include "generic.h"
#include "strbuf.h"
#include "strlib.h"

 /* Private function prototypes */
static void intFetchFn(va_list args, GenericType* dst);
static void shortFetchFn(va_list args, GenericType* dst);
static void longFetchFn(va_list args, GenericType* dst);
static void floatFetchFn(va_list args, GenericType* dst);
static void doubleFetchFn(va_list args, GenericType* dst);
static void charFetchFn(va_list args, GenericType* dst);
static void unsignedFetchFn(va_list args, GenericType* dst);
static void unsignedShortFetchFn(va_list args, GenericType* dst);
static void unsignedLongFetchFn(va_list args, GenericType* dst);
static void unsignedCharFetchFn(va_list args, GenericType* dst);
static void pointerFetchFn(va_list args, GenericType* dst);
static void intStoreFn(GenericType src, void* dst);
static void shortStoreFn(GenericType src, void* dst);
static void longStoreFn(GenericType src, void* dst);
static void floatStoreFn(GenericType src, void* dst);
static void doubleStoreFn(GenericType src, void* dst);
static void charStoreFn(GenericType src, void* dst);
static void unsignedStoreFn(GenericType src, void* dst);
static void unsignedShortStoreFn(GenericType src, void* dst);
static void unsignedLongStoreFn(GenericType src, void* dst);
static void unsignedCharStoreFn(GenericType src, void* dst);
static void pointerStoreFn(GenericType src, void* dst);
static string intToStringFn(GenericType any);
static string shortToStringFn(GenericType any);
static string longToStringFn(GenericType any);
static string floatToStringFn(GenericType any);
static string doubleToStringFn(GenericType any);
static string charToStringFn(GenericType any);
static string boolToStringFn(GenericType any);
static string unsignedToStringFn(GenericType any);
static string unsignedShortToStringFn(GenericType any);
static string unsignedLongToStringFn(GenericType any);
static string unsignedCharToStringFn(GenericType any);
static string stringToStringFn(GenericType any);
static string pointerToStringFn(GenericType any);

/* Constants */

#define BUFSIZE 40

/* Exported entries */

string toStringGeneric(int size, ...) {
    va_list args;
    string type;
    void* arg;

    va_start(args, size);
    arg = va_arg(args, void*);
    va_end(args);
    type = getBlockType(arg);
    if (endsWith(type, "StringBuffer")) {
        return copyString(getString((StringBuffer)arg));
    }
    else {
        error("toString: Illegal argument type");
    }
}

int getTypeSizeForType(string type) {
    if (stringEqual(type, "int")) return sizeof(int);
    if (stringEqual(type, "short")) return sizeof(short);
    if (stringEqual(type, "long")) return sizeof(long);
    if (stringEqual(type, "float")) return sizeof(float);
    if (stringEqual(type, "double")) return sizeof(double);
    if (stringEqual(type, "char")) return sizeof(char);
    if (stringEqual(type, "bool")) return sizeof(bool);
    if (stringEqual(type, "unsigned")) return sizeof(unsigned);
    if (stringEqual(type, "unsigned short")) return sizeof(unsigned short);
    if (stringEqual(type, "unsigned long")) return sizeof(unsigned long);
    if (stringEqual(type, "unsigned char")) return sizeof(unsigned char);
    if (stringEqual(type, "char *")) return sizeof(char*);
    if (stringEqual(type, "string")) return sizeof(string);
    return sizeof(void*);
}

CompareFn getCompareFnForType(string type) {
    if (stringEqual(type, "int")) return intCmpFn;
    if (stringEqual(type, "short")) return shortCmpFn;
    if (stringEqual(type, "long")) return longCmpFn;
    if (stringEqual(type, "float")) return floatCmpFn;
    if (stringEqual(type, "double")) return doubleCmpFn;
    if (stringEqual(type, "char")) return charCmpFn;
    if (stringEqual(type, "bool")) return intCmpFn;
    if (stringEqual(type, "unsigned")) return unsignedCmpFn;
    if (stringEqual(type, "unsigned short")) return unsignedShortCmpFn;
    if (stringEqual(type, "unsigned long")) return unsignedLongCmpFn;
    if (stringEqual(type, "unsigned char")) return unsignedCharCmpFn;
    if (stringEqual(type, "char *")) return stringCmpFn;
    if (stringEqual(type, "string")) return stringCmpFn;
    return pointerCmpFn;
}

FetchFn getFetchFnForType(string type) {
    if (stringEqual(type, "int")) return intFetchFn;
    if (stringEqual(type, "short")) return shortFetchFn;
    if (stringEqual(type, "long")) return longFetchFn;
    if (stringEqual(type, "float")) return floatFetchFn;
    if (stringEqual(type, "double")) return doubleFetchFn;
    if (stringEqual(type, "char")) return charFetchFn;
    if (stringEqual(type, "bool")) return intFetchFn;
    if (stringEqual(type, "unsigned")) return unsignedFetchFn;
    if (stringEqual(type, "unsigned short")) return unsignedShortFetchFn;
    if (stringEqual(type, "unsigned long")) return unsignedLongFetchFn;
    if (stringEqual(type, "unsigned char")) return unsignedCharFetchFn;
    return pointerFetchFn;
}

StoreFn getStoreFnForType(string type) {
    if (stringEqual(type, "int")) return intStoreFn;
    if (stringEqual(type, "short")) return shortStoreFn;
    if (stringEqual(type, "long")) return longStoreFn;
    if (stringEqual(type, "float")) return floatStoreFn;
    if (stringEqual(type, "double")) return doubleStoreFn;
    if (stringEqual(type, "char")) return charStoreFn;
    if (stringEqual(type, "bool")) return intStoreFn;
    if (stringEqual(type, "unsigned")) return unsignedStoreFn;
    if (stringEqual(type, "unsigned short")) return unsignedShortStoreFn;
    if (stringEqual(type, "unsigned long")) return unsignedLongStoreFn;
    if (stringEqual(type, "unsigned char")) return unsignedCharStoreFn;
    return pointerStoreFn;
}

ToStringFn getToStringFn(string type) {
    if (stringEqual(type, "int")) return intToStringFn;
    if (stringEqual(type, "short")) return shortToStringFn;
    if (stringEqual(type, "long")) return longToStringFn;
    if (stringEqual(type, "float")) return floatToStringFn;
    if (stringEqual(type, "double")) return doubleToStringFn;
    if (stringEqual(type, "char")) return charToStringFn;
    if (stringEqual(type, "bool")) return boolToStringFn;
    if (stringEqual(type, "unsigned")) return unsignedToStringFn;
    if (stringEqual(type, "unsigned short")) return unsignedShortToStringFn;
    if (stringEqual(type, "unsigned long")) return unsignedLongToStringFn;
    if (stringEqual(type, "unsigned char")) return unsignedCharToStringFn;
    if (stringEqual(type, "char *")) return stringToStringFn;
    if (stringEqual(type, "string")) return stringToStringFn;
    return pointerToStringFn;
}

/* Private functions */

static void intFetchFn(va_list args, GenericType* dst) {
    dst->intRep = va_arg(args, int);
}

static void shortFetchFn(va_list args, GenericType* dst) {
    dst->shortRep = (short)va_arg(args, int);
}

static void longFetchFn(va_list args, GenericType* dst) {
    dst->longRep = va_arg(args, long);
}

static void floatFetchFn(va_list args, GenericType* dst) {
    dst->floatRep = (float)va_arg(args, double);
}

static void doubleFetchFn(va_list args, GenericType* dst) {
    dst->doubleRep = va_arg(args, double);
}

static void charFetchFn(va_list args, GenericType* dst) {
    dst->charRep = (char)va_arg(args, int);
}

static void unsignedFetchFn(va_list args, GenericType* dst) {
    dst->unsignedRep = va_arg(args, unsigned);
}

static void unsignedShortFetchFn(va_list args, GenericType* dst) {
    dst->unsignedShortRep = (unsigned short)va_arg(args, int);
}

static void unsignedLongFetchFn(va_list args, GenericType* dst) {
    dst->unsignedLongRep = va_arg(args, unsigned long);
}

static void unsignedCharFetchFn(va_list args, GenericType* dst) {
    dst->unsignedCharRep = (unsigned char)va_arg(args, int);
}

static void pointerFetchFn(va_list args, GenericType* dst) {
    dst->pointerRep = va_arg(args, void*);
}

static void intStoreFn(GenericType src, void* dst) {
    *((int*)dst) = src.intRep;
}

static void shortStoreFn(GenericType src, void* dst) {
    *((short*)dst) = src.shortRep;
}

static void longStoreFn(GenericType src, void* dst) {
    *((long*)dst) = src.longRep;
}

static void floatStoreFn(GenericType src, void* dst) {
    *((float*)dst) = src.floatRep;
}

static void doubleStoreFn(GenericType src, void* dst) {
    *((double*)dst) = src.doubleRep;
}

static void charStoreFn(GenericType src, void* dst) {
    *((char*)dst) = src.charRep;
}

static void unsignedStoreFn(GenericType src, void* dst) {
    *((unsigned*)dst) = src.unsignedRep;
}

static void unsignedShortStoreFn(GenericType src, void* dst) {
    *((unsigned short*)dst) = src.unsignedShortRep;
}

static void unsignedLongStoreFn(GenericType src, void* dst) {
    *((unsigned long*)dst) = src.unsignedLongRep;
}

static void unsignedCharStoreFn(GenericType src, void* dst) {
    *((unsigned char*)dst) = src.unsignedCharRep;
}

static void pointerStoreFn(GenericType src, void* dst) {
    *((void**)dst) = src.pointerRep;
}

static string intToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%d", any.intRep);
    return copyString(buffer);
}

static string shortToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%d", any.shortRep);
    return copyString(buffer);
}

static string longToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%ldL", any.longRep);
    return copyString(buffer);
}

static string floatToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%g", any.floatRep);
    return copyString(buffer);
}

static string doubleToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%g", any.doubleRep);
    return copyString(buffer);
}

static string charToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%c", any.charRep);
    return copyString(buffer);
}

static string boolToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%s", (any.boolRep) ? "true" : "false");
    return copyString(buffer);
}

static string unsignedToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%u", any.unsignedRep);
    return copyString(buffer);
}

static string unsignedShortToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%u", any.unsignedShortRep);
    return copyString(buffer);
}

static string unsignedLongToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%luL", any.unsignedLongRep);
    return copyString(buffer);
}

static string unsignedCharToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "%c", any.unsignedCharRep);
    return copyString(buffer);
}

static string stringToStringFn(GenericType any) {
    return copyString((string)any.pointerRep);
}

static string pointerToStringFn(GenericType any) {
    char buffer[BUFSIZE];

    sprintf(buffer, "0x%lX", (long)any.pointerRep);
    return copyString(buffer);
}
