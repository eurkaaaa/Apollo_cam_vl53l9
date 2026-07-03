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

#ifndef _MEDIA_C_CONTROLS_H_
#define _MEDIA_C_CONTROLS_H_

#include <stddef.h>
#include <stdint.h>
#include "control.h"
#include "list.h"


#ifdef __cplusplus
extern "C" {
#endif

// List of memory_t
typedef list_t controls_t;

static inline controls_t* controls_new(size_t size) {
    return list_new(size, sizeof(control_t));
}

static inline void controls_free(controls_t* controls, void (*free_func)(control_t*)) {
    if (controls) {
        list_free(controls, (void (*)(void*))free_func);
    }
}

static inline bool controls_add(controls_t* controls, control_t* control) {
    return list_add(controls, control);
}

static inline control_t* controls_get(const controls_t* controls, size_t index) {
    return (control_t*)list_get(controls, index);
}

static inline size_t controls_size(const controls_t* controls) {
    return controls->size;
}

static inline size_t controls_capacity(const controls_t* controls) {
    return controls->capacity;
}

static inline bool controls_empty(const controls_t* controls) {
    return controls->size == 0;
}

static inline control_t* controls_find(const controls_t* controls, const char* name) {
    for (size_t i = 0; i < controls->size; i++) {
        control_t* control = controls_get(controls, i);
        if (strcmp(control->name, name) == 0) {
            return control;
        }
    }
    return NULL;
}

static inline void controls_iterate(controls_t* controls, void (*callback)(control_t*)) {
    if(!controls || !callback) {
        return;
    }
    for (size_t i = 0; i < controls->size; i++) {
        control_t* control = controls_get(controls, i);
        callback(control);
    }
}

static inline void controls_inspect(const controls_t* controls, int (*print_func)(const char*, ...)) {
    print_func("Controls:\n");
    for (size_t i = 0; i < controls->size; ++i) {
        const control_t* control = controls_get(controls, i);
        control_print(control, print_func, "\t");
    }
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_CONTROLS_H_