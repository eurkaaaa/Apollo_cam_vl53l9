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

#ifndef _MEDIA_C_PROPERTY_H_
#define _MEDIA_C_PROPERTY_H_

#include <stdlib.h>
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    value_t value;
} property_t;

static inline void property_print(const property_t* property, int (*print_func)(const char*, ...), const char * indent) {
    print_func("%s%s: ", indent, property->name);
    value_print(&property->value, print_func);
    print_func("\n");
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_PROPERTY_H_
