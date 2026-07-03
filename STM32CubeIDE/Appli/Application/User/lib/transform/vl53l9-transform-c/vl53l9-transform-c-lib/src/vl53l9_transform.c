/**
 *******************************************************************************
 * @file    vl53l9_transform.c
 *******************************************************************************
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

#include "vl53l9_transform.h"
#include "media-c/memory.h"
#include "media-c/properties.h"
#include "media-c/property.h"
#include "media-c/stream_buffers.h"
#include "vl53l9-transform-c/about.h"
#include "vl53l9_calib_default.h"
#include "vl53l9_strings.h"
#include "vl53l9_transform_utils.h"
#include "vl53l9_utils.h"

#include "algo/confidence.h"
#include "algo/distance_calibration.h"
#include "algo/distance_check.h"
#include "algo/extract.h"
#include "algo/radial_to_perp.h"
#include "algo/ratenorm.h"
#include "algo/reflectance.h"
#include "algo/sharpener.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if VL53L9_TRANSFORM_DEBUG
#include <stdio.h>
#define DEBUG_PRINT(fmt, ...) printf("DEBUG: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) // Do nothing if the flag is not set
#endif

#define CTRL_DEFAULT_SPEC { { 0 }, { 0 }, 0 }

#define R2P_EFL_UM         (2428.16f)
#define R2P_SPAD_SIZE_UM   (10.17f)
#define R2P_ALPHA          (-15e-5f)
#define R2P_BETA           (0.0f)
#define R2P_GAMMA          (0.0f)
#define R2P_KAPPA          (0.0f)
#define R2P_PARALLAX_LIMIT (50.0f)

#define SIGNAL_FACTOR (1.4f)

#define NB_SHOTS_STEP_1   (0)
#define NB_SHOTS_STEP_4_5 (1)
#define NB_SHOTS_STEP_6   (2)
#define NB_SHOTS_STEP_7   (3)

/* private types */

typedef enum {
    _STREAM_ID_IN_RAW = 0U,
    _STREAM_ID_OUT_DEPTH,
    _STREAM_ID_OUT_AMBIENT,
    _STREAM_ID_OUT_AMPLITUDE,
    _STREAM_ID_OUT_CONFIDENCE,
    // TODO: add custom output
    _STREAM_ID_MAX, // NOTE: keep this entry at the end
} _stream_id_t;

typedef enum {
    _CONTROL_ID_BYPASS_R2P_ALGO = 0U,
    _CONTROL_ID_BYPASS_SHARPENER_ALGO,
    _CONTROL_ID_BYPASS_R2P_FILTER,
    _CONTROL_ID_BYPASS_CONFIDENCE_FILTER,
    _CONTROL_ID_BYPASS_REFLECTANCE_FILTER,
    _CONTROL_ID_BYPASS_SHARPENER_FILTER,
    _CONTROL_ID_CALIB_BUFFER, // NOTE: this control is mandatory
    _CONTROL_ID_COVER_GLASS,
    // TODO: add custom output
    _CONTROL_ID_MAX, // NOTE: keep this entry at the end
} _control_id_t;

// NOTE: internal buffers used for processing
typedef enum {

    // extract
    _depth_in = 0,    // float
    _amplitude_in,    // float
    _ambient_in,      // float
    _msb_in,          // bool
    _dss_lut_in,      // unsigned char
    _effective_spads, // float

    // distance_calibration
    _depth_calibrated, // float

    // ratenorm
    _amplitude_ref,     // float
    _amplitude_ref_rad, // float
    _signal_rate,       // float
    _ambient_rate,      // float
    _ambient_norm,      // float

    // reflectance
    _reflectance,       // float
    _validity_low_refl, // bool

    // radial_to_perp
    _depth_r2p,      // float
    _center_x_r2p,   // float (required by pointcloud)
    _distortion_r2p, // float (required by pointcloud)
    _validity_r2p,   // bool

    // pointcloud
    _pointcloud, // 4 * float per pixel (x, y, z, confidence)

    // sharpener
    _validity_sharpener, // bool
    _sharpener_score,    // float (required by flicker filter)

    // confidence
    _confidence,           // float
    _xtalk_estimated,      // float (required by dmax)
    _threshold_confidence, // float (TODO: can be removed after reworking confidence algo)
    _validity_confidence,  // bool
    _android_confidence,   // unsigned char

    // distance_check
    _depth_out,  // float
    _status_out, // unsigned char

    // _depth16_out,
    // _custom_out,

    _nb_buffers, // keep last

} _buffer_id_t;

typedef struct {
    /* mandatory */
    media_error_t last_error; // TODO: not fully supported yet

    /* custom */
    bool is_first_frame;
    bool is_pointcloud_requested;
    void *buffers[_nb_buffers];
    value_t controls[_CONTROL_ID_MAX];
    properties_t *properties[_STREAM_ID_MAX];

    /* calibration data */
    vl53l9_calib_data_t calib;
    float *offset_map;          // needed by distance_calibration algo
    float *ref_amp_no_expo;     // needed by ratenorm algo
    float *ref_amp_rad_no_expo; // needed by ratenorm algo
    float *coeff_norm_no_expo;  // needed by ratenorm algo
} _context_t;

typedef struct {
    transform_t transform;
    _context_t ctx;
} _vl53l9_transform_t;

// TODO: check malloc returned value

// private functions prototypes

// transform api implementation

static int _get_streams(_vl53l9_transform_t *const self, const streams_t **streams);
static int _get_controls(_vl53l9_transform_t *const self, const controls_t **controls);
static int _about(_vl53l9_transform_t *self, const properties_t **properties);
static int _query_stream_capabilities(_vl53l9_transform_t *self, const char *name, capabilities_t **caps);
static media_error_t _get_last_error(_vl53l9_transform_t *self);

static int _do_initialize(_vl53l9_transform_t *const self);
static int _do_release(_vl53l9_transform_t *const self);
static int _do_prepare(_vl53l9_transform_t *const self);
static int _do_finalize(_vl53l9_transform_t *const self);
static int _do_get_control(_vl53l9_transform_t *const self, const char *const name, value_t *value);
static int _do_set_control(_vl53l9_transform_t *const self, const char *name, const value_t value);
static int _do_get_stream_capabilities(_vl53l9_transform_t *const self, const char *const stream_name,
                                       const capabilities_t **caps);
static int _do_set_stream_capabilities(_vl53l9_transform_t *const self, const char *const stream_name,
                                       const capabilities_t *caps);

static int _query_stream_dependendencies(_vl53l9_transform_t *const self, const char *name,
                                         const strings_t **dependencies);
static int _do_process_stream(_vl53l9_transform_t *const self, const stream_buffers_t *stream_buffers);

// helpers
static void _extract_metadata(uint8_t *, size_t, vl53l9_metadata_t *);
static void _compute_calibration_maps(_context_t *, uint8_t, size_t, size_t);
static properties_t *_check_stream_properties(const properties_t *, _stream_id_t);
static bool _check_streams_consistency(properties_t **cap);
static _stream_id_t _get_stream_id(const char *const);
static void _compute_integration_time(vl53l9_metadata_t *const, unsigned int nb_shots[4], float *const);
static void _compute_confidence_params(vl53l9_metadata_t *const, unsigned int nb_shots[4], float *const, float *const,
                                       float *const, float *const);
static void _build_nb_shots(vl53l9_metadata_t *const, unsigned int nb_shots[4]);

// post-processing blocks wrappers
static void _process_extract(_context_t *, vl53l9_metadata_t *, memory_t *);
static void _process_confidence(_context_t *, vl53l9_metadata_t *);
static void _process_distance_calibration(_context_t *, vl53l9_metadata_t *);
static void _process_ratenorm(_context_t *, vl53l9_metadata_t *);
static void _process_reflectance(_context_t *, vl53l9_metadata_t *);
static void _process_radial_to_perp(_context_t *, vl53l9_metadata_t *);
static void _process_sharpener(_context_t *, vl53l9_metadata_t *);
static void _process_distance_check(_context_t *, vl53l9_metadata_t *);
static void _process_pointcloud(_context_t *, vl53l9_metadata_t *);
static void _process_free_buffers(_context_t *);

// NOTE: elements defined in this list should match the order of IDs in the enum
static control_t _controls_list[] = {
    { "bypass-r2p-algo",
      CTRL_BYPASS_R2P_ALGO_NICK,
      CTRL_BYPASS_R2P_ALGO_DESCRIPTION,
      _CONTROL_ID_BYPASS_R2P_ALGO,
      { .val.v_bool = false, .tid = VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },

    { "bypass-sharpener-algo",
      CTRL_BYPASS_SHARPENER_ALGO_NICK,
      CTRL_BYPASS_SHARPENER_ALGO_DESCRIPTION,
      _CONTROL_ID_BYPASS_SHARPENER_ALGO,
      { .val.v_bool = false, .tid = VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },

    { "bypass-r2p-filter",
      CTRL_BYPASS_R2P_FILTER_NICK,
      CTRL_BYPASS_R2P_FILTER_DESCRIPTION,
      _CONTROL_ID_BYPASS_R2P_FILTER,
      { .val.v_bool = false, .tid = VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    { "bypass-conf-filter",
      CTRL_BYPASS_CONFIDENCE_FILTER_NICK,
      CTRL_BYPASS_CONFIDENCE_FILTER_DESCRIPTION,
      _CONTROL_ID_BYPASS_CONFIDENCE_FILTER,
      { .val.v_bool = false, VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    { "bypass-refl-filter",
      CTRL_BYPASS_REFLECTANCE_FILTER_NICK,
      CTRL_BYPASS_REFLECTANCE_FILTER_DESCRIPTION,
      _CONTROL_ID_BYPASS_REFLECTANCE_FILTER,
      { .val.v_bool = false, .tid = VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    { "bypass-sharpener-filter",
      CTRL_BYPASS_SHARPENER_FILTER_NICK,
      CTRL_BYPASS_SHARPENER_FILTER_DESCRIPTION,
      _CONTROL_ID_BYPASS_SHARPENER_FILTER,
      { .val.v_bool = false, .tid = VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    { "calib-buffer",
      CTRL_CALIB_BUFFER_NICK,
      CTRL_CALIB_BUFFER_DESCRIPTION,
      _CONTROL_ID_CALIB_BUFFER,
      { .val.v_ptr = NULL, .tid = VTID_POINTER },
      VTID_POINTER,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    { "cover-glass",
      CTRL_COVER_GLASS_NICK,
      CTRL_COVER_GLASS_DESCRIPTION,
      _CONTROL_ID_COVER_GLASS,
      { .val.v_bool = false, VTID_BOOL },
      VTID_BOOL,
      CTRL_FLAGS_READABLE | CTRL_FLAGS_WRITABLE,
      CTRL_DEFAULT_SPEC },
    // TODO: add custom output
};

// clang-format off
#define PROPERTY_LIST(format, width, height)                          \
    (property_t[]) {                                                  \
        { "format", { .val.v_string = format, .tid = VTID_STRING } }, \
        { "width", { .val.v_uint32 = width, .tid = VTID_UINT32 } },   \
        { "height", { .val.v_uint32 = height, .tid = VTID_UINT32 } }, \
    }

#define STREAM_PROPS(format, width, height)             \
    (const properties_t) {                              \
        .items = PROPERTY_LIST(format, width, height),  \
        .size = 3,                                      \
        .capacity = 3,                                  \
        .item_size = sizeof(property_t),                \
    }
// clang-format on

// TODO: add custom stream
// NOTE: set csi_width to 32 or other aligned value (check that there is no padding introduced from the capture system)
static const streams_t _streams = {
    .items =
        (void *)(const stream_t[]){
            {
                .name = "raw",
                .description = STREAM_RAW_DESCRIPTION,
                .direction = DIRECTION_INPUT,
                .capabilities =
                    &(capabilities_t){ .items =
                                           (const properties_t *[]){
                                               &STREAM_PROPS("3DMD", 100, 149), &STREAM_PROPS("3DMD", 100, 39), // csi
                                               &STREAM_PROPS("3DMD", 14842, 1), &STREAM_PROPS("3DMD", 3844, 1), // i3c
                                           },
                                       .size = 4,
                                       .capacity = 4,
                                       .item_size = sizeof(properties_t *) },
            },
            {
                .name = "depth",
                .description = STREAM_DEPTH_DESCRIPTION,
                .direction = DIRECTION_OUTPUT,
                .capabilities = &(capabilities_t){ .items =
                                                       (const properties_t *[]){
                                                           &STREAM_PROPS("ZF32", 54, 42),
                                                           &STREAM_PROPS("ZF32", 24, 20),
                                                           &STREAM_PROPS("ZAPC", 54, 42),
                                                           &STREAM_PROPS("ZAPC", 24, 20),
                                                       },
                                                   .size = 4,
                                                   .capacity = 4,
                                                   .item_size = sizeof(properties_t *) },
            },
            {
                .name = "ambient",
                .description = STREAM_AMBIENT_DESCRIPTION,
                .direction = DIRECTION_OUTPUT,
                .capabilities = &(capabilities_t){ .items =
                                                       (const properties_t *[]){
                                                           &STREAM_PROPS("IF32", 54, 42),
                                                           &STREAM_PROPS("IF32", 24, 20),
                                                       },
                                                   .size = 2,
                                                   .capacity = 2,
                                                   .item_size = sizeof(properties_t *) },
            },
            {
                .name = "amplitude",
                .description = STREAM_AMPLITUDE_DESCRIPTION,
                .direction = DIRECTION_OUTPUT,
                .capabilities = &(capabilities_t){ .items = (const properties_t *[]){ &STREAM_PROPS("AF32", 54, 42),
                                                                                      &STREAM_PROPS("AF32", 24, 20) },
                                                   .size = 2,
                                                   .capacity = 2,
                                                   .item_size = sizeof(properties_t *) },
            },
            {
                .name = "confidence",
                .description = STREAM_CONFIDENCE_DESCRIPTION,
                .direction = DIRECTION_OUTPUT,
                .capabilities = &(capabilities_t){ .items = (const properties_t *[]){ &STREAM_PROPS("CF32", 54, 42),
                                                                                      &STREAM_PROPS("CF32", 24, 20) },
                                                   .size = 2,
                                                   .capacity = 2,
                                                   .item_size = sizeof(properties_t *) },
            } },
    .size = 5,
    .capacity = 5,
    .item_size = sizeof(stream_t)
};

static const controls_t _controls = { .items = _controls_list,
                                      .size = sizeof(_controls_list) / sizeof(_controls_list[0]),
                                      .capacity = sizeof(_controls_list) / sizeof(_controls_list[0]),
                                      .item_size = sizeof(control_t) };

// TODO: to be moved to about.h.in template, retrieve information from the build system
static const properties_t _about_props = {
    .items =
        (void *)(const property_t[]){ { "name", { .val.v_string = "vl53l9-transform-c", VTID_STRING } },
                                      { "description", { .val.v_string = "VL53L9 Transform library", VTID_STRING } },
                                      { "version", { .val.v_string = VL53L9_TRANSFORM_C_VERSION, VTID_STRING } } },
    .size = 3,
    .capacity = 3,
    .item_size = sizeof(property_t)
};

// NOTE: these symbols are defined to allow dynamic load of the library
#ifdef EXPORT_CREATE_DESTROY
media_t *media_create() {
    return (media_t *)vl53l9_transform_create();
}

void media_destroy(media_t *self) {
    vl53l9_transform_destroy((transform_t *)self);
}
#endif

transform_t *vl53l9_transform_create() {
    _vl53l9_transform_t *vl53l9_transform = (_vl53l9_transform_t *)malloc(sizeof(_vl53l9_transform_t));
    *vl53l9_transform = (_vl53l9_transform_t) {
        .transform = {
            .media = {
                /* public*/
                .get_version = (void (*)(const media_t*, uint32_t*, uint32_t*, uint32_t*))transform_get_version,
                .get_streams = (int (*)(const media_t*, const streams_t**))_get_streams,
                .get_controls = (int (*)(const struct _media *, const controls_t**))_get_controls,
                .about = (int (*)(const struct _media *, const properties_t**))_about,
                .query_stream_capabilities = (int (*)(const struct _media *, const char *, capabilities_t **))_query_stream_capabilities,
                .get_last_error = (media_error_t (*)(const struct _media *))_get_last_error,

                /* private */
                .do_initialize = (int (*)(const media_t*))_do_initialize,
                .do_release = (int (*)(const media_t*))_do_release,
                .do_prepare = (int (*)(const media_t*))_do_prepare,
                .do_finalize = (int (*)(const media_t*))_do_finalize,
                .do_get_control = (int (*)(const media_t*, const char*, value_t*))_do_get_control,
                .do_set_control = (int (*)(const media_t*, const char*, const value_t))_do_set_control,
                .do_get_stream_capabilities = (int (*)(const struct _media *, const char *const, const capabilities_t**))_do_get_stream_capabilities,
                .do_set_stream_capabilities = (int (*)(const struct _media *, const char *const, const capabilities_t*))_do_set_stream_capabilities,
            },
            .query_stream_dependencies = (int (*)(const transform_t*, const char *, const strings_t**))_query_stream_dependendencies,
            .do_process_stream = (int (*)(const transform_t*, const stream_buffers_t *))_do_process_stream,
        }
    };

    return (transform_t *)vl53l9_transform;
}

void vl53l9_transform_destroy(transform_t *self) {
    _vl53l9_transform_t *vl53l9_transform = (_vl53l9_transform_t *)self;

    // free transform instance
    free(vl53l9_transform);
}

static int _do_initialize(_vl53l9_transform_t *self) {

    DEBUG_PRINT("Enter");
    _context_t *ctx = &self->ctx;
    ctx->last_error = (media_error_t){ .code = MEDIA_ERROR_NONE, .message = NULL };
    ctx->is_first_frame = true; // used to detect first frame in the processing pipeline
    ctx->is_pointcloud_requested = false;

    // copy default values from global controls list to instance internal context
    for (size_t i = 0; i < _controls.size; i++) {
        control_t *ctrl = (control_t *)list_get((list_t *)&_controls, i);
        ctx->controls[ctrl->quark] = ctrl->value;
    }

    // initialize capabilities to empty list of properties for each stream
    // NOTE: capabilities must be explicitly set by the user for each requested stream
    for (size_t i = 0; i < _STREAM_ID_MAX; i++) {
        ctx->properties[i] = NULL;
    }

    // initialize internal buffers used during stream processing
    for (int i = 0; i < _nb_buffers; i++) {
        ctx->buffers[i] = NULL;
    }

    ctx->offset_map = NULL;

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _do_release(_vl53l9_transform_t *self) {
    DEBUG_PRINT("Enter");
    (void)self;

    // TODO: free resources allocated during initialization

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _do_prepare(_vl53l9_transform_t *self) {
    DEBUG_PRINT("Enter");

    _context_t *ctx = &self->ctx;

    // make sure that mandatory controls have been set, otherwise return error
    uint8_t *calib_buffer = (uint8_t *)ctx->controls[_CONTROL_ID_CALIB_BUFFER].val.v_ptr;

    // apply default values if control is not set
    if (!calib_buffer) {
        // TODO: define default values
        vl53l9_calib_default_apply(&ctx->calib);
        DEBUG_PRINT("Using default calibration data");
    } else {
        // extract calibration data from buffers if all controls are set
        vl53l9_utils_parse_calib_data(calib_buffer, &ctx->calib);
    }

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _do_finalize(_vl53l9_transform_t *self) {
    DEBUG_PRINT("Enter");

    _context_t *ctx = &self->ctx;
    ctx->is_first_frame = true;

    // deallocate resources that have been allocated in process_stream
    free(ctx->offset_map);
    free(ctx->ref_amp_no_expo);
    free(ctx->ref_amp_rad_no_expo);
    free(ctx->coeff_norm_no_expo);
    ctx->offset_map = NULL;
    ctx->ref_amp_no_expo = NULL;
    ctx->ref_amp_rad_no_expo = NULL;
    ctx->coeff_norm_no_expo = NULL;

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _do_get_control(_vl53l9_transform_t *self, const char *const name, value_t *const value) {
    DEBUG_PRINT("Enter");
    _context_t *ctx = &self->ctx;

    for (size_t i = 0; i < _controls.size; i++) {
        control_t *ctrl = (control_t *)list_get((list_t *)&_controls, i);
        if (strcmp(ctrl->name, name) == 0) {
            *value = ctx->controls[i];
            DEBUG_PRINT("Exit");
            return MEDIA_ERROR_NONE;
        }
    }
    DEBUG_PRINT("Control not found");
    return MEDIA_ERROR_INVALID_PARAMETER;
}

static int _do_set_control(_vl53l9_transform_t *self, const char *name, const value_t value) {
    DEBUG_PRINT("Enter");
    _context_t *ctx = &self->ctx;

    for (size_t i = 0; i < _controls.size; i++) {
        control_t *ctrl = (control_t *)list_get((list_t *)&_controls, i);
        if (strcmp(ctrl->name, name) == 0) {
            ctx->controls[i] = value; // NOTE: this assumes that elements in the list are in the same order as the enum
            DEBUG_PRINT("Exit");
            return MEDIA_ERROR_NONE;
        }
    }
    DEBUG_PRINT("Control not found");
    return MEDIA_ERROR_INVALID_PARAMETER;
}

// TODO: set capabilities to const to avoid modification (to avoid copying them as well)
static int _do_get_stream_capabilities(_vl53l9_transform_t *self, const char *const stream_name,
                                       const capabilities_t **caps) {

    DEBUG_PRINT("Enter");
    (void)self; // unused parameter

    _stream_id_t stream_id = _get_stream_id(stream_name);
    if (stream_id == _STREAM_ID_MAX) {
        DEBUG_PRINT("Invalid stream name");
        return MEDIA_ERROR_INVALID_PARAMETER;
    }

    if (caps) {
        *caps = streams_get(&_streams, stream_id)->capabilities;
    } else {
        DEBUG_PRINT("Invalid caps pointer");
        return MEDIA_ERROR_INVALID_PARAMETER;
    }

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _do_set_stream_capabilities(_vl53l9_transform_t *self, const char *const stream_name,
                                       const capabilities_t *caps) {
    DEBUG_PRINT("Enter");
    _context_t *ctx = &self->ctx;

    // TODO: handle custom output

    // NOTE: this implementation supports only one capability (list of properties) per stream
    if (caps->size != 1) {
        DEBUG_PRINT("Invalid capability size, only one capability supported per stream");
        return MEDIA_ERROR_INVALID_PARAMETER;
    }

    // retrieve stream id
    _stream_id_t stream_id = _get_stream_id(stream_name);
    if (stream_id >= _STREAM_ID_MAX) {
        DEBUG_PRINT("Invalid stream name");
        return MEDIA_ERROR_INVALID_PARAMETER;
    }

    // check if raw input stream capabilities have been set before setting output ones
    if (stream_id != _STREAM_ID_IN_RAW) {
        if (ctx->properties[_STREAM_ID_IN_RAW] == NULL) {
            DEBUG_PRINT("Raw input stream capabilities not set yet");
            return MEDIA_ERROR_INVALID_STATE;
        }
    }

    // check capability validity and store it in the internal context
    properties_t *user_props = *capabilities_get(caps, 0);
    properties_t *requested_props = _check_stream_properties(user_props, stream_id);
    if (requested_props) {
        ctx->properties[stream_id] = requested_props;

        if (stream_id == _STREAM_ID_OUT_DEPTH) {
            const char *depth_format = properties_find(requested_props, "format")->value.val.v_string;
            if (strcmp(depth_format, "ZAPC") == 0) {
                ctx->is_pointcloud_requested = true;
            } else {
                ctx->is_pointcloud_requested = false;
            }
        }

        DEBUG_PRINT("Format: %s, width: %d, height: %d", properties_find(requested_props, "format")->value.val.v_string,
                    properties_find(requested_props, "width")->value.val.v_uint32,
                    properties_find(requested_props, "height")->value.val.v_uint32);
    } else {
        DEBUG_PRINT("Invalid capabilities");
        return MEDIA_ERROR_INVALID_PARAMETER;
    }

    if (_check_streams_consistency(ctx->properties) == false) {
        DEBUG_PRINT("Inconsistent streams capabilities");
        return MEDIA_ERROR_INVALID_STATE;
    }

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _get_streams(_vl53l9_transform_t *self, const streams_t **streams) {
    DEBUG_PRINT("Enter");
    (void)self;
    *streams = &_streams;
    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _get_controls(_vl53l9_transform_t *self, const controls_t **controls) {
    DEBUG_PRINT("Enter");
    (void)self;
    *controls = &_controls;
    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static int _about(_vl53l9_transform_t *self, const properties_t **properties) {
    DEBUG_PRINT("Enter");
    (void)self;
    *properties = &_about_props;
    DEBUG_PRINT("Exit");
    return -0;
}

static int _query_stream_capabilities(_vl53l9_transform_t *self, const char *name, capabilities_t **caps) {
    (void)self;
    (void)name;
    (void)caps;
    return MEDIA_ERROR_UNIMPLEMENTED; // TODO: implement this function
}

static int _query_stream_dependendencies(_vl53l9_transform_t *const self, const char *name,
                                         const strings_t **dependencies) {
    (void)self;
    (void)name;
    (void)dependencies;
    return MEDIA_ERROR_UNIMPLEMENTED; // TODO: implement this function
}

static int _do_process_stream(_vl53l9_transform_t *self, const stream_buffers_t *stream_buffers) {

    DEBUG_PRINT("Enter");

    // retrieve private context associated with the current instance
    _context_t *ctx = &self->ctx;

    // retrieve input and output buffers
    // TODO: use names from the streams_t structure instead of hardcoded strings
    stream_buffer_t *input_stream_buffer = stream_buffers_find(stream_buffers, "raw");
    stream_buffer_t *distance_stream_buffer = stream_buffers_find(stream_buffers, "depth");
    stream_buffer_t *amplitude_stream_buffer = stream_buffers_find(stream_buffers, "amplitude");
    stream_buffer_t *ambient_stream_buffer = stream_buffers_find(stream_buffers, "ambient");
    stream_buffer_t *confidence_stream_buffer = stream_buffers_find(stream_buffers, "confidence");

    // TODO: these flags should be stored in the context and used to determine which outputs need to be computed
    bool is_distance_output_requested = distance_stream_buffer != NULL;
    bool is_amplitude_output_requested = amplitude_stream_buffer != NULL;
    bool is_ambient_output_requested = ambient_stream_buffer != NULL;
    bool is_confidence_output_requested = confidence_stream_buffer != NULL;
    // TODO: add custom output
    // TODO: return error if no output stream is requested

    // make sure capabilities have been previously set for each requested output stream
    if (!input_stream_buffer) {
        DEBUG_PRINT("Raw stream not provided");
        return MEDIA_ERROR_INVALID_STATE;
    }
    if (is_distance_output_requested && !ctx->properties[_STREAM_ID_OUT_DEPTH]) {
        DEBUG_PRINT("Depth stream is requested but capabilities are not set");
        return MEDIA_ERROR_INVALID_STATE;
    }
    if (is_amplitude_output_requested && !ctx->properties[_STREAM_ID_OUT_AMPLITUDE]) {
        DEBUG_PRINT("Amplitude stream is requested but capabilities are not set");
        return MEDIA_ERROR_INVALID_STATE;
    }
    if (is_ambient_output_requested && !ctx->properties[_STREAM_ID_OUT_AMBIENT]) {
        DEBUG_PRINT("Ambient stream is requested but capabilities are not set");
        return MEDIA_ERROR_INVALID_STATE;
    }
    if (is_confidence_output_requested && !ctx->properties[_STREAM_ID_OUT_CONFIDENCE]) {
        DEBUG_PRINT("Confidence stream is requested but capabilities are not set");
        return MEDIA_ERROR_INVALID_STATE;
    }
    // TODO: add custom output

    // TODO: make sure that the input buffer size is consistent with the provided capabilities

    // parse status line to extract metadata
    vl53l9_metadata_t meta;
    // TODO: check list size and returned pointer validity
    memory_t *input_memory = memories_get(input_stream_buffer->buffer.memories, 0);
    _extract_metadata(input_memory->data, input_memory->size, &meta);

    size_t width = meta.crop_enable ? meta.crop_x_size : meta.frame_width;
    size_t height = meta.crop_enable ? meta.crop_y_size : meta.frame_height;
    size_t resolution = width * height;

    // NOTE: calibration maps are allocated and computed during the first frame processing
    if (ctx->is_first_frame) {
        ctx->is_first_frame = false;
        ctx->offset_map = (float *)malloc(resolution * sizeof(float));
        ctx->ref_amp_no_expo = (float *)malloc(resolution * sizeof(float));
        ctx->ref_amp_rad_no_expo = (float *)malloc(resolution * sizeof(float));
        ctx->coeff_norm_no_expo = (float *)malloc(resolution * sizeof(float));
        _compute_calibration_maps(ctx, meta.binning, width, height);
    }

    _process_extract(ctx, &meta, input_memory);
    _process_distance_calibration(ctx, &meta);
    _process_confidence(ctx, &meta);
    _process_ratenorm(ctx, &meta);
    _process_reflectance(ctx, &meta);
    _process_radial_to_perp(ctx, &meta);
    _process_sharpener(ctx, &meta);
    _process_distance_check(ctx, &meta);
    _process_pointcloud(ctx, &meta);

    // fill output data if data is requested

    memory_t *output_memory = NULL;

    // TODO: check list size and returned pointer validity
    // TODO: make sure memories are big enough to hold the data
    if (is_distance_output_requested) {
        output_memory = memories_get(distance_stream_buffer->buffer.memories, 0);
        if (ctx->is_pointcloud_requested) {
            memcpy(output_memory->data, ctx->buffers[_pointcloud], resolution * 4 * sizeof(float));
        } else {
            memcpy(output_memory->data, ctx->buffers[_depth_out], resolution * sizeof(float));
        }
    }
    if (is_amplitude_output_requested) {
        output_memory = memories_get(amplitude_stream_buffer->buffer.memories, 0);
        memcpy(output_memory->data, ctx->buffers[_signal_rate], resolution * sizeof(float));
    }
    if (is_ambient_output_requested) {
        output_memory = memories_get(ambient_stream_buffer->buffer.memories, 0);
        memcpy(output_memory->data, ctx->buffers[_ambient_norm], resolution * sizeof(float));
    }
    if (is_confidence_output_requested) {
        output_memory = memories_get(confidence_stream_buffer->buffer.memories, 0);
        memcpy(output_memory->data, ctx->buffers[_confidence], resolution * sizeof(float));
    }

    // free intermediate buffers
    _process_free_buffers(ctx);

    DEBUG_PRINT("Exit");
    return MEDIA_ERROR_NONE;
}

static media_error_t _get_last_error(_vl53l9_transform_t *self) {
    _context_t *ctx = &self->ctx;
    DEBUG_PRINT("Enter");
    DEBUG_PRINT("Last error code: %d", self->ctx.last_error.code);
    DEBUG_PRINT("Exit");
    return ctx->last_error;
}

/* private functions */

static void _extract_metadata(uint8_t *buffer, size_t size, vl53l9_metadata_t *metadata) {

    unsigned int offset = size - sizeof(vl53l9_metadata_t);
    memcpy(metadata, &buffer[offset], sizeof(vl53l9_metadata_t));
}

static void _compute_calibration_maps(_context_t *ctx, uint8_t binning, size_t width, size_t height) {

    // offset map
    // NOTE: offset map should match input frame size assuming that crop is done at the end of the pipe
    // reorder calibration map (from columns by columns to rows by rows) and convert to float
    float reordered_offset_map[54 * 42] = { 0 };
    for (int i = 0; i < 54; i++) {
        for (int j = 0; j < 42; j++) {
            reordered_offset_map[54 * j + i] = ctx->calib.distance_offset[42 * i + j] + ctx->calib.global_offset;
        }
    }

    // skip bicubic resize if the frame size is the same as the calibration map (nominal binning x2)
    if (width == 54 && height == 42) {
        memcpy(ctx->offset_map, reordered_offset_map, 54 * 42 * sizeof(float));
    } else {
        // TODO: use algo embedded in ratenorm module
        const float bicubic_coeff = -0.75f;
        bicubic_resize(reordered_offset_map, 54, 42, ctx->offset_map, width, height, bicubic_coeff);
    }

    // compute rad2perp coeff map
    radial_to_perp_params_t r2p_params = { 0 };
    r2p_params.efl = R2P_EFL_UM;
    r2p_params.residual_offset_x = ctx->calib.residual_offset_x;
    r2p_params.residual_offset_y = ctx->calib.residual_offset_y;
    r2p_params.max_distance = 8800.0f; // range vs precision
    r2p_params.parallax_correction = false;
    r2p_params.parallax_limit = R2P_PARALLAX_LIMIT;
    r2p_params.alpha = R2P_ALPHA;
    r2p_params.beta = R2P_BETA;
    r2p_params.gamma = R2P_GAMMA;
    r2p_params.kappa = R2P_KAPPA;
    r2p_params.max_spads_x = 216;
    r2p_params.max_spads_y = 168;
    r2p_params.spad_size_um = R2P_SPAD_SIZE_UM;

    float *r2p_1mm_map = (float *)malloc(width * height * sizeof(float));
    float *r2p_coeffs_map = (float *)malloc(width * height * sizeof(float));

    for (size_t i = 0; i < width * height; i++) {
        r2p_1mm_map[i] = 1.0f;
    }

    vl53l9_algo_radial_to_perp(/* input */
                               r2p_1mm_map,

                               /* output */
                               r2p_coeffs_map, NULL, NULL, NULL,

                               /* parameters */
                               r2p_params, width, height, binning);

    // compute ratenorm maps
    // TODO: make sure that only these parameters are used
    ratenorm_params_t ratenorm_params = { 0 };
    ratenorm_params.fast_mode = true;
    ratenorm_params.ref_scaler = ctx->calib.amplitude_scaler;
    ratenorm_params.ref_distance = ctx->calib.amplitude_distance;
    ratenorm_params.ref_reflectance = ctx->calib.amplitude_reflectance;
    ratenorm_params.ref_expo = ctx->calib.amplitude_exposure;
    ratenorm_params.peak_l_scaler = 1.1f;
    ratenorm_params.max_spads = 10.0f; // TODO: for the time being use hard-coded value
    ratenorm_params.max_spads_ref = 10.461f;

    // NOTE: amplitude_coeffs from calib are not reordered since only the max value is needed
    // TODO: temporary workaround to convert amplitude coeffs to float (to be removed when algo signature is updated)
    float *reference_coeffs = (float *)malloc(35 * sizeof(float));
    for (size_t i = 0; i < 35; i++) {
        reference_coeffs[i] = (float)ctx->calib.amplitude_coeffs[i];
    }

    vl53l9_algo_ratenorm_compute_norm_maps(/* input */
                                           reference_coeffs, r2p_coeffs_map,

                                           /* output */
                                           ctx->ref_amp_no_expo, ctx->ref_amp_rad_no_expo, ctx->coeff_norm_no_expo,

                                           /* parameters */
                                           ratenorm_params, width, height, binning);

    // free temporary buffers memory
    free(r2p_1mm_map);
    free(r2p_coeffs_map);
    free(reference_coeffs); // TODO: temporary (to be removed when ratenorm algo is updated)
}

static _stream_id_t _get_stream_id(const char *const stream_name) {

    // TODO: retrieve stream names from stream list
    if (strcmp(stream_name, "raw") == 0) {
        return _STREAM_ID_IN_RAW;
    } else if (strcmp(stream_name, "depth") == 0) {
        return _STREAM_ID_OUT_DEPTH;
    } else if (strcmp(stream_name, "ambient") == 0) {
        return _STREAM_ID_OUT_AMBIENT;
    } else if (strcmp(stream_name, "amplitude") == 0) {
        return _STREAM_ID_OUT_AMPLITUDE;
    } else if (strcmp(stream_name, "confidence") == 0) {
        return _STREAM_ID_OUT_CONFIDENCE;
    } else {
        DEBUG_PRINT("Invalid stream name");
        return _STREAM_ID_MAX;
    }
}

static properties_t *_check_stream_properties(const properties_t *provided_props, _stream_id_t id) {

    capabilities_t *supported_cap_list = NULL;

    switch (id) {
    case _STREAM_ID_IN_RAW:
    case _STREAM_ID_OUT_DEPTH:
    case _STREAM_ID_OUT_AMBIENT:
    case _STREAM_ID_OUT_AMPLITUDE:
    case _STREAM_ID_OUT_CONFIDENCE:
        supported_cap_list = streams_get(&_streams, id)->capabilities;
        DEBUG_PRINT("supported_cap_list size: %d", supported_cap_list->size);
        DEBUG_PRINT("provided_props size: %d", provided_props->size);
        break;
    default:
        DEBUG_PRINT("Invalid stream id");
        return NULL;
    }

    for (size_t i = 0; i < supported_cap_list->size; i++) {

        // TODO: add const qualifier to make sure supported props are not modified
        // NOTE: this implementation makes use only of one list of properties per stream
        properties_t *supported_props = *capabilities_get(supported_cap_list, i);

        if (supported_props->size != provided_props->size) {
            continue;
        }

        // make sure provided format, width and height properties are supported
        property_t *supported_format = properties_find(supported_props, "format");
        property_t *provided_format = properties_find(provided_props, "format");
        if (supported_format == NULL || provided_format == NULL) {
            DEBUG_PRINT("Format property not found");
            return NULL;
        } else if (strcmp(supported_format->value.val.v_string, provided_format->value.val.v_string) != 0) {
            continue;
        }

        property_t *supported_width = properties_find(supported_props, "width");
        property_t *provided_width = properties_find(provided_props, "width");
        if (supported_width == NULL || provided_width == NULL) {
            DEBUG_PRINT("Width property not found");
            return NULL;
        } else if (supported_width->value.val.v_uint32 != provided_width->value.val.v_uint32) {
            continue;
        }

        property_t *supported_height = properties_find(supported_props, "height");
        property_t *provided_height = properties_find(provided_props, "height");
        if (supported_height == NULL || provided_height == NULL) {
            DEBUG_PRINT("Height property not found");
            return NULL;
        } else if (supported_height->value.val.v_uint32 != provided_height->value.val.v_uint32) {
            continue;
        }

        // if a configuration matching the provided one has been found, return it
        DEBUG_PRINT("Matching configuration found, index: %d", i);
        return supported_props;
    }

    // no matching configuration has been found
    return NULL;
}

static bool _check_streams_consistency(properties_t **cap) {

    size_t in_width, in_height;
    size_t out_width, out_height;

    // retrieve width and height associated with the input stream
    in_width = properties_find(cap[_STREAM_ID_IN_RAW], "width")->value.val.v_uint32;
    in_height = properties_find(cap[_STREAM_ID_IN_RAW], "height")->value.val.v_uint32;

    if (((in_width == 100) && (in_height == 149)) || ((in_width == 14842) && (in_height == 1))) {
        out_width = 54;
        out_height = 42;
    } else if (((in_width == 100) && (in_height == 39)) || ((in_width == 3844) && (in_height == 1))) {
        out_width = 24;
        out_height = 20;
    } else {
        DEBUG_PRINT("Invalid input stream size");
        return false;
    }

    // check consistency of output streams if set
    for (unsigned int id = 0; id <= _STREAM_ID_MAX; id++) {
        // TODO: filter streams by direction instead of using switch
        // TODO: add custom output
        switch (id) {
        case _STREAM_ID_OUT_DEPTH:
        case _STREAM_ID_OUT_AMBIENT:
        case _STREAM_ID_OUT_AMPLITUDE:
        case _STREAM_ID_OUT_CONFIDENCE:
            if (cap[id] == NULL) {
                continue;
            }
            uint32_t cap_width = properties_find(cap[id], "width")->value.val.v_uint32;
            uint32_t cap_height = properties_find(cap[id], "height")->value.val.v_uint32;
            if ((cap_width != out_width) || (cap_height != out_height)) {
                DEBUG_PRINT("Output stream size mismatch");
                return false;
            }
            break;
        default:
            break;
        }
    }

    // TODO: raise error if pointcloud is requested and rad2perp is bypassed (still not sure where this should be done)

    return true;
}

// TODO: it would be preferable to move this logic within the confidence algo and only pass the constants
static void _compute_confidence_params(vl53l9_metadata_t *const meta, unsigned int nb_shots[4],
                                       float *const scaling_factor_main, float *const scaling_factor_short,
                                       float *const threshold_main, float *const threshold_short) {

    const float ambient_factor = 1.0f;
    const float ambient_window = 64.0f;
    const float ambient_blanking = 0.0f;
    const float threshold_six = 3.8f;
    const float threshold_six_c = 3.5185f;
    const float threshold_seven = 3.8707f;
    const float threshold_seven_c = 3.5185f;

    float scaling_factor = ambient_factor * (float)(1 << meta->ambient_attenuation) /
                           ((float)nb_shots[3] * (ambient_window + ambient_blanking));

    // compute scaling factor and thresholds depending on number of steps
    if (meta->nb_step == 7) {
        *scaling_factor_main = (float)nb_shots[1] * 4.0f * scaling_factor;
        *scaling_factor_short = (float)nb_shots[2] * 4.0f * scaling_factor;
        *threshold_main = threshold_seven;
        *threshold_short = threshold_seven_c;
    } else {
        *scaling_factor_main = (float)nb_shots[1] * 8.0f * scaling_factor;
        *scaling_factor_short = (float)nb_shots[2] * 8.0f * scaling_factor;
        *threshold_main = threshold_six;
        *threshold_short = threshold_six_c;
    }
}

static void _compute_integration_time(vl53l9_metadata_t *const meta, unsigned int nb_shots[4],
                                      float *const integration_time) {

    const uint16_t seven_step_periods[7] = { 360, 180, 90, 84, 82, 82, 80 };
    const uint16_t six_step_periods[7] = { 360, 180, 90, 84, 80, 84, 80 };

    float expos[7] = { 1.0f * (float)nb_shots[0], 2.0f * (float)nb_shots[0], 4.0f * (float)nb_shots[0], 0, 0, 0, 0 };

    if (meta->nb_step == 7) {
        expos[3] = 8 * (float)nb_shots[0];
        expos[4] = (float)nb_shots[1];
        expos[5] = (float)nb_shots[2];
        expos[6] = (float)nb_shots[3];
    } else {
        expos[3] = (float)nb_shots[1];
        expos[4] = 0;
        expos[5] = (float)nb_shots[2];
        expos[6] = (float)nb_shots[3];
    }

    // hadamard product of matrix and accumulation
    *integration_time = 0.0f;
    float weighted_expos[7] = { 0 };
    for (int i = 0; i < 7; i++) {
        weighted_expos[i] = expos[i] * (meta->nb_step == 7 ? seven_step_periods[i] : six_step_periods[i]);
        *integration_time += weighted_expos[i];
    }

    *integration_time *= 2.0e-6f; // apply 2x factor and convert to us
}

static void _build_nb_shots(vl53l9_metadata_t *const meta, unsigned int nb_shots[4]) {

    nb_shots[0] = meta->nb_shot_step1_lsb | (meta->nb_shot_step1_mid << 8) | (meta->nb_shot_step1_msb << 16);
    nb_shots[1] = meta->nb_shot_step4_5_lsb | (meta->nb_shot_step4_5_mid << 8) | (meta->nb_shot_step4_5_msb << 16);
    nb_shots[2] = meta->nb_shot_step6_lsb | (meta->nb_shot_step6_mid << 8) | (meta->nb_shot_step6_msb << 16);
    nb_shots[3] = meta->nb_shot_step7_lsb | (meta->nb_shot_step7_mid << 8) | (meta->nb_shot_step7_msb << 16);
}

static float _compute_max_spads(vl53l9_calib_data_t *const calib, vl53l9_metadata_t *const meta) {

    float max_dss_coeff = 0.0f;
    float dss_coeffs[] = { 0.015f, 0.16f, 0.106f, 0.22f, 3.0f, 6.0f, 10.0f };

    // TODO: temporary use hardcoded values
    // float *dss_coeffs = (meta->nb_step == 6) ? calib->dss_long_effective_spad : calib->dss_short_effective_spad;
    (void)calib;

    // for (int i = 0; i < 8; i++) { // TODO: double check the number of coeffs when retrieving data from calib
    for (int i = 0; i < 7; i++) {
        if (max_dss_coeff < dss_coeffs[i]) {
            max_dss_coeff = dss_coeffs[i];
        }
    }

    return max_dss_coeff * meta->binning * meta->binning / 4.0f;
}

/* postprocessing components wrappers */

// predecessors: none
static void _process_extract(_context_t *ctx, vl53l9_metadata_t *meta, memory_t *input) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    // TODO: filter dss if not supported by the pixel format
    const unsigned char *raw_in = (const unsigned char *)input->data;
    const float dss_coeffs_in[] = { 0.015f, 0.16f, 0.106f, 0.22f, 3.0f, 6.0f, 10.0f };

    // TODO: temporary use hardcoded values
    // const float *dss_coeffs_in =
    //     (meta->nb_step == 6) ? ctx->calib.dss_long_effective_spad : ctx->calib.dss_short_effective_spad;

    ctx->buffers[_depth_in] = malloc(resolution * sizeof(float));
    ctx->buffers[_amplitude_in] = malloc(resolution * sizeof(float));
    ctx->buffers[_ambient_in] = malloc(resolution * sizeof(float));
    ctx->buffers[_msb_in] = malloc(resolution * sizeof(bool));
    ctx->buffers[_dss_lut_in] = malloc(resolution * sizeof(unsigned char));
    ctx->buffers[_effective_spads] = malloc(resolution * sizeof(float));

    float *depth = (float *)ctx->buffers[_depth_in];
    float *amplitude = (float *)ctx->buffers[_amplitude_in];
    float *ambient = (float *)ctx->buffers[_ambient_in];
    bool *msb = (bool *)ctx->buffers[_msb_in];
    unsigned char *dss_lut_id = (unsigned char *)ctx->buffers[_dss_lut_in];
    float *effective_spads = (float *)ctx->buffers[_effective_spads];

    vl53l9_algo_extract(/* input */
                        raw_in, dss_coeffs_in,

                        /* output */
                        depth, amplitude, ambient, msb, dss_lut_id, effective_spads,

                        /* parameters */
                        meta->frame_width, meta->frame_height, meta->crop_enable, meta->crop_x_offset,
                        meta->crop_y_offset, meta->crop_x_size, meta->crop_y_size, meta->binning);
}

// predecessors: extract
static void _process_confidence(_context_t *ctx, vl53l9_metadata_t *meta) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    // compute exposure from metadata
    unsigned int nb_shots[4]; // steps 1, 4-5, 6, 7
    float integration_time;
    _build_nb_shots(meta, nb_shots);
    _compute_integration_time(meta, nb_shots, &integration_time);

    // fill params structure
    confidence_params_t params = { 0 };
    params.cover_glass = ctx->controls[_CONTROL_ID_COVER_GLASS].val.v_bool;
    params.signal_factor = SIGNAL_FACTOR;
    params.xtalk_coeff = 0.9f * integration_time; // NOTE: threshold scaler set to 1.0f in both cover glass cases
    _compute_confidence_params(meta, nb_shots, &params.scaling_factor_main, &params.scaling_factor_short,
                               &params.threshold_main, &params.threshold_short);

    float *ambient = (float *)ctx->buffers[_ambient_in];
    float *amplitude = (float *)ctx->buffers[_amplitude_in];
    bool *msb = (bool *)ctx->buffers[_msb_in];
    float *effective_spads = (float *)ctx->buffers[_effective_spads];
    float *noise_reduction = NULL; // TODO: TNR output (if not bypassed), ignored if set to null

    ctx->buffers[_confidence] = malloc(resolution * sizeof(float));
    ctx->buffers[_validity_confidence] = malloc(resolution * sizeof(bool));
    // TODO: these buffers are not used in the pipeline, but needs to be allocated for internal computations, will be
    // improved in the future to reduce memory footprint
    ctx->buffers[_threshold_confidence] = malloc(resolution * sizeof(float));
    ctx->buffers[_xtalk_estimated] = malloc(resolution * sizeof(float));

    if (ctx->is_pointcloud_requested) {
        ctx->buffers[_android_confidence] = malloc(resolution * sizeof(unsigned char));
    } else {
        ctx->buffers[_android_confidence] = NULL;
    }

    float *confidence = (float *)ctx->buffers[_confidence];
    float *threshold_confidence = (float *)ctx->buffers[_threshold_confidence];
    bool *validity_confidence = (bool *)ctx->buffers[_validity_confidence];
    float *xtalk_estimated = (float *)ctx->buffers[_xtalk_estimated];
    unsigned char *android_confidence = (unsigned char *)ctx->buffers[_android_confidence];

    vl53l9_algo_confidence(/* input */
                           ambient, amplitude, msb, effective_spads, noise_reduction,

                           /* output */
                           confidence, threshold_confidence, validity_confidence, xtalk_estimated, android_confidence,

                           /* parameters */
                           params, resolution);
}

// predecessors: extract
static void _process_distance_calibration(_context_t *ctx, vl53l9_metadata_t *meta) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    distance_calibration_params_t params = {
        .gain_correction = 0.0f,
        .pileup_base = 6,
        .pileup_rate_point = 60e6f,
        .pileup_base_short = 1,
        .pileup_short_dist = 100.0f,
        .nlc_mode = 2,
        .constant_prec = 0,
        .constant_range = 0,
        .lut_prec = { -13, -2, -2, 6, 6, 6, 3, -9 },
        .lut_range = { -7, 1, 1, 5, 5, 5, -7, -7 },
    };

    // prepare output buffers
    ctx->buffers[_depth_calibrated] = malloc(resolution * sizeof(float));

    float *depth_in = (float *)ctx->buffers[_depth_in];
    unsigned char *dss_lut_in = (unsigned char *)ctx->buffers[_dss_lut_in];

    // NOTE: if nlc_mode = 3, then ratenorm must be executed before distance calibration
    // NOTE: if nlc_mode != 3, signal_rate_in and ambient_rate_in are not used
    float *signal_rate_in = NULL;
    float *ambient_rate_in = NULL;

    float *depth_calibrated = (float *)ctx->buffers[_depth_calibrated];

    vl53l9_algo_distance_calibration(/* input */
                                     depth_in, ctx->offset_map, signal_rate_in, ambient_rate_in, dss_lut_in,

                                     /* output */
                                     depth_calibrated,

                                     /* parameters */
                                     params, resolution, meta->nb_step);
}

// predecessors: extract
static void _process_ratenorm(_context_t *ctx, vl53l9_metadata_t *meta) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    // prepare parameters
    ratenorm_params_t params = { 0 };
    params.fast_mode = true;
    params.ref_mode = 0;
    params.ref_scaler = ctx->calib.amplitude_scaler;
    params.ref_distance = ctx->calib.amplitude_distance;
    params.ref_reflectance = ctx->calib.amplitude_reflectance;
    params.ref_expo = ctx->calib.amplitude_exposure;
    params.ref_amp_const = 275;
    params.nominal_width = 54;
    params.nominal_height = 42;
    params.nominal_binning = 2;
    params.ambient_window = 64.0f;
    params.ambient_blanking = 0.0f;
    params.signal_factor = SIGNAL_FACTOR;
    params.main_scaler = 1.0f;
    params.side_scaler = 0.725f;
    params.corner_scaler = 0.4f;
    params.left_side_l_scaler = 1.1f;
    params.right_side_l_scaler = 1.1f;
    params.middle_section_l_scaler = 1.1f;
    params.center_region_l_scaler = 1.125f;
    params.peak_l_scaler = 1.1f;
    params.top_row_l_scaler = 1.1f;
    params.bottom_row_l_scaler = 1.1f;
    params.corner_post_l_scaler = 1.05f;
    params.exposure_width_0 = 1.3f;
    params.exposure_width_1 = 2.6f;
    params.max_spads = _compute_max_spads(&ctx->calib, meta);
    params.max_spads_ref = 10.461f;
    params.bicubic_coef = -0.75f;

    // TODO: define constants for the indices
    unsigned int nb_shots[4]; // steps 1, 4-5, 6, 7
    _build_nb_shots(meta, nb_shots);
    unsigned int expo_sf = nb_shots[1];
    unsigned int expo_sc = nb_shots[2];
    unsigned int expo_sa = nb_shots[3];

    // prepare input
    float *amplitude = (float *)ctx->buffers[_amplitude_in];
    float *ambient = (float *)ctx->buffers[_ambient_in];
    bool *msb = (bool *)ctx->buffers[_msb_in];
    float *effective_spads = (float *)ctx->buffers[_effective_spads];

    // prepare outputs
    // TODO: keep only the required outputs by following blocks to save memory
    ctx->buffers[_amplitude_ref] = malloc(resolution * sizeof(float)); // TODO: make sure it's needed
    ctx->buffers[_amplitude_ref_rad] = malloc(resolution * sizeof(float));
    ctx->buffers[_signal_rate] = malloc(resolution * sizeof(float));
    ctx->buffers[_ambient_norm] = malloc(resolution * sizeof(float));
    // NOTE: required by dist calib when nlc_mode = 3 or smart integration
    ctx->buffers[_ambient_rate] = malloc(resolution * sizeof(float));

    float *amplitude_ref = (float *)ctx->buffers[_amplitude_ref];
    float *amplitude_ref_rad = (float *)ctx->buffers[_amplitude_ref_rad];
    float *signal_rate = (float *)ctx->buffers[_signal_rate];
    float *ambient_rate = (float *)ctx->buffers[_ambient_rate];
    float *ambient_norm = (float *)ctx->buffers[_ambient_norm];

    vl53l9_algo_ratenorm_compute_rates(/* input */
                                       amplitude, ambient, effective_spads, msb, ctx->ref_amp_no_expo,
                                       ctx->ref_amp_rad_no_expo, ctx->coeff_norm_no_expo,

                                       /* output */
                                       amplitude_ref, amplitude_ref_rad, signal_rate, ambient_rate, ambient_norm,

                                       /* parameters */
                                       params, resolution, meta->nb_step, expo_sf, expo_sc, expo_sa,
                                       meta->ambient_attenuation);
}

// predecessors: distance calibration, rate norm
static void _process_reflectance(_context_t *ctx, vl53l9_metadata_t *meta) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    reflectance_params_t params = { 0 };
    params.max_spads = _compute_max_spads(&ctx->calib, meta);
    params.min_refl_thr = 1.5f;
    params.max_refl_thr = 200.0f;
    params.correction_factor = 1.05f; // NOTE: coefficient specific to this implementation
    params.sq_law_exponent = 1.995f;
    params.six_step_scaler = 2.9f;
    params.cutoff_distance = 25.0f;
    params.cover_glass = ctx->controls[_CONTROL_ID_COVER_GLASS].val.v_bool;

    // TODO: define constants for the indices
    unsigned int nb_shots[4]; // steps 1, 4-5, 6, 7
    _build_nb_shots(meta, nb_shots);
    unsigned int expo_sf = nb_shots[1];
    unsigned int expo_sc = nb_shots[2];

    // prepare input
    float *distance = (float *)ctx->buffers[_depth_calibrated];
    float *amplitude = (float *)ctx->buffers[_amplitude_in];
    bool *msb = (bool *)ctx->buffers[_msb_in];
    float *effective_spads = (float *)ctx->buffers[_effective_spads];
    float *amplitude_ref_rad = (float *)ctx->buffers[_amplitude_ref_rad];

    // prepare outputs
    ctx->buffers[_reflectance] = malloc(resolution * sizeof(float));
    ctx->buffers[_validity_low_refl] = malloc(resolution * sizeof(bool));

    float *reflectance = (float *)ctx->buffers[_reflectance];
    bool *low_refl_validity = (bool *)ctx->buffers[_validity_low_refl];
    bool *high_refl_validity = NULL; // required only for validation purposes

    vl53l9_algo_reflectance(/* input */
                            distance, amplitude, msb, effective_spads, amplitude_ref_rad,

                            /* output */
                            reflectance, low_refl_validity, high_refl_validity,

                            /* parameters */
                            params, resolution, expo_sf, expo_sc, meta->nb_step);
}

// predecessors: distance calibration
static void _process_radial_to_perp(_context_t *ctx, vl53l9_metadata_t *meta) {

    // NOTE: bypassed algorithm is a pass-through
    if (ctx->controls[_CONTROL_ID_BYPASS_R2P_ALGO].val.v_bool) {
        // TODO: allocate memory for _depth_r2p and then copy _depth_calibrated to it instead of using the same
        // pointer
        ctx->buffers[_depth_r2p] = ctx->buffers[_depth_calibrated];
        ctx->buffers[_distortion_r2p] = NULL;
        ctx->buffers[_center_x_r2p] = NULL;
        ctx->buffers[_validity_r2p] = NULL;
        return;
    }

    size_t width = (meta->crop_enable) ? (meta->crop_x_size) : (meta->frame_width);
    size_t height = (meta->crop_enable) ? (meta->crop_y_size) : (meta->frame_height);
    size_t resolution = width * height;

    // prepare parameters
    radial_to_perp_params_t params = { 0 };
    params.efl = R2P_EFL_UM;
    params.residual_offset_x = ctx->calib.residual_offset_x;
    params.residual_offset_y = ctx->calib.residual_offset_y;
    params.max_distance = (meta->nb_step == 6) ? 8500.0f : 8800.0f; // range vs precision
    params.parallax_correction = true;
    params.parallax_limit = R2P_PARALLAX_LIMIT;
    params.alpha = R2P_ALPHA;
    params.beta = R2P_BETA;
    params.gamma = R2P_GAMMA;
    params.kappa = R2P_KAPPA;
    params.max_spads_x = 216;
    params.max_spads_y = 168;
    params.spad_size_um = R2P_SPAD_SIZE_UM;

    // allocate outputs
    ctx->buffers[_depth_r2p] = malloc(resolution * sizeof(float));
    ctx->buffers[_validity_r2p] = malloc(resolution * sizeof(bool));

    if (ctx->is_pointcloud_requested) {
        ctx->buffers[_center_x_r2p] = malloc(resolution * sizeof(float));
        ctx->buffers[_distortion_r2p] = malloc(resolution * sizeof(float));
    } else {
        ctx->buffers[_center_x_r2p] = NULL;
        ctx->buffers[_distortion_r2p] = NULL;
    }

    // set inputs and outputs
    float *depth_calibrated = (float *)ctx->buffers[_depth_calibrated];
    float *depth_r2p = (float *)ctx->buffers[_depth_r2p];
    float *center_x_r2p = (float *)ctx->buffers[_center_x_r2p];
    float *distortion_r2p = (float *)ctx->buffers[_distortion_r2p];
    bool *validity_r2p = (bool *)ctx->buffers[_validity_r2p];

    vl53l9_algo_radial_to_perp(/* input */
                               depth_calibrated,

                               /* output */
                               depth_r2p, center_x_r2p, distortion_r2p, validity_r2p,

                               /* parameters */
                               params, width, height, meta->binning);
}

// predecessors: distance calibration
static void _process_sharpener(_context_t *ctx, vl53l9_metadata_t *meta) {

    // handle bypass
    if (ctx->controls[_CONTROL_ID_BYPASS_SHARPENER_ALGO].val.v_bool) {
        ctx->buffers[_validity_sharpener] = NULL;
        ctx->buffers[_sharpener_score] = NULL;
        return;
    }

    size_t width = (meta->crop_enable) ? (meta->crop_x_size) : (meta->frame_width);
    size_t height = (meta->crop_enable) ? (meta->crop_y_size) : (meta->frame_height);

    sharpener_params_t params = { 0 };
    params.invalid_distance = 12000.0f;
    params.min_range_threshold_mm = 300.0f;
    params.scale_range_threshd_by_range = true;
    params.range_threshold_factor = 0.3f;
    params.enable_max_range_threshold = true;
    params.max_range_threshold_mm_6_step = 1200.0f;
    params.max_range_threshold_mm_7_step = 600.0f;
    params.enable_distance = true;
    params.enable_gaussian = true;
    params.channel_ratio = 19.23f;
    params.sigma_factor = 0.8f;
    params.distance_power = 0.1f;
    params.signal_threshold_factor = 0.05f;
    params.threshold_includes_glare = false;
    params.glare_ratio = 0.0000138871530f;
    params.leak_shift_range_grouping = 3;
    params.nb_lines_overlap = 1;

    // allocate outputs
    ctx->buffers[_validity_sharpener] = malloc(width * height * sizeof(bool));
    ctx->buffers[_sharpener_score] = NULL; // TODO: not required yet, should be allocated if needed

    // set inputs and outputs
    float *depth = (float *)ctx->buffers[_depth_calibrated];
    float *signal = (float *)ctx->buffers[_signal_rate];
    bool *validity_sharpener = (bool *)ctx->buffers[_validity_sharpener];
    float *sharpener_score = (float *)ctx->buffers[_sharpener_score];

    vl53l9_algo_sharpener(/* input */
                          depth, signal,

                          /* output */
                          validity_sharpener, sharpener_score,

                          /* parameters */
                          params, width, height, meta->nb_step);
}

// predecessors: radial to perp, confidence, reflectance, sharpener
static void _process_distance_check(_context_t *ctx, vl53l9_metadata_t *meta) {

    size_t resolution =
        (meta->crop_enable) ? (meta->crop_x_size * meta->crop_y_size) : (meta->frame_width * meta->frame_height);

    const float invalid_distance_default = 12000.0f;

    ctx->buffers[_depth_out] = (float *)malloc(resolution * sizeof(float));
    ctx->buffers[_status_out] = (unsigned char *)malloc(resolution * sizeof(unsigned char));

    float *depth_r2p = (float *)ctx->buffers[_depth_r2p];
    bool *validity_r2p = (bool *)ctx->buffers[_validity_r2p];
    bool *validity_confidence = (bool *)ctx->buffers[_validity_confidence];
    const bool *validity_reflectance = (bool *)ctx->buffers[_validity_low_refl];
    const bool *validity_sharpener = (bool *)ctx->buffers[_validity_sharpener];
    const bool *fp_validity = NULL; // TODO: not implemented yet
    const float *dmax = NULL;       // TODO: not implemented yet

    float *depth_out = (float *)ctx->buffers[_depth_out];
    unsigned char *status_out = (unsigned char *)ctx->buffers[_status_out];

    // TODO: use helper instead of direct access
    bool r2p_filter = !ctx->controls[_CONTROL_ID_BYPASS_R2P_FILTER].val.v_bool;
    bool confidence_filter = !ctx->controls[_CONTROL_ID_BYPASS_CONFIDENCE_FILTER].val.v_bool;
    bool reflectance_filter = !ctx->controls[_CONTROL_ID_BYPASS_REFLECTANCE_FILTER].val.v_bool;
    bool sharpener_filter = !ctx->controls[_CONTROL_ID_BYPASS_SHARPENER_FILTER].val.v_bool;
    bool fp_filter = false;   // TODO: not implemented yet
    bool dmax_select = false; // TODO: not implemented yet

    vl53l9_algo_distance_check(/* input */
                               depth_r2p, dmax, validity_r2p, validity_confidence, validity_reflectance,
                               validity_sharpener, fp_validity,

                               /* output */
                               depth_out, status_out,

                               /* parameters */
                               resolution, invalid_distance_default, r2p_filter, confidence_filter, reflectance_filter,
                               sharpener_filter, fp_filter, dmax_select);
}

static void _process_pointcloud(_context_t *ctx, vl53l9_metadata_t *meta) {

    if (!ctx->is_pointcloud_requested) {
        return;
    }

    size_t width = (meta->crop_enable) ? (meta->crop_x_size) : (meta->frame_width);
    size_t height = (meta->crop_enable) ? (meta->crop_y_size) : (meta->frame_height);
    size_t resolution = width * height;

    // prepare parameters
    radial_to_perp_params_t params = { 0 };
    params.efl = R2P_EFL_UM;
    params.residual_offset_x = ctx->calib.residual_offset_x;
    params.residual_offset_y = ctx->calib.residual_offset_y;
    params.max_distance = (meta->nb_step == 6) ? 8500.0f : 8800.0f; // range vs precision
    params.parallax_correction = true;
    params.parallax_limit = R2P_PARALLAX_LIMIT;
    params.alpha = R2P_ALPHA;
    params.beta = R2P_BETA;
    params.gamma = R2P_GAMMA;
    params.kappa = R2P_KAPPA;
    params.max_spads_x = 216;
    params.max_spads_y = 168;
    params.spad_size_um = R2P_SPAD_SIZE_UM;

    // allocate outputs
    ctx->buffers[_pointcloud] = malloc(resolution * 4 * sizeof(float)); // (x, y, z, confidence)

    // set inputs
    float *depth = (float *)ctx->buffers[_depth_out];
    float *center_x = (float *)ctx->buffers[_center_x_r2p];
    float *distorsion = (float *)ctx->buffers[_distortion_r2p];
    unsigned char *confidence = (unsigned char *)ctx->buffers[_android_confidence];

    // set outputs
    float *pointcloud = (float *)ctx->buffers[_pointcloud];

    vl53l9_algo_pointcloud(/* input */
                           depth, confidence, center_x, distorsion,

                           /* output */
                           pointcloud,

                           /* parameters */
                           params, width, height, meta->binning);
}

static void _process_free_buffers(_context_t *ctx) {
    for (size_t i = 0; i < _nb_buffers; i++) {
        if (ctx->buffers[i] != NULL) {
            free(ctx->buffers[i]);
            ctx->buffers[i] = NULL;
        }
    }
}
