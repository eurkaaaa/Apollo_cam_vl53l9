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

#ifndef _MEDIA_C_LIST_H_
#define _MEDIA_C_LIST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void* items;
    size_t size;
    size_t capacity;
    size_t item_size;
} list_t;

static inline list_t* list_new(size_t initial_capacity, size_t item_size) {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if (list) {
        list->items = malloc(initial_capacity * item_size);
        if (!list->items) {
            free(list);
            return NULL;
        }
        list->size = 0;
        list->capacity = initial_capacity;
        list->item_size = item_size;
    }
    return list;
}

static inline void list_clear(list_t* list, void (*free_func)(void*)) {
    if (list && free_func) {
        for (size_t i = 0; i < list->size; ++i) {
            free_func((char*)list->items + (i * list->item_size));
        }
        list->size = 0;
    }
}

static inline void list_free(list_t* list, void (*free_func)(void*)) {
    if (list) {
        list_clear(list, free_func);
        free(list->items);
        free(list);
    }
}

static inline bool list_add(list_t* list, void* item) {
    if (list->size >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        void* new_items = realloc(list->items, new_capacity * list->item_size);
        if (!new_items) {
            return false;
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }
    memcpy((char*)list->items + (list->size * list->item_size), item, list->item_size);
    list->size++;
    return true;
}

static inline void* list_get(const list_t* list, size_t index) {
    if (index < list->size) {
        return (char*)list->items + (index * list->item_size);
    }
    return NULL;
}

static inline bool list_set(const list_t* list, size_t index, void* item) {
    if (index < list->size) {
        memcpy((char*)list->items + (index * list->item_size), item, list->item_size);
        return true;
    }
    return false;
}

static inline bool list_remove(list_t* list, size_t index, void (*free_func)(void*)) {
    if (index < list->size) {
        if (free_func) {
            free_func((char*)list->items + (index * list->item_size));
        }
        for (size_t i = index; i < list->size - 1; ++i) {
            memcpy((char*)list->items + (i * list->item_size), (char*)list->items + ((i + 1) * list->item_size),
                   list->item_size);
        }
        list->size--;
        return true;
    }
    return false;
}

static inline size_t list_size(const list_t* list) { return list->size; }

static inline size_t list_capacity(const list_t* list) { return list->capacity; }

/**
 * @brief Validate a list_t instance for basic integrity.
 *
 * This function checks that the list pointer is not NULL, the items pointer is not NULL,
 * the item size matches the expected type, the capacity is not zero, and the size does not exceed the capacity.
 * If @p must_be_non_empty is true, the list must also not be empty.
 *
 * @param list Pointer to the list_t instance.
 * @param expected_item_size The expected size of each item (e.g., sizeof(stream_t)).
 * @param must_be_non_empty If true, the list must not be empty; if false, empty lists are allowed.
 * @return MEDIA_ERROR_NONE if valid, or an appropriate MEDIA_ERROR_* code.
 */
static inline int list_check_valid(const list_t* list, size_t expected_item_size, bool must_be_non_empty) {
    if (!list) {
        return MEDIA_ERROR_INVALID_PARAMETER;
    }
    if (list->item_size == 0 || list->capacity == 0) {
        return MEDIA_ERROR_NOT_FOUND;
    }
    if (list->item_size != expected_item_size) {
        return MEDIA_ERROR_UNIMPLEMENTED;
    }
    if (list->size > list->capacity) {
        return MEDIA_ERROR_UNIMPLEMENTED;
    }
    if (list->items == NULL && list->size > 0) {
        return MEDIA_ERROR_NOT_FOUND;
    }
    if (must_be_non_empty && list->size == 0) {
        return MEDIA_ERROR_NOT_FOUND;
    }
    return MEDIA_ERROR_NONE;
}

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_LIST_H_