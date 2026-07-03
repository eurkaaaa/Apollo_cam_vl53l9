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

#ifndef _MEDIA_C_MEMORY_H_
#define _MEDIA_C_MEMORY_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEM_FLAG_NONE = 0x00,
    MEM_FLAG_ION = 0x01,
    MEM_FLAG_ALIGNED = 0x02
} memory_flags_t;

typedef struct {
    uint8_t* data;
    size_t offset;
    size_t size;
    size_t maxsize;
    memory_flags_t flags;
} memory_t;

static inline void memory_add_flags(memory_t* memory, memory_flags_t flags) {
    memory->flags = (memory_flags_t)((uint32_t)memory->flags | (uint32_t)flags);
}   

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_MEMORY_H_