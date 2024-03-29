/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains trivial C standard library routines to
 * allow linking without external dependencies.
 *
 */

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>


#define MAX_TEXT_LENGTH   256

typedef struct free_block {
    size_t size;
    struct free_block* next;
} free_block;

static free_block free_block_list_head = { 0, 0 };
static const size_t align_to = 16;

__attribute__((weak)) size_t strnlen(const char *s, size_t count)
{
    size_t result = 0;
    while (count && *s) {
        result++;
        s++;
    }

    return result;
}

__attribute__((weak)) void *memcpy(void * restrict dest, const void * restrict src, size_t n)
{
    // no overlaps allowed!!
    char *cDest = (char*)dest;
    char *cSrc = (char*)src;

    // no overlaps allowed!!
    {
        if (cDest > cSrc) {
            assert((cSrc + n -1) < cDest);
        } else {
            assert((cDest + n -1) < cSrc);
        }
    }

    while (n--) {
        *cDest = *cSrc;
        cDest++; cSrc++;
    }

    return (dest);
}

__attribute__((weak)) int memcmp(const void * restrict s1, const void * restrict s2, size_t n)
{
    int result;

    const unsigned char *temp1 = s1;
    const unsigned char *temp2 = s2;

    while (n > 0 && (*temp1 == *temp2)) {
        temp1++;
        temp2++;
        n--;
    }

    if (n > 0) {
        result = *temp1 - *temp2;
    } else {
    	result = 0;
    }

    return result;
}


__attribute__((weak)) void *memset(void *dest, int c, size_t n)
{
    char *cDest = (char*)dest;

    while (n--) {
        *cDest = (char)c;
        cDest++;
    }

    return (dest);
}

__attribute__((weak)) size_t strlen (const char *s)
{
    size_t result = 0;

    while (*s) {
        s++;
        result++;
    }

    return result;
}

/***********************************************************************/

int tolower(int __c); // ctypes.h
__attribute__((weak)) int tolower(int __c)
{
    if ((__c > ('A'-1)) && (__c < ('Z'+1))) {
        __c -= ('A'-'a');
    }
    return __c;
}

__attribute__((weak)) int strcasecmp(const char *s1, const char *s2)
{
    int result = 0;

    while ((*s1 != 0) && (*s2 != 0)) {
        int c1 = tolower((unsigned char)*s1);
        int c2 = tolower((unsigned char)*s2);

        if (c1 < c2) {
            result = -1;
            break;
        } else if (c1 > c2) {
            result = 1;
            break;
        }

        s1++;
        s2++;
    }

    if (result == 0) {
        if ((*s1) && !(*s2)) { result = 1; }
        if ((*s2) && !(*s1)) { result = -1; }
    }

    return result;
}

__attribute__((weak)) char *strtok_r(char *str, const char *delim, char **saveptr)
{
    char *result = NULL;

    if (*saveptr == NULL) {
        *saveptr = str;
    }

    if (**saveptr == 0) {
        *saveptr = NULL;
        result = NULL;
    } else {
        result = *saveptr;
    }

    if (result != NULL) {
        while (**saveptr != 0) {
            const char *pDelim = delim;
            while (*pDelim != 0) {
                if (**saveptr == *pDelim) {
                    **saveptr = 0;
                    *saveptr = *saveptr + 1;

                    return result;
                }
                pDelim++;
            }
            *saveptr = *saveptr + 1;
        }
    }

    return result;
}


__attribute__((weak)) int strncmp(const char *s1, const char *s2, size_t n)
{
    int result = 0;

    assert(s1);
    assert(s2);

    for (size_t i = 0u; i < n; i++) {
        if ((*s1 == '\0') || (*s2 == '\0')) {
            break;
        } else if (*s1 < *s2) {
            result = -1;
            break;
        } else if (*s1 > *s2) {
            result = 1;
            break;
        } else /* if (*s1 == *s2) */ {
            result = 0;
        }

        s1++;
        s2++;
    }

    return result;
}

__attribute__((weak)) char* strncpy(char* destination, const char* source, size_t num)
{
    // return if no memory is allocated to the destination
    if (destination == NULL) {
        return NULL;
    }

    // take a pointer pointing to the beginning of the destination string
    char* ptr = destination;

    // copy first `num` characters of C-string pointed by source
    // into the array pointed by destination
    while (*source && num--)
    {
        *destination = *source;
        destination++;
        source++;
    }

    // null terminate destination string
    *destination = '\0';

    // the destination is returned by standard `strncpy()`
    return ptr;
}

__attribute__((weak)) int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

__attribute__((weak)) char * strcpy(char *dst0, const char *src0)
{
  char *s = dst0;
  while (*dst0++ = *src0++)
    ;
  return s;
}

__attribute__((weak)) char *strcat(char *s1, const char *s2)
{
  char *s = s1;
  while (*s1)
    s1++;
  while (*s1++ = *s2++)
    ;
  return s;
}



