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

#ifndef _MEDIA_C_BUFFER_H_
#define _MEDIA_C_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#include "memories.h"
#include "properties.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    memories_t* memories;
    uint64_t timestamp;
    uint32_t nb;
    properties_t* metadata;
} buffer_t;

static inline buffer_t* buffer_new(memories_t* memories, uint64_t timestamp, uint32_t nb, properties_t* metadata) {
    buffer_t* buffer = (buffer_t*)malloc(sizeof(buffer_t));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->memories = memories;
    buffer->timestamp = timestamp;
    buffer->nb = nb;
    buffer->metadata = metadata;
    return buffer;
}

static inline void buffer_free(buffer_t* buffer) {
    if (buffer == NULL) {
        return;
    }
    free(buffer);
}

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_BUFFER_H_