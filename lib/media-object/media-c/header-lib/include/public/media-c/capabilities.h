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

#ifndef _MEDIA_C_CAPABILITIES_H_
#define _MEDIA_C_CAPABILITIES_H_

#include "properties.h"

#ifdef __cplusplus
extern "C" {
#endif

// List of properties_t
typedef list_t capabilities_t;

/**
 * @brief Create a new capabilities list with an initial capacity.
 *
 * @param initial_capacity The initial capacity of the list.
 * @return A pointer to the newly created capabilities list.
 */
static inline capabilities_t* capabilities_new(size_t initial_capacity) {
    return list_new(initial_capacity, sizeof(properties_t*));
}

/**
 * @brief Create a new capabilities list with a single property.
 *
 * @param properties A pointer to the properties to be added to the list.
 * @return A pointer to the newly created capabilities list.
 */
static inline capabilities_t* capabilities_new_simple(properties_t** properties) {
    capabilities_t* capabilities = list_new(1, sizeof(properties_t*));
    list_add(capabilities, properties);
    return capabilities;
}

/**
 * @brief Free the memory allocated for the capabilities list.
 *
 * @param capabilities A pointer to the capabilities list to be freed.
 * @param free_func A function pointer to free the properties.
 */
static inline void capabilities_free(const capabilities_t* capabilities, void (*free_func)(properties_t**)) {
    list_free((list_t*)capabilities, (void (*)(void*))free_func);
}

/**
 * @brief Add a property to the capabilities list.
 *
 * @param capabilities A pointer to the capabilities list.
 * @param properties A pointer to the properties to be added.
 * @return true if the property was added successfully, false otherwise.
 */
static inline bool capabilities_add(capabilities_t* capabilities, properties_t** properties) {
    return list_add(capabilities, properties);
}

/**
 * @brief Get a property from the capabilities list by index.
 *
 * @param capabilities A pointer to the capabilities list.
 * @param index The index of the property to get.
 * @return A pointer to the property at the specified index.
 */
static inline properties_t** capabilities_get(const capabilities_t* capabilities, size_t index) {
    return (properties_t**)list_get(capabilities, index);
}

/**
 * @brief Set a property in the capabilities list by index.
 *
 * @param capabilities A pointer to the capabilities list.
 * @param index The index of the property to set.
 * @param properties A pointer to the properties to set.
 * @return true if the property was set successfully, false otherwise.
 */
static inline bool capabilities_set(const capabilities_t* capabilities, size_t index, properties_t** properties) {
    return list_set(capabilities, index, properties);
}

/**
 * @brief Get the capacity of the capabilities list.
 *
 * @param capabilities A pointer to the capabilities list.
 * @return The capacity of the capabilities list.
 */
static inline size_t capabilities_capacity(const capabilities_t* capabilities) { return capabilities->capacity; }

/**
 * @brief Get the size of the capabilities list.
 *
 * @param capabilities A pointer to the capabilities list.
 * @return The size of the capabilities list.
 */
static inline size_t capabilities_size(const capabilities_t* capabilities) { return capabilities->size; }

/**
 * @brief Check if the capabilities list is empty.
 *
 * @param capabilities A pointer to the capabilities list.
 * @return true if the capabilities list is empty, false otherwise.
 */
static inline bool capabilities_empty(const capabilities_t* capabilities) { return capabilities->size == 0; }

/**
 * @brief Clear the capabilities list.
 *
 * @param capabilities A pointer to the capabilities list.
 * @param free_func A function pointer to free the properties.
 */
static inline void capabilities_clear(capabilities_t* capabilities, void (*free_func)(properties_t**)) {
    list_clear(capabilities, (void (*)(void*))free_func);
}

/**
 * @brief Duplicate a capabilities list.
 *
 * @param original A pointer to the original capabilities list.
 * @return A pointer to the duplicated capabilities list, or NULL if duplication failed.
 */
static inline capabilities_t* capabilities_duplicate(const capabilities_t* original) {
    if (original == NULL) {
        return NULL;
    }

    capabilities_t* duplicate = capabilities_new(capabilities_capacity(original));
    if (duplicate == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < capabilities_size(original); ++i) {
        properties_t** original_properties = capabilities_get(original, i);
        if (original_properties == NULL) {
            capabilities_free(duplicate, NULL);
            return NULL;
        }

        properties_t* new_properties = properties_duplicate(*original_properties);
        if (new_properties == NULL) {
            capabilities_free(duplicate, NULL);
            return NULL;
        }

        if (!capabilities_add(duplicate, &new_properties)) {
            properties_free(new_properties, NULL);
            capabilities_free(duplicate, NULL);
            return NULL;
        }
    }
    return duplicate;
}

static inline void capabilities_iterate(const capabilities_t* capabilities, void (*callback)(properties_t**)) {
    if (capabilities == NULL || callback == NULL) {
        return;
    }
    for (size_t i = 0; i < capabilities_size(capabilities); ++i) {
        properties_t** properties = capabilities_get(capabilities, i);
        callback(properties);
    }
}

static inline void capabilities_inspect(const capabilities_t* caps, int (*print_func)(const char*, ...),
                                        const char* indent) {
    print_func("%sCapabilities:\n", indent);
    char new_indent[10];
    snprintf(new_indent, sizeof(new_indent), "%s\t", indent);
    for (size_t i = 0; i < capabilities_size(caps); ++i) {
        const properties_t* cap_properties = *capabilities_get(caps, i);
        properties_inspect(cap_properties, print_func, new_indent);
    }
}

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_CAPABILITIES_H_