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

#ifndef _MEDIA_C_PROPERTIES_H_
#define _MEDIA_C_PROPERTIES_H_

#include <stdlib.h>
#include <string.h>

#include "property.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

// List of property_t
typedef list_t properties_t;

static inline properties_t* properties_new(size_t size) {
    return list_new(size, sizeof(property_t));
}

static inline void properties_free(properties_t* properties, void (*free_func)(property_t*)) {
    if (properties) {
        list_free(properties, (void (*)(void*))free_func);
    }
}

static inline bool properties_add(properties_t* properties, property_t* property) {
    return list_add(properties, property);
}

static inline property_t* properties_get(const properties_t* properties, size_t index) {
    return (property_t*)list_get(properties, index);
}

static inline size_t properties_size(const properties_t* properties) {
    return properties->size;
}

static inline size_t properties_capacity(const properties_t* properties) {
    return properties->capacity;
}

static inline bool properties_empty(const properties_t* properties) {
    return properties->size == 0;
}

static inline property_t* properties_find(const properties_t* properties, const char* name) {
    for (size_t i = 0; i < properties->size; ++i) {
        property_t* property = (property_t*)list_get(properties, i);
        if (strcmp(property->name, name) == 0) {
            return property;
        }
    }
    return NULL;
}

static inline void properties_clear(properties_t* properties, void (*free_func)(property_t*)) {
    list_clear(properties, (void (*)(void*))free_func);
}

static inline properties_t* properties_duplicate(const properties_t* original) {
    if (original == NULL) {
        return NULL;
    }

    properties_t* duplicate = properties_new(properties_capacity(original));
    if (duplicate == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < properties_size(original); ++i) {
        property_t* original_property = properties_get(original, i);
        if (original_property == NULL) {
            properties_free(duplicate, NULL);
            return NULL;
        }
        if (!properties_add(duplicate, original_property)) {
            properties_free(duplicate, NULL);
            return NULL;
        }
    }
    return duplicate;
}

static inline void properties_iterate(const properties_t* properties, void (*callback)(property_t*)) {
    if (properties == NULL || callback == NULL) {
        return;
    }
    for (size_t i = 0; i < properties->size; ++i) {
        property_t* property = (property_t*)list_get(properties, i);
        callback(property);
    }
}

static inline void properties_inspect(const properties_t* properties, int (*print_func)(const char*, ...), const char* indent) {
    print_func("%sProperties:\n", indent);
    char new_indent[10];
    snprintf(new_indent, sizeof(new_indent), "%s\t", indent);
    for (size_t i = 0; i < properties->size; ++i) {
        const property_t* property = properties_get(properties, i);
        property_print(property, print_func, new_indent);
    }
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_PROPERTIES_H_
