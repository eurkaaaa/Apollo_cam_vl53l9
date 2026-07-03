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

#ifndef _TRANSFORM_C_H_
#define _TRANSFORM_C_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "media-c/stream_buffers.h"
#include "media.h"
#include "transform-c/version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _transform {
    media_t media;  // Inherit from media_t

    /**
     * @brief Query the dependencies of a stream
     * @param[in] self Instance of the IPP handle
     * @param[in] name Name of the stream
     * @param[out] dependencies List of dependent streams
     * @note @p dependencies is a NULL-terminated array
     */
    int (*query_stream_dependencies)(const struct _transform* self, const char* name, const strings_t** dependencies);

    //Private members
    /**
     * @brief Process a stream
     * @param[in] self Instance of the IPP handle
     * @param[in] input Input stream buffer
     * @param[out] output Output stream buffer
     *
     * @note Some output streams may have multiple dependencies with several input streams, so not necessarily ready
     * to be computed. If an output stream is not ready to be computed due to pending input stream dependencies, its
     * processing is deferred until all dependencies are completed.
     *
     * TODO: how to inform how many output streams are requested?
     *
     */
    int (*do_process_stream)(const struct _transform* self, const stream_buffers_t* stream_buffers);

} transform_t;

static inline int transform_initialize(const transform_t* self) {
    media_t* media = (media_t*)self;
    return media_initialize(media);
}

static inline int transform_release(const transform_t* self) {
    media_t* media = (media_t*)self;
    return media_release(media);
}

static inline int transform_prepare(const transform_t* self) {
    media_t* media = (media_t*)self;
    return media_prepare(media);
}

static inline int transform_finalize(const transform_t* self) {
    media_t* media = (media_t*)self;
    return media_finalize(media);
}

static inline int transform_get_state(const transform_t* self, media_state_t* state) {
    if (!self || !state) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    }
    // Call the media's get_state function to retrieve the state
    int ret = media_get_state((const media_t*)self, state);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the media's get_state function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved the state
}

static inline void transform_get_version(const transform_t* self, uint32_t* major, uint32_t* minor, uint32_t* patch) {
    // Get the version of the implemented Transform API
    *major = TRANSFORM_C_VERSION_MAJOR;
    *minor = TRANSFORM_C_VERSION_MINOR;
    *patch = TRANSFORM_C_VERSION_PATCH;
}

static inline int transform_get_streams(const transform_t* self, const streams_t** streams) {
    media_t* media = (media_t*)self;
    return media_get_streams(media, streams);
}

static inline int transform_get_stream_capabilities(const transform_t* self, const char* const name,
                                                    const capabilities_t** caps) {
    media_t* media = (media_t*)self;
    return media_get_stream_capabilities(media, name, caps);
}

static inline int transform_set_stream_capabilities(const transform_t* self, const char* const name,
                                                    const capabilities_t* caps) {
    media_t* media = (media_t*)self;
    return media_set_stream_capabilities(media, name, caps);
}

static inline int transform_get_controls(const transform_t* self, const controls_t** controls) {
    media_t* media = (media_t*)self;
    return media_get_controls(media, controls);
}

static inline int transform_get_control(const transform_t* self, const char* const name, value_t* value) {
    media_t* media = (media_t*)self;
    return media_get_control(media, name, value);
}

static inline int transform_set_control(const transform_t* self, const char* name, const value_t value) {
    media_t* media = (media_t*)self;
    return media_set_control(media, name, value);
}

static inline int transform_about(const transform_t* self, const properties_t** properties) {
    media_t* media = (media_t*)self;
    return media_about(media, properties);
}

static inline int transform_process_stream(const transform_t* self, const stream_buffers_t* stream_buffers) {
    if (!self || !stream_buffers) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    }
    if (self->do_process_stream == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }

    // Call the private do_process_stream function to process the stream buffers
    // and return the result
    // Note: The do_process_stream function should handle the processing logic
    // and return MEDIA_ERROR_NONE on success or an appropriate error code on failure
    int ret = self->do_process_stream(self, stream_buffers);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully processed the stream buffers
}

static inline int transform_query_stream_dependencies(const transform_t* self, const char* name,
                                                      const strings_t** dependencies) {
    if (!self || !name || !dependencies) {
        return MEDIA_ERROR_INVALID_PARAMETER;  // Check for null pointers
    }
    if (self->query_stream_dependencies == NULL) {
        return MEDIA_ERROR_UNIMPLEMENTED;  // Unsupported operation
    }
    // Call the private query_stream_dependencies function to retrieve the dependencies
    int ret = self->query_stream_dependencies(self, name, dependencies);
    if (ret != MEDIA_ERROR_NONE) {
        return ret;  // Return the error code from the private function
    }
    return MEDIA_ERROR_NONE;  // Successfully retrieved dependencies
}

static inline void transform_inspect(const transform_t* self, int (*print_func)(const char*, ...)) {
    media_inspect((const media_t*)self, print_func);
}

#ifdef __cplusplus
}
#endif

#endif  // _TRANSFORM_C_H_