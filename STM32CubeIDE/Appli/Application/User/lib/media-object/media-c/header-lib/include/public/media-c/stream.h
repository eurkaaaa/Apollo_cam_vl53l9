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

#ifndef _MEDIA_C_STREAM_H_
#define _MEDIA_C_STREAM_H_

#include "capabilities.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DIRECTION_NONE,
    DIRECTION_INPUT,
    DIRECTION_OUTPUT
} direction_t;

typedef struct {
    const char *name;
    const char *description;
    direction_t direction;
    capabilities_t* capabilities;
} stream_t;


static inline void stream_inspect(const stream_t* stream, int (*print_func)(const char*, ...), const char* indent) {
    print_func("%sName: %s\n", indent, stream->name);
    print_func("%sDescription: %s\n", indent, stream->description);
    print_func("%sDirection: %d\n", indent, stream->direction);
    capabilities_inspect(stream->capabilities, print_func, indent);
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_STREAM_H_