/*******************************************************************************
 *
 * Copyright (c) 2025 STMicroelectronics - All Rights Reserved
 *
 * License terms: STMicroelectronics Proprietary in accordance with licensing
 * terms at SLA0106
 *
 * STMicroelectronics confidential
 * Reproduction and Communication of this document is strictly prohibited unless
 * specifically authorized in writing by STMicroelectronics.
 *
 *******************************************************************************
 */

#ifndef _MEDIA_C_STRINGS_H_
#define _MEDIA_C_STRINGS_H_

#include <stddef.h>
#include <stdint.h>
#include "list.h"


#ifdef __cplusplus
extern "C" {
#endif

// List of memory_t
typedef list_t strings_t;

static inline strings_t* strings_new(size_t size) {
    return list_new(size, sizeof(char*));
}

static inline void strings_free(strings_t* strings, void (*free_func)(char*)) {
    if (strings) {
        list_free(strings, (void (*)(void*))free_func);
    }
}

static inline bool strings_add(strings_t* strings, char* string) {
    return list_add(strings, &string);
}

static inline char* strings_get(const strings_t* strings, size_t index) {
    return *(char**)list_get(strings, index);
}   

static inline size_t strings_size(const strings_t* strings) {
    return strings->size;
}

static inline size_t strings_capacity(const strings_t* strings) {
    return strings->capacity;
}

static inline bool strings_empty(const strings_t* strings) {
    return strings->size == 0;
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_STRINGS_H_