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

#ifndef _MEDIA_C_VALUE_H_
#define _MEDIA_C_VALUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "fraction.h"

typedef enum {
    VTID_INVALID = 0,  // Reserved for uninitialized or error states
    VTID_FLOAT,
    VTID_DOUBLE,
    VTID_UINT32,
    VTID_INT32,
    VTID_UINT64,
    VTID_INT64,
    VTID_BOOL,
    VTID_STRING,
    VTID_POINTER,
    VTID_FRACTION
} vtid_t;

typedef union {
    float v_float;
    double v_double;
    uint32_t v_uint32;
    int32_t v_int32;
    uint64_t v_uint64;
    int64_t v_int64;
    bool v_bool;
    const char *v_string;
    void *v_ptr;
    fraction_t v_fraction;
} vtype_t;

typedef struct {
    vtype_t val;
    vtid_t tid;
} value_t;

static inline void value_print(const value_t *value, int (*print_func)(const char *, ...)) {
    switch (value->tid) {
        case VTID_FLOAT:
            print_func("%f", value->val.v_float);
            break;
        case VTID_DOUBLE:
            print_func("%lf", value->val.v_double);
            break;
        case VTID_UINT32:
            print_func("%u", value->val.v_uint32);
            break;
        case VTID_INT32:
            print_func("%d", value->val.v_int32);
            break;
        case VTID_UINT64:
            print_func("%llu", value->val.v_uint64);
            break;
        case VTID_INT64:
            print_func("%lld", value->val.v_int64);
            break;
        case VTID_BOOL:
            print_func("%s", value->val.v_bool ? "true" : "false");
            break;
        case VTID_STRING:
            print_func("%s", value->val.v_string);
            break;
        case VTID_FRACTION:
            print_func("%d/%d", value->val.v_fraction.numerator, value->val.v_fraction.denominator);
            break;
        default:
            print_func("unknown");
    }
}

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_VALUE_H_
