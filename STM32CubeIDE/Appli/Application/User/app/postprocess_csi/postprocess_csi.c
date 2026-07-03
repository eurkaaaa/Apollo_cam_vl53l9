#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "interface.h"

#include "vl53l9.h"
#include "vl53l9_device.h"
#include "vl53l9_transform.h"
#include "vl53l9_utils.h"

#define DEVICE_ID 0

static memory_t allocate_memory(uint16_t size);
static void handle_error(void);

__attribute__((aligned(32))) volatile uint32_t output_buff[3725];

// dump frame form csi to avoid overwrite and drop csi padding
static int dump_csi_frame(vl53l9_frame_t *p_frame, uint16_t resolution, uint8_t *p_buffer, size_t *buffer_size) {
    void *ptr_cursor = p_buffer;
    uint16_t frame_size =
        resolution * (sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + 0.5) + sizeof(vl53l9_metadata_t);
    if (frame_size > *buffer_size) {
        // output buffer to small
        return -1;
    }

    memcpy(ptr_cursor, p_frame->p_distance, sizeof(uint16_t) * resolution);
    ptr_cursor += resolution * sizeof(uint16_t);
    memcpy(ptr_cursor, p_frame->p_amplitude, sizeof(uint16_t) * resolution);
    ptr_cursor += resolution * sizeof(uint16_t);
    memcpy(ptr_cursor, p_frame->p_ambient, sizeof(uint16_t) * resolution);
    ptr_cursor += resolution * sizeof(uint16_t);
    memcpy(ptr_cursor, p_frame->p_dss_lut, 0.5 * resolution);
    ptr_cursor += (uint32_t)(0.5 * resolution);
    memcpy(ptr_cursor, p_frame->p_metadata, sizeof(vl53l9_metadata_t));
    ptr_cursor += sizeof(vl53l9_metadata_t);

    if ((ptr_cursor - (void *)p_buffer) != frame_size) {
        return -1;
    }
    *buffer_size = frame_size;

    return 0;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


const char ASCII_CHARS[] = "@%#*+=-:. ";

void print_transform_frame(uint8_t width, uint8_t height, memory_t out_depth_buffer, uint32_t frame_counter,
                           float frame_rate) {
    // set to the top of the screen
    printf("\033[%d;%dH", 0, 0);
    printf("Frame n. %lu @ %u fps\n", frame_counter, (unsigned int)frame_rate);
    int pixel_step = 1;
    float min = FLT_MAX;
    float max = FLT_MIN;
    float value;

    for (int i = 0; i < out_depth_buffer.size; i++) {
        value = ((float *)out_depth_buffer.data)[i];
        min = MIN(value, min);
        max = MAX(value, max);
    }

    uint16_t average = (max - min) * 0.05;
    min = MAX(min - average, 0);
    max = MIN(max + average, UINT16_MAX);

    for (int y = 0; y < height; y += pixel_step) {
        for (int x = 0; x < width; x += pixel_step) {
            int pixel_index = (y * width + x);
            value = ((float *)out_depth_buffer.data)[pixel_index];

            int ascii_index = (value - min) * (sizeof(ASCII_CHARS) - 1) / (max - min);
            ascii_index = MAX(0, MIN(ascii_index, sizeof(ASCII_CHARS) - 1));

            printf("%c", ASCII_CHARS[ascii_index]);
			//printf("%.0f ", value);
        }
        printf("\n");
    }
}

void print_pointcloud_frame(uint8_t width, uint8_t height, memory_t out_pointcloud_buffer, uint32_t frame_counter, float frame_rate) {
    // set to the top of the screen
    printf("\033[%d;%dH", 0, 0);
    printf("Frame n. %lu @ %u fps\n", frame_counter, (unsigned int)frame_rate);
    int pixel_step = 1;
    float x_val, y_val, z_val, conf_val;

    for (int y = 0; y < height; y += pixel_step) {
        for (int x = 0; x < width; x += pixel_step) {
            int pixel_index = (y * width + x) * 4; // 4 floats per pixel
            float *pointcloud = (float *)out_pointcloud_buffer.data;
            x_val   = pointcloud[pixel_index + 0];
            y_val   = pointcloud[pixel_index + 1];
            z_val   = pointcloud[pixel_index + 2];
            conf_val= pointcloud[pixel_index + 3];
            printf("(%d,%d): x=%.0f, y=%.0f, z=%.0f, conf=%.0f\n", x, y, x_val, y_val, z_val, conf_val);
        }
    }
}


int vl53l9_app() {

    int ret;
    transform_t *p_transform = vl53l9_transform_create();
    vl53l9_device_t *p_dev = &device[DEVICE_ID];
    vl53l9_profile_t *p_profile = &g_ranging_profiles[VL53L9_USECASE_AR_RANGE];
    p_profile->power = VL53L9_POWER_REGULAR;
    vl53l9_hw_config_t hw_config;

    uint16_t raw_buffer_size, frame_buffer_size; // bytes
    uint8_t width = 0, height = 0;               // pixels
    vl53l9_get_raw_buffer_size(p_profile->binning, &raw_buffer_size);
    vl53l9_utils_get_resolution(p_profile->binning, &width, &height);
    frame_buffer_size = width * height * sizeof(float);
    uint16_t resolution = width * height;

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

    ret = vl53l9_get_hw_config(p_dev, &hw_config);
    if (ret) {
        handle_error();
    }
    hw_config.output_interface = VL53L9_OUTPUT_CSI2;
    hw_config.signaling_mode = true;
    hw_config.csi_data_rate = 1e9;
    hw_config.csi_virtual_channel = 0;
    hw_config.csi_status_line_force_width = false;
    hw_config.csi_status_line_datatype = 0x2A;
    hw_config.csi_frame_datatype = 0x2A;
    hw_config.csi_frame_height = 148;
    hw_config.csi_frame_width = 100;

    ret = vl53l9_set_hw_config(p_dev, hw_config);
    if (ret) {
        handle_error();
    }
    ret = platform_start_csi_pipe((uint8_t *)output_buff);
    if (ret) {
        handle_error();
    }

    // initialize processing pipeline
    ret = transform_initialize(p_transform);
    if (ret) {
        handle_error();
    }

    // inspect available streams, capabilities and controls
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
    property_t depth_format = { "format", { .val.v_string = "ZAPC", .tid = VTID_STRING } };
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

    // NOTE: the following controls are mandatory and must be set before calling prepare()
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
    memory_t in_raw_mem = allocate_memory(raw_buffer_size);
    memory_t out_depth_mem = allocate_memory(frame_buffer_size * 4);

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

    ret = vl53l9_start(p_dev);
    if (ret) {
        handle_error();
    }

    platform_profiler_enable();
    uint32_t start_time = platform_profiler_get_timestamp();
    uint32_t stop_time;
    float frame_rate;

    vl53l9_frame_t frame;
    uint32_t previous_frame_counter = 0;

    while (1) {

        ret = platform_wait_for_event(PLATFORM_CAM_PIPE_FRAME_EVT, 1000);
        if (ret) {
            handle_error();
        }

        platform_acknowledge_event(PLATFORM_CAM_PIPE_FRAME_EVT);

        ret = vl53l9_utils_parse_frame((uint8_t *)output_buff, sizeof(output_buff), &frame);
        if (ret) {
            handle_error();
        }
        // copy frame and skip csi padding
        ret = dump_csi_frame(&frame, resolution, (uint8_t *)in_raw_mem.data, &in_raw_mem.size);
        if (ret) {
            handle_error();
        }
        // process the previous frame while the sensor is acquiring the next one
        ret = transform_process_stream(p_transform, &stream_buffers);
        if (ret) {
            handle_error();
        }

        // measure frame rate
        stop_time = platform_profiler_get_timestamp();
        frame_rate = (1.0f / (float)(platform_profiler_convert_to_us(stop_time - start_time))) * 1000000;
        start_time = stop_time;

		//print_transform_frame(width, height, out_depth_mem, frame.p_metadata->frame_counter, frame_rate);
		print_pointcloud_frame(width, height, out_depth_mem, frame.p_metadata->frame_counter, frame_rate);

        printf("Processed frame n. %lu @ %u fps  (missed frames = %d) \n", frame.p_metadata->frame_counter,
               (unsigned int)frame_rate, (int)(frame.p_metadata->frame_counter - 1 - previous_frame_counter));

        previous_frame_counter = frame.p_metadata->frame_counter;

        // skip frame to old
        platform_acknowledge_event(PLATFORM_CAM_PIPE_FRAME_EVT);
    }

    // free memory and pipeline resources
    free(in_raw_mem.data);
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
    while (1)
        ;
}
