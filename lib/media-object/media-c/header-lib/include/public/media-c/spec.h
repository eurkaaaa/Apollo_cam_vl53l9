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

#ifndef _MEDIA_C_RANGE_H_
#define _MEDIA_C_RANGE_H_

#include <stdint.h>
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    float v_float;
    int32_t v_int;
    uint32_t v_uint;
} num_type_t;

typedef struct {
    num_type_t min;
    num_type_t max;
    vtid_t tid;
} spec_t;


static void spec_print(const spec_t* spec, int (*print_func)(const char*, ...), const char* indent) {
    print_func("%sSpec: min = ", indent);
    switch (spec->tid) {
        case VTID_FLOAT:
            print_func("%f", spec->min.v_float);
            break;
        case VTID_INT32:
            print_func("%d", spec->min.v_int);
            break;
        case VTID_UINT32:
            print_func("%u", spec->min.v_uint);
            break;
        default:
            print_func("unknown");
    }
    print_func(", max = ");
    switch (spec->tid) {
        case VTID_FLOAT:
            print_func("%f", spec->max.v_float);
            break;
        case VTID_INT32:
            print_func("%d", spec->max.v_int);
            break;
        case VTID_UINT32:
            print_func("%u", spec->max.v_uint);
            break;
        default:
            print_func("unknown");
    }
    print_func("\n");
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_RANGE_H_
