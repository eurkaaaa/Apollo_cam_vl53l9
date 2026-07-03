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

#ifndef _MEDIA_C_CONTROL_H_
#define _MEDIA_C_CONTROL_H_

#include <stdint.h>
#include "value.h"
#include "spec.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CTRL_FLAGS_NONE = 0,
    CTRL_FLAGS_READABLE = 1,
    CTRL_FLAGS_WRITABLE = 2
} ctrl_flags_t;

typedef struct {
    const char *name;     /**< Control identifier */
    const char *nick;     /**< Nick used for UI as label */
    const char *description;
    uint32_t quark; /**< Non-zero integer which uniquely identifies the string name */
    value_t value; // Default value
    vtid_t type;
    ctrl_flags_t flags;
    spec_t spec;
} control_t;


static void control_print(const control_t* control, int (*print_func)(const char*, ...), const char* indent) {
    char new_indent[10];
    snprintf(new_indent, sizeof(new_indent), "%s\t", indent);
    print_func("%sControl:\n", indent);
    print_func("%s\tName: %s\n", indent, control->name);
    print_func("%s\tNick: %s\n", indent, control->nick);
    print_func("%s\tDescription: %s\n", indent, control->description);
    print_func("%s\tQuark: %u\n", indent, control->quark);
    print_func("%s\tValue: ", indent);
    value_print(&control->value, print_func);
    print_func("\n");
    print_func("%s\tType: %d\n", indent, control->type);
    print_func("%s\tFlags: %d\n", indent, control->flags);
    spec_print(&control->spec, print_func, new_indent);
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_CONTROL_H_