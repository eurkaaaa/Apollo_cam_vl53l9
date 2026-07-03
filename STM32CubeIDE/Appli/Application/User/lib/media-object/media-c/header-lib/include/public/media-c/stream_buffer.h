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

#ifndef _MEDIA_C_STREAM_BUFFER_H_
#define _MEDIA_C_STREAM_BUFFER_H_

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    buffer_t buffer;
} stream_buffer_t;

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_STREAM_BUFFER_H_