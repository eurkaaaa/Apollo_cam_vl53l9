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

#ifndef _MEDIA_C_STREAM_BUFFERS_H_
#define _MEDIA_C_STREAM_BUFFERS_H_

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "list.h"
#include "stream_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

// List of stream_buffer_t
typedef list_t stream_buffers_t;

static inline stream_buffers_t* stream_buffers_new(size_t initial_capacity) {
    return list_new(initial_capacity, sizeof(stream_buffer_t));
}

static inline void stream_buffers_free(stream_buffers_t* stream_buffers, void (*free_func)(stream_buffer_t*)) {
    list_free(stream_buffers, (void (*)(void*))free_func);
}

static inline bool stream_buffers_add(stream_buffers_t* stream_buffers, stream_buffer_t* stream_buffer) {
    return list_add(stream_buffers, stream_buffer);
}

static inline stream_buffer_t* stream_buffers_get(const stream_buffers_t* stream_buffers, size_t index) {
    return (stream_buffer_t*)list_get(stream_buffers, index);
}

static inline size_t stream_buffers_size(const stream_buffers_t* stream_buffers) {
    return stream_buffers->size;
}

static inline size_t stream_buffers_capacity(const stream_buffers_t* stream_buffers) {
    return stream_buffers->capacity;
}

static inline bool stream_buffers_empty(const stream_buffers_t* stream_buffers) {
    return stream_buffers->size == 0;
}

static inline stream_buffer_t* stream_buffers_find(const stream_buffers_t* stream_buffers, const char* name) {
    for (size_t i = 0; i < stream_buffers->size; ++i) {
        stream_buffer_t* stream_buffer = (stream_buffer_t*)list_get(stream_buffers, i);
        if (strcmp(stream_buffer->name, name) == 0) {
            return stream_buffer;
        }
    }
    return NULL;
}

static inline void stream_buffers_clear(stream_buffers_t* stream_buffers, void (*free_func)(stream_buffer_t*)) {
    list_clear(stream_buffers, (void (*)(void*))free_func);
}

static inline void stream_buffers_iterate(const stream_buffers_t* stream_buffers, void (*callback)(stream_buffer_t*)) {
    if( stream_buffers == NULL || callback == NULL) {
        return;
    }
    for (size_t i = 0; i < stream_buffers->size; ++i) {
        stream_buffer_t* stream_buffer = (stream_buffer_t*)list_get(stream_buffers, i);
        callback(stream_buffer);
    }
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_STREAM_BUFFERS_H_