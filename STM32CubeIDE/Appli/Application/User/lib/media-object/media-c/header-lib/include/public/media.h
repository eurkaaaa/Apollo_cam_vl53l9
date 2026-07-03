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

#ifndef _MEDIA_C_H_
#define _MEDIA_C_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "media-c/buffer.h"
#include "media-c/capabilities.h"
#include "media-c/control.h"
#include "media-c/controls.h"
#include "media-c/error.h"
#include "media-c/properties.h"
#include "media-c/spec.h"
#include "media-c/state.h"
#include "media-c/stream.h"
#include "media-c/stream_buffers.h"
#include "media-c/streams.h"
#include "media-c/strings.h"
#include "media-c/value.h"
#include "media-c/version.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file media.h
 * @brief Public API for the Media Processing Library
 *
 * This header file defines the public interface for the Media Processing Library,
 * including functions to manage media streams, controls, and capabilities.
 */
typedef struct _media {
    /* ============================================================================
       ============================== Public members ============================== */

    /**
     * @brief Get the version of the implementated media API
     * @param[in] self Instance of the media handle
     * @param[out] major
     * @param[out] minor
     * @param[out] patch
     */
    void (*get_version)(const struct _media *self, uint32_t *major, uint32_t *minor, uint32_t *patch);

    /**
     * @brief Get the list of available streams for a given instance
     * @param[in] self Instance of the media handle
     * @param[out] streams List of streams. Ownership is not transferred to the caller (const)
     * @note @p streams is a NULL-terminated array
     */
    int (*get_streams)(const struct _media *self, const streams_t **streams);

    /**
     * @brief Get the list of available controls for a given instance
     * @param[in] self Instance of the media handle
     * @param[out] controls List of controls
     * @note @p controls is a NULL-terminated array
     */
    int (*get_controls)(const struct _media *self, const controls_t **controls);

    /**
     * @brief Get the value of a control by its ID
     * @param[in] self Instance of the media handle
     * @param[in] id ID of the control
     * @param[out] value Value of the control
     */
    int (*about)(const struct _media *self, const properties_t **properties);

    /**
     * @brief Query the capabilities of a stream
     * @param[in] self Instance of the media handle
     * @param[in] name Name of the stream
     * @param[out] streams List of dependent streams
     * @note @p streams is a NULL-terminated array
     */
    int (*query_stream_capabilities)(const struct _media *self, const char *name, capabilities_t **caps);

    /**
     * @brief Get the error information of the media instance.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API to retrieve error details.
     */
    media_error_t (*get_last_error)(const struct _media *self);

    /* =============================================================================
       ============================== Private members ============================== */

    /**
     * @brief Initialize the private context of the handle.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API.
     */
    int (*do_initialize)(const struct _media *self);

    /**
     * @brief Release the private context of the handle.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API.
     */
    int (*do_release)(const struct _media *self);

    /**
     * @brief Prepare the media instance for processing.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API.
     */
    int (*do_prepare)(const struct _media *self);

    /**
     * @brief Finalize the media instance after processing.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API.
     */
    int (*do_finalize)(const struct _media *self);

    /**
     * @brief Get the value of a control by its name
     * @param[in] self Instance of the media handle
     * @param[in] name Name of the control
     * @param[out] value Value of the control
     */
    int (*do_get_control)(const struct _media *self, const char *const name, value_t *value);

    /**
     * @brief Set the value of a control by its name
     * @param[in] self Instance of the media handle
     * @param[in] name Name of the control
     * @param[in] value Value of the control
     */
    int (*do_set_control)(const struct _media *self, const char *name, const value_t value);

    /**
     * @brief Get the capabilities of a given stream
     * @param[in] self Instance of the media handle
     * @param[in] name Name of the stream
     * @param[out] caps List of capabilities
     * @note @p caps is a NULL-terminated array
     */
    int (*do_get_stream_capabilities)(const struct _media *self, const char *const name, const capabilities_t **caps);

    /**
     * @brief Set the desired capabilities of a given stream
     * @param[in] self Instance of the media handle
     * @param[in] name Name of the stream
     * @param[in] caps List of capabilities
     */
    int (*do_set_stream_capabilities)(const struct _media *self, const char *const name, const capabilities_t *caps);

    /**
     * @brief Set the current state of the media instance.
     * @note This is a private member and should not be called directly by users.
     * It is used internally by the public API to update the state.
     */
    int (*set_state)(const struct _media *self, media_state_t state);

    media_state_t state;                        // Current state of the media instance
    media_state_transition_t state_transition;  // Current state transition of the media instance

} media_t;

/// Private function to set the state of the Media instance
static inline int set_state(media_t *self, media_state_t state) {
    if (!self) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer or unimplemented function
    }

    // Check for valid state transition
    media_state_t current = self->state;
    media_state_transition_t transition = MEDIA_STATE_TRANSITION_NONE;

    if (current == MEDIA_STATE_NULL && state == MEDIA_STATE_INITIALIZED) {
        transition = MEDIA_STATE_TRANSITION_NULL_TO_INITIALIZED;
    } else if (current == MEDIA_STATE_INITIALIZED && state == MEDIA_STATE_PREPARED) {
        transition = MEDIA_STATE_TRANSITION_INITIALIZED_TO_PREPARED;
    } else if (current == MEDIA_STATE_PREPARED && state == MEDIA_STATE_STREAMING) {
        transition = MEDIA_STATE_TRANSITION_PREPARED_TO_STREAMING;
    } else if (current == MEDIA_STATE_STREAMING && state == MEDIA_STATE_PREPARED) {
        transition = MEDIA_STATE_TRANSITION_STREAMING_TO_PREPARED;
    } else if (current == MEDIA_STATE_PREPARED && state == MEDIA_STATE_INITIALIZED) {
        transition = MEDIA_STATE_TRANSITION_PREPARED_TO_INITIALIZED;
    } else if (current == MEDIA_STATE_INITIALIZED && state == MEDIA_STATE_NULL) {
        transition = MEDIA_STATE_TRANSITION_INITIALIZED_TO_NULL;
    } else if (current == state) {
        // No state change needed
        return MEDIA_ERROR_NONE;
    } else {
        // Invalid transition
        return MEDIA_ERROR_INVALID_STATE;
    }

    // Update state and transition
    self->state = state;
    self->state_transition = transition;
    return MEDIA_ERROR_NONE;
}

/// Public functions

/**
 * @brief Gets the current state of the Media instance.
 *
 * @param media Pointer to the Media instance.
 * @return int The current state (e.g., 0 = Null, 1 = Initialized, 2 = Prepared).
 */
static inline int media_get_state(const media_t *self, media_state_t *state) {
    if (!self || !state) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer
    }
    *state = self->state;     // Get the current state
    return MEDIA_ERROR_NONE;  // Success
}

/**
 * @brief Initializes the processing library.
 *
 * This is the first mandatory step in initializing the processing library.
 *
 * @param media Pointer to the Media instance.
 * @return int Returns 0 (NONE) if no error, or an error code otherwise.
 */
static inline int media_initialize(media_t *self) {
    if (!self) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer
    media_state_t state;
    int state_ret = media_get_state(self, &state);
    if (state_ret != MEDIA_ERROR_NONE) {
        return state_ret;
    }
    // Call the private initialization function
    // and update the state to initialized if successful
    if (self->do_initialize == NULL || self->do_release == NULL || self->do_prepare == NULL ||
        self->do_finalize == NULL || self->get_streams == NULL || self->get_controls == NULL || self->about == NULL ||
        self->query_stream_capabilities == NULL || self->get_last_error == NULL || self->do_get_control == NULL ||
        self->do_set_control == NULL || self->do_get_stream_capabilities == NULL ||
        self->do_set_stream_capabilities == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    int ret = self->do_initialize(self);
    if (ret == MEDIA_ERROR_NONE) {
        set_state(self, MEDIA_STATE_INITIALIZED);  // Update state to initialized
    }
    return ret;
}

/**
 * @brief Releases resources and buffers.
 *
 * All allocated resources are freed. This function cannot be called if the processing library hasn't been initialized.
 *
 * @param media Pointer to the Media instance.
 * @return int Returns 0 (NONE) if no error, or an error code otherwise.
 */
static inline int media_release(media_t *self) {
    if (!self) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer
    media_state_t state;
    int state_ret = media_get_state(self, &state);
    if (state_ret != MEDIA_ERROR_NONE) {
        return state_ret;
    }
    // Call the private release function
    // and update the state to uninitialized if successful
    if (self->do_release == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Release resources and update state
    // Note: do_release should handle any necessary cleanup
    // and return MEDIA_ERROR_NONE on success
    int ret = self->do_release(self);
    if (ret == MEDIA_ERROR_NONE) {
        set_state(self, MEDIA_STATE_NULL);  // Update state to uninitialized
    }
    return ret;
}

/**
 * @brief Prepares the Media instance.
 *
 * This function is responsible for preparing the Media instance. All static controls must be set before calling this function.
 * It may allocate resources, set up internal state, and perform any necessary initialization steps.
 * The Media instance must be in the Initialized state before calling this function.
 *
 * @param media Pointer to the Media instance.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_prepare(media_t *self) {
    if (!self) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer
    media_state_t state;
    int state_ret = media_get_state(self, &state);
    if (state_ret != MEDIA_ERROR_NONE) {
        return state_ret;
    }
    // Call the private prepare function
    // and update the state to prepared if successful
    if (self->do_prepare == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Prepare the media instance and update state
    // Note: do_prepare should handle any necessary setup
    // and return MEDIA_ERROR_NONE on success
    // This function may block for a significant amount of time, depending on the Media implementation
    // The derived class must ensure that the preparation logic is efficient and does not block for an excessive amount of time.
    int ret = self->do_prepare(self);
    if (ret == MEDIA_ERROR_NONE) {
        ret = set_state(self, MEDIA_STATE_PREPARED);  // Update state to prepared
    }
    return ret;
}

/**
 * @brief Finalizes the Media instance and frees associated resources.
 *
 * This function must be called after the Media instance has been prepared.
 *
 * @param media Pointer to the Media instance.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_finalize(media_t *self) {
    if (!self) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointer
    media_state_t state;
    int state_ret = media_get_state(self, &state);
    if (state_ret != MEDIA_ERROR_NONE) {
        return state_ret;
    }
    // Call the private finalize function
    // and update the state to initialized if successful
    if (self->do_finalize == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Finalize the media instance and update state
    // Note: do_finalize should handle any necessary cleanup
    // and return MEDIA_ERROR_NONE on success
    // This function may block for a significant amount of time, depending on the Media implementation
    // The derived class must ensure that the finalization logic is efficient and does not block for an excessive amount of time.
    int ret = self->do_finalize(self);
    if (ret == MEDIA_ERROR_NONE) {
        set_state(self, MEDIA_STATE_INITIALIZED);  // Update state to initialized
    }
    return ret;
}

static inline void media_get_version(const media_t *self, uint32_t *major, uint32_t *minor, uint32_t *patch) {
    self->get_version(self, major, minor, patch);
}

/**
 * @brief Retrieves the list of streams available in the Media instance.
 *
 * Streams are the input and output interfaces of the Media instance.
 *
 * @param media Pointer to the Media instance.
 * @param streams Array of pointers to MediaStream to be filled.
 * @param count Pointer to a variable holding the size of the streams array; set to the number of streams on return.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_get_streams(const media_t *self, const streams_t **streams) {
    if (!self || !streams) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->get_streams == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private get_streams function to retrieve the streams
    int ret = self->get_streams(self, streams);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved streams
}

/**
 * @brief Retrieves the capabilities of a specific stream by name.
 *
 * This function queries the capabilities associated with the given stream name.
 * Can only be called when the Media instance is in the Prepared state.
 *
 * @param media Pointer to the Media instance.
 * @param name Name of the stream.
 * @param caps Pointer to a MediaCapabilities pointer to receive the capabilities.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_get_stream_capabilities(const media_t *self, const char *const name,
                                                const capabilities_t **caps) {
    if (!self || !name || !caps) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->do_get_stream_capabilities == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private get_stream_capabilities function to retrieve the capabilities
    int ret = self->do_get_stream_capabilities(self, name, caps);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved capabilities
}

/**
 * @brief Configures the desired capabilities on a stream.
 *
 * This function sets the capabilities for the specified stream. Can only be called when the Media instance is in the Prepared state.
 *
 * @param media Pointer to the Media instance.
 * @param name Name of the stream.
 * @param caps Pointer to the capabilities to set.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_set_stream_capabilities(const media_t *self, const char *const name,
                                                const capabilities_t *caps) {
    if (!self || !name || !caps) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->do_set_stream_capabilities == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private set_stream_capabilities function to set the capabilities
    int ret = self->do_set_stream_capabilities(self, name, caps);
    return ret;
}

/**
 * @brief Retrieves the list of controls available in the Media instance.
 *
 * Controls can be used to configure or query various aspects of the Media processing.
 *
 * @param media Pointer to the Media instance.
 * @param controls Array of pointers to MediaControl to be filled.
 * @param count Pointer to a variable holding the size of the controls array; set to the number of controls on return.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_get_controls(const media_t *self, const controls_t **controls) {
    if (!self || !controls) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->get_controls == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private get_controls function to retrieve the controls
    int ret = self->get_controls(self, controls);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved controls
}

/**
 * @brief Gets the value of a control by its name.
 *
 * Allows retrieving the value of a specified control.
 *
 * @param media Pointer to the Media instance.
 * @param name Name of the control.
 * @param value Pointer to a MediaValue to receive the value.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_get_control(const media_t *self, const char *const name, value_t *value) {
    if (!self || !name || !value) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->do_get_control == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private get_control function to retrieve the control value
    int ret = self->do_get_control(self, name, value);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    if (value->tid == VTID_INVALID) {
        return MEDIA_ERROR_NOT_FOUND;  // Control not found or invalid type
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved control value
}

/**
 * @brief Sets the value of a control by its name.
 *
 * Allows setting the value of a specified control.
 *
 * @param media Pointer to the Media instance.
 * @param name Name of the control.
 * @param value Pointer to the value to set.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_set_control(const media_t *self, const char *name, const value_t value) {
    if (!self || !name) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->do_set_control == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Validate the value type before setting
    if (value.tid == VTID_INVALID) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Invalid value type
    }
    // Call the private set_control function to set the control value
    int ret = self->do_set_control(self, name, value);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully set control value
}

/**
 * @brief Retrieves the Media information properties.
 *
 * This function retrieves properties such as name, description, version, license, url, and open source dependencies.
 *
 * @param media Pointer to the Media instance.
 * @param info Pointer to a MediaProperties pointer to receive the properties.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_about(const media_t *self, const properties_t **properties) {
    if (!self || !properties) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->about == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private about function to retrieve the properties
    int ret = self->about(self, properties);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved properties
}

/**
 * @brief Determines the compatible capabilities of a stream in the current configuration context.
 *
 * For input streams, lists all available capabilities. For output streams, returns only the capabilities compatible with the current configuration of its dependent input streams.
 *
 * @param media Pointer to the Media instance.
 * @param name Name of the stream.
 * @param caps Pointer to a MediaCapabilities pointer to receive the capabilities.
 * @return int Error code indicating the success or failure of the operation.
 */
static inline int media_query_stream_capabilities(const media_t *self, const char *name, capabilities_t **caps) {
    if (!self || !name || !caps) return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    if (self->query_stream_capabilities == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private query_stream_capabilities function to retrieve the capabilities
    int ret = self->query_stream_capabilities(self, name, caps);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved capabilities
}

/**
 * @brief Inspects and prints the current state and configuration of the Media instance.
 *
 * Writes a human-readable summary of the Media instance to the provided output stream.
 *
 * @param media Pointer to the Media instance.
 * @param os Output stream (e.g., FILE*).
 * @return int Error code indicating the success or failure of the inspection.
 */
static inline void media_inspect(const media_t *self, int (*print_func)(const char *, ...)) {
    if (!self) {
        print_func("Invalid media instance.\n");
        return;
    }

    // Print about information
    const properties_t *properties;
    if (self->about(self, &properties) == 0) {
        print_func("About:\n  ");
        properties_inspect(properties, print_func, "\t");
    } else {
        print_func("Failed to get about information.\n");
    }

    // Print streams
    const streams_t *streams;
    if (self->get_streams(self, &streams) == 0) {
        streams_inspect(streams, print_func);
    } else {
        print_func("Failed to get stream list.\n");
    }

    // Print controls
    const controls_t *controls;
    if (self->get_controls(self, &controls) == 0) {
        controls_inspect(controls, print_func);
    } else {
        print_func("Failed to get control list.\n");
    }
}

#ifdef __cplusplus
}
#endif

#endif // _MEDIA_C_H_