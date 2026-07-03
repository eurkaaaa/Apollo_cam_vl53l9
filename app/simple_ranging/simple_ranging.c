/**
 *******************************************************************************
 * @file   simple_ranging.c
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

static void handle_error(void);

#define DEVICE_ID (0)

int vl53l9_app() {

    int ret;
    vl53l9_device_t *p_dev = &device[DEVICE_ID];

    // select the profile configuration to be applied
    vl53l9_profile_t *p_profile = &g_ranging_profiles[VL53L9_USECASE_AR_RANGE];
    uint16_t buffer_size;

    vl53l9_get_raw_buffer_size(p_profile->binning, &buffer_size);
    uint8_t *p_buffer = malloc(buffer_size);

    platform_power_reset(DEVICE_ID);
    if (p_dev->bus_type & PLATFORM_BUS_I3C) {
        platform_assign_dynamic_address();
    }

    ret = vl53l9_init(p_dev);
    if (ret) {
        handle_error();
    }

    platform_set_profile(p_dev, p_profile);

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

    while (1) {

        ret = vl53l9_trigger_frame(p_dev);
        if (ret) {
            handle_error();
        }

        ret = platform_wait_for_event(PLATFORM_GPIO_IT_EVT, 1000);
        if (ret) {
            handle_error();
        }

        platform_acknowledge_event(PLATFORM_GPIO_IT_EVT);

        ret = vl53l9_get_frame(p_dev, p_buffer, buffer_size);
        if (ret) {
            handle_error();
        }

        vl53l9_frame_t frame = { 0 };
        ret = vl53l9_utils_parse_frame(p_buffer, buffer_size, &frame);
        if (ret) {
            handle_error();
        }

        // measure frame rate
        stop_time = platform_profiler_get_timestamp();
        frame_rate = (1.0f / (float)(platform_profiler_convert_to_us(stop_time - start_time))) * 1000000;
        start_time = stop_time;
        printf("Frame n. %lu @ %u fps\n", frame.p_metadata->frame_counter, (unsigned int)frame_rate);
    }

    return 0;
}

static void handle_error(void) {
    while (1)
        ;
}
