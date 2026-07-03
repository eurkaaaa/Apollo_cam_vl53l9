/**
 *******************************************************************************
 * @file   simple_ranging_csi.c
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

#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "vl53l9.h"
#include "vl53l9_device.h"
#include "vl53l9_utils.h"

#include "main.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static void handle_error(void);

__attribute__((aligned(32))) volatile uint8_t g_csi_output_buffer[14900]; // 100 * 149 (csi_width * csi_height)

const char ASCII_CHARS[] = "@%#*+=-:. ";

void print_frame(const vl53l9_frame_t frame, float frame_rate) {
    // set to the top of the screen
    printf("\033[%d;%dH", 0, 0);
    printf("Frame n. %lu @ %u fps\n", frame.p_metadata->frame_counter, (unsigned int)frame_rate);
    int pixel_step = 1;
    uint16_t min = UINT16_MAX;
    uint16_t max = 0;

    for (int i = 0; i < (frame.p_metadata->frame_height * frame.p_metadata->frame_width); i++) {
        uint16_t value = frame.p_distance[i].value;
        min = MIN(value, min);
        max = MAX(value, max);
    }

    uint16_t average = (max - min) * 0.05;
    min = MAX(min - average, 0);
    max = MIN(max + average, UINT16_MAX);

    for (int y = 0; y < frame.p_metadata->frame_height; y += pixel_step) {
        for (int x = 0; x < frame.p_metadata->frame_width; x += pixel_step) {
            int pixel_index = (y * frame.p_metadata->frame_width + x);
            uint16_t value = frame.p_distance[pixel_index].value;

            int ascii_index = (value - min) * (sizeof(ASCII_CHARS) - 1) / (max - min);
            ascii_index = MAX(0, MIN(ascii_index, sizeof(ASCII_CHARS) - 1));

            printf("%c", ASCII_CHARS[ascii_index]);
        }
        printf("\n");
    }
}

int vl53l9_app() {

    int ret;
    int ret_irq;
    vl53l9_device_t *p_dev = &device[0];

    vl53l9_hw_config_t hw_config;

    vl53l9_frame_t frame;

    // select the profile configuration to be applied
    vl53l9_profile_t *p_profile = &g_ranging_profiles[VL53L9_USECASE_AR_RANGE];

    ret = platform_power_reset(0);
    if (ret) {
        handle_error();
    }

    if (p_dev->bus_type & PLATFORM_BUS_I3C) {
        if (platform_assign_dynamic_address() != 0) {
            handle_error();
        }
    }

    ret = vl53l9_init(p_dev);
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

    ret = platform_start_csi_pipe((uint8_t *)g_csi_output_buffer);
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

    platform_enable_event(PLATFORM_CAM_PIPE_FRAME_EVT);

    printf("\033[2J");

    while (1) {
        ret_irq = platform_wait_for_event(PLATFORM_CAM_PIPE_FRAME_EVT, 1000);
        if (ret_irq == 0) {
            platform_acknowledge_event(PLATFORM_CAM_PIPE_FRAME_EVT);

            // invalidate cache to ensure data coherency (TODO: abstract this call)
            SCB_InvalidateDCache_by_Addr((uint32_t *)g_csi_output_buffer, sizeof(g_csi_output_buffer));

            ret = vl53l9_utils_parse_frame((uint8_t *)g_csi_output_buffer, sizeof(g_csi_output_buffer), &frame);

            if (ret) {
                handle_error();
            }
            stop_time = platform_profiler_get_timestamp();
            frame_rate = (1.0f / (float)(platform_profiler_convert_to_us(stop_time - start_time))) * 1000000;
            start_time = stop_time;
            print_frame(frame, frame_rate);
        }
    }

    return 0;
}

static void handle_error(void) {
    while (1)
        ;
}
