/**
 *******************************************************************************
 * @file   postprocess.c
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

#include "vl53l9.h"
#include "vl53l9_device.h"
#include "vl53l9_transform.h"
#include "vl53l9_utils.h"

#define DEVICE_ID 0

static memory_t allocate_memory(uint16_t size);
static void handle_error(void);

int vl53l9_app() {

    int ret;
    transform_t *p_transform = vl53l9_transform_create();
    vl53l9_device_t *p_dev = &device[DEVICE_ID];
    vl53l9_profile_t *p_profile = &g_ranging_profiles[VL53L9_USECASE_AR_RANGE];

    uint16_t raw_buffer_size, frame_buffer_size; // bytes
    uint8_t width = 0, height = 0;               // pixels
    vl53l9_get_raw_buffer_size(p_profile->binning, &raw_buffer_size);
    vl53l9_utils_get_resolution(p_profile->binning, &width, &height);
    frame_buffer_size = width * height * sizeof(float);

    // sensor reset
    platform_power_reset(DEVICE_ID);
    if (p_dev->bus_type & PLATFORM_BUS_I3C) {
        platform_assign_dynamic_address();
    }

    // initialize sensor and retrieve calibration data
    ret = vl53l9_init(p_dev);
    if (ret) {
        handle_error();
    }

    uint8_t calib_data[VL53L9_CALIB_DATA_SIZE];
    ret = vl53l9_get_calib_data(p_dev, calib_data);
    if (ret) {
        handle_error();
    }

    platform_set_profile(p_dev, p_profile);

    // initialize processing pipeline
    ret = transform_initialize(p_transform);
    if (ret) {
        handle_error();
    }

    // inspect available streams and controls
    const streams_t *stream_list;
    transform_get_streams(p_transform, &stream_list);
    streams_inspect(stream_list, printf);

    const controls_t *control_list;
    transform_get_controls(p_transform, &control_list);
    controls_inspect(control_list, printf);

    // set capabilities

    /**
     * NOTE:
     * setting capabilities is a mandatory step:
     *  - at least one input and one output stream must be set
     *  - input stream must be configured before output ones
     *  - there are no default capabilities, they must be explicitly set
     */

    // build raw stream capabilities
    uint32_t in_width, in_height, out_width, out_height;
    if (p_profile->binning == 2) {
        in_width = 100;
        in_height = 149;
        out_width = 54;
        out_height = 42;
    } else if (p_profile->binning == 4) {
        in_width = 100;
        in_height = 39;
        out_width = 24;
        out_height = 20;
    } else {
        // unsupported binning
        handle_error();
    }

    property_t raw_format = { "format", { .val.v_string = "3DMD", .tid = VTID_STRING } };
    property_t raw_width = { "width", { .val.v_uint32 = in_width, .tid = VTID_UINT32 } };
    property_t raw_height = { "height", { .val.v_uint32 = in_height, .tid = VTID_UINT32 } };

    properties_t *raw_props = properties_new(3); // format, width, height
    properties_add(raw_props, &raw_format);
    properties_add(raw_props, &raw_width);
    properties_add(raw_props, &raw_height);
    capabilities_t *raw_caps = capabilities_new_simple(&raw_props);

    // build depth stream capabilities
    property_t depth_format = { "format", { .val.v_string = "ZF32", .tid = VTID_STRING } };
    property_t depth_width = { "width", { .val.v_uint32 = out_width, .tid = VTID_UINT32 } };
    property_t depth_height = { "height", { .val.v_uint32 = out_height, .tid = VTID_UINT32 } };

    properties_t *depth_props = properties_new(3); // format, width, height
    properties_add(depth_props, &depth_format);
    properties_add(depth_props, &depth_width);
    properties_add(depth_props, &depth_height);
    capabilities_t *depth_caps = capabilities_new_simple(&depth_props);

    // set stream capabilities
    ret = transform_set_stream_capabilities(p_transform, "raw", raw_caps);
    if (ret) {
        handle_error();
    }

    ret = transform_set_stream_capabilities(p_transform, "depth", depth_caps);
    if (ret) {
        handle_error();
    }

    // free properties and capabilities
    // TODO: improve using free functions
    properties_free(raw_props, NULL);
    properties_free(depth_props, NULL);
    capabilities_free(raw_caps, NULL);
    capabilities_free(depth_caps, NULL);

    // set controls

    // NOTE: the following control is mandatory and must be set before calling prepare()
    ret = transform_set_control(p_transform, "calib-buffer", (value_t){ .val.v_ptr = calib_data, .tid = VTID_POINTER });
    if (ret) {
        handle_error();
    }

    // check pipeline configuration and compute internal parameters required for processing
    ret = transform_prepare(p_transform);
    if (ret) {
        handle_error();
    }

    // allocate memory and initialize buffers (raw data is double buffered)
    // TODO: double buffering should be achieved through 2x buffer_t (not memory_t)
    uint8_t raw_mem_index = 0;
    memory_t in_raw_mem[2] = { allocate_memory(raw_buffer_size), allocate_memory(raw_buffer_size) };
    memory_t out_depth_mem = allocate_memory(frame_buffer_size);

    memories_t in_raw_mems = { .items = &in_raw_mem, .size = 1, .capacity = 1, .item_size = sizeof(memory_t) };
    memories_t out_depth_mems = { .items = &out_depth_mem, .size = 1, .capacity = 1, .item_size = sizeof(memory_t) };

    stream_buffer_t in_raw_stream_buffer = { .name = "raw", .buffer = { .memories = &in_raw_mems, .nb = 1 } };
    stream_buffer_t out_depth_stream_buffer = { .name = "depth", .buffer = { .memories = &out_depth_mems, .nb = 1 } };

    // build stream buffers container
    stream_buffers_t stream_buffers = { .items =
                                            (stream_buffer_t[]){
                                                in_raw_stream_buffer,
                                                out_depth_stream_buffer,
                                            },
                                        .size = 2,
                                        .capacity = 2,
                                        .item_size = sizeof(stream_buffer_t) };

    ret = vl53l9_set_sync_mode(p_dev, VL53L9_SYNC_MANUAL);
    if (ret) {
        handle_error();
    }

    ret = vl53l9_start(p_dev);
    if (ret) {
        handle_error();
    }

    platform_profiler_enable();
    uint32_t start_time = platform_profiler_get_timestamp();
    uint32_t stop_time;
    float frame_rate;

    bool is_first_frame = true;

    while (1) {

        vl53l9_trigger_frame(p_dev);
        if (ret) {
            handle_error();
        }

        ret = platform_wait_for_event(PLATFORM_GPIO_IT_EVT, 1000);
        if (ret) {
            handle_error();
        }

        platform_acknowledge_event(PLATFORM_GPIO_IT_EVT);

        // grab raw data from sensor and fill input buffer
        ret = vl53l9_get_frame_async(p_dev, in_raw_mem[raw_mem_index].data, in_raw_mem[raw_mem_index].size);
        if (ret) {
            handle_error();
        }

        // process the previous frame while the sensor is acquiring the next one
        if (is_first_frame) {
            is_first_frame = false;
        } else {
            // TODO: find a better way to handle this, maybe leveraging mems list
            in_raw_mems.items = &in_raw_mem[(raw_mem_index + 1) % 2];
            ret = transform_process_stream(p_transform, &stream_buffers);
            if (ret) {
                handle_error();
            }
        }

        ret = platform_wait_for_event(PLATFORM_I3C_DMA_RX_EVT, 1000);
        if (ret) {
            handle_error();
        }
        platform_acknowledge_event(PLATFORM_I3C_DMA_RX_EVT);

        ret = vl53l9_get_frame_async_ack(p_dev, in_raw_mem[raw_mem_index].data, in_raw_mem[raw_mem_index].size);
        if (ret) {
            handle_error();
        }

        // TODO: to be moved below but avoid printing for first frame
        vl53l9_frame_t frame = { 0 };
        ret = vl53l9_utils_parse_frame(in_raw_mem[raw_mem_index].data, in_raw_mem[raw_mem_index].size, &frame);
        if (ret) {
            handle_error();
        }

        raw_mem_index = (raw_mem_index + 1) % 2;

        // measure frame rate
        stop_time = platform_profiler_get_timestamp();
        frame_rate = (1.0f / (float)(platform_profiler_convert_to_us(stop_time - start_time))) * 1000000;
        start_time = stop_time;
        printf("Processed frame n. %lu @ %u fps\n", frame.p_metadata->frame_counter, (unsigned int)frame_rate);
    }

    // free memory and pipeline resources
    free(in_raw_mem[0].data);
    free(in_raw_mem[1].data);
    free(out_depth_mem.data);
    transform_finalize(p_transform);
    transform_release(p_transform);
    vl53l9_transform_destroy(p_transform);

    return 0;
}

static memory_t allocate_memory(uint16_t size) {
    memory_t memory;
    memory.size = size;
    memory.data = malloc(size);
    if (memory.data == NULL) {
        handle_error();
    }
    return memory;
}

static void handle_error(void) {
    vl53l9_status_t status = { 0 };
    vl53l9_get_status(&device[DEVICE_ID], &status);
    while (1)
        ;
}
