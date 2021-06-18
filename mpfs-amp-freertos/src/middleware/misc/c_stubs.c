/*******************************************************************************
 * Copyright 2019-2021 Microchip Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains trivial C standard library routines for the
 * Hart Software Services, to allow it link without external dependencies.
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

__attribute__((weak)) void * sbrk (int  incr)
{
    extern char __heap_start;//set by linker
    extern char __heap_end;//set by linker

    static char *heap_end;		/* Previous end of heap or 0 if none */
    char        *prev_heap_end;

    if (0 == heap_end) {
        heap_end = &__heap_start;			/* Initialize first time round */
    }

    prev_heap_end  = heap_end;
    heap_end      += incr;

    if( heap_end < (&__heap_end)) {

    } else {
        return (char*)-1;
    }
    return (void *) prev_heap_end;

}

__attribute__((weak)) void* malloc(size_t size) {

    size = (size + sizeof(size_t) + (align_to - 1)) & ~ (align_to - 1);
    free_block* block = free_block_list_head.next;
    free_block** head = &(free_block_list_head.next);
    while (block != 0) {
        if (block->size >= size) {
            *head = block->next;
            return ((char*)block) + sizeof(size_t);
        }
        head = &(block->next);
        block = block->next;
    }

    block = (free_block*)sbrk(size);
    block->size = size;

    return ((char*)block) + sizeof(size_t);
}

__attribute__((weak)) void free(void* ptr) {
    
    free_block* block = (free_block*)(((char*)ptr) - sizeof(size_t));
    block->next = free_block_list_head.next;
    free_block_list_head.next = block;
}

__attribute__((weak)) int rand(void)
{
    return 0;
}


