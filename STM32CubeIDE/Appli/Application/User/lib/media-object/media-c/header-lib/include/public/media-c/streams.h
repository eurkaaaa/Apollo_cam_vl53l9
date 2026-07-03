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

#ifndef _MEDIA_C_STREAMs_H_
#define _MEDIA_C_STREAMs_H_

#include "list.h"
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// List of stream_t
typedef list_t streams_t;

static inline streams_t* streams_new(size_t initial_capacity) {
    return list_new(initial_capacity, sizeof(stream_t));
}

static inline void streams_free(streams_t* streams, void (*free_func)(stream_t*)) {
    list_free(streams, (void (*)(void*))free_func);
}

static inline bool streams_add(streams_t* streams, stream_t* stream) {
    return list_add(streams, stream);
}

static inline stream_t* streams_get(const streams_t* streams, size_t index) {
    return (stream_t*)list_get(streams, index);
}

static inline size_t streams_size(const streams_t* streams) {
    return streams->size;
}

static inline size_t streams_capacity(const streams_t* streams) {
    return streams->capacity;
}

static inline bool streams_empty(const streams_t* streams) {
    return streams->size == 0;
}

static inline stream_t* streams_find(const streams_t* streams, const char* name) {
    for (size_t i = 0; i < streams->size; ++i) {
        stream_t* stream = (stream_t*)list_get(streams, i);
        if (strcmp(stream->name, name) == 0) {
            return stream;
        }
    }
    return NULL;
}

static inline void streams_clear(streams_t* streams, void (*free_func)(stream_t*)) {
    list_clear(streams, (void (*)(void*))free_func);
}

static inline void streams_inspect(const streams_t* streams, int (*print_func)(const char*, ...)) {
    print_func("Streams:\n");
    for (size_t i = 0; i < streams->size; ++i) {
        const stream_t* stream = streams_get(streams, i);
        stream_inspect(stream, print_func, "\t");
    }
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_STREAMs_H_