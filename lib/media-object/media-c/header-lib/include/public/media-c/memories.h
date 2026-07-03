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

#ifndef _MEDIA_C_MEMORIES_H_
#define _MEDIA_C_MEMORIES_H_

#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

// List of memory_t
typedef list_t memories_t;

static inline memories_t* memories_new(size_t size) {
    return list_new(size, sizeof(memory_t));
}

static inline void memories_free(memories_t* memories, void (*free_func)(memory_t*)) {
    if (memories) {
        list_free(memories, (void (*)(void*))free_func);
    }
}

static inline bool memories_add(memories_t* memories, memory_t* memory) {
    return list_add(memories, memory);
}

static inline memory_t* memories_get(const memories_t* memories, size_t index) {
    return (memory_t*)list_get(memories, index);
}

static inline size_t memories_size(const memories_t* memories) {
    return memories->size;
}

static inline size_t memories_capacity(const memories_t* memories) {
    return memories->capacity;
}

static inline bool memories_empty(const memories_t* memories) {
    return memories->size == 0;
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_MEMORIES_H_