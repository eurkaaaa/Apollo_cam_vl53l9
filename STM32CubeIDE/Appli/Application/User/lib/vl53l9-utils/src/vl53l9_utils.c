/**
 *******************************************************************************
 * @file    vl53l9_utils.c
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

#include "vl53l9_utils.h"

#include <string.h> // memcpy

#define DEBUG_SETTINGS_ADDR (0x5CCU)
#define OTP_SECTION_1_ADDR (0x6E8U)
#define OTP_SECTION_2_ADDR (0xD18U)

#define OTP_SECTION_1_OFFSET  (OTP_SECTION_1_ADDR - DEBUG_SETTINGS_ADDR)
#define OTP_SECTION_2_OFFSET  (OTP_SECTION_2_ADDR - DEBUG_SETTINGS_ADDR)

#define GLOBAL_DIST_OFFSET  (OTP_SECTION_1_OFFSET + 0x0004U)
#define OPTICAL_OFFSET_X    (OTP_SECTION_1_OFFSET + 0x0420U)
#define OPTICAL_OFFSET_Y    (OTP_SECTION_1_OFFSET + 0x0421U)
#define RESIDUAL_OFFSET_X   (OTP_SECTION_1_OFFSET + 0x0422U)
#define RESIDUAL_OFFSET_Y   (OTP_SECTION_1_OFFSET + 0x0423U)
#define RAD2PERP_FOV_GAIN   (OTP_SECTION_1_OFFSET + 0x043cU)
#define CAL_AMP_COEFF       (OTP_SECTION_1_OFFSET + 0x0598U)
#define CAL_AMP_SCALER      (OTP_SECTION_1_OFFSET + 0x05bbU)
#define CAL_AMP_DISTANCE    (OTP_SECTION_1_OFFSET + 0x05bcU)
#define CAL_AMP_EXPO        (OTP_SECTION_1_OFFSET + 0x05beU)
#define CAL_AMP_REFLECTANCE (OTP_SECTION_1_OFFSET + 0x05c0U)

#define DIST_OFFSET_0_945 (OTP_SECTION_1_OFFSET + 0x0008U)
#define DIST_OFFSET_1_315 (OTP_SECTION_1_OFFSET + 0x0440U)
#define DIST_OFFSET_2_441 (OTP_SECTION_2_OFFSET + 0x0000U)

typedef struct {
    uint8_t _binning;
    uint8_t _width;
    uint8_t _height;
} _resolution_lut_t;

static int8_t _compute_distance_offset(uint8_t *p_buffer, size_t index, uint8_t pos);
static size_t _get_raw_resolution(uint8_t binning);

// TODO: this implementation handles only little-endian platforms (in case of big-endian data must be swapped)
void vl53l9_utils_parse_calib_data(uint8_t *buffer, vl53l9_calib_data_t *p_data) {

    uint8_t data8;
    uint16_t data16;

    if (!buffer || !p_data) {
        return; // TODO: return internal error
    }

    // TODO: fill p_data->dss_short_effective_spad when new fw patch is available
    // TODO: fill p_data->dss_long_effective_spad when new fw patch is available

    p_data->global_offset = *((int16_t *)(buffer + GLOBAL_DIST_OFFSET));

    // S6.0
    // read distance offsets over 3 blocks and realign data taking into account the 6-bit signed format
    // i = buffer - j = p_data - k = dist_offsets_pos/size
    const size_t dist_offsets_pos[3] = { DIST_OFFSET_0_945, DIST_OFFSET_1_315, DIST_OFFSET_2_441 };
    const size_t dist_offsets_size[3] = { 945, 315, 441 };
    size_t j = 0;
    for (size_t k = 0; k < 3; k++) {
        // NOTE: we process 3 bytes at a time (24 bits) that store 4 distance offsets (4 x 6 bits = 24 bits)
        size_t start = dist_offsets_pos[k];
        size_t end = dist_offsets_pos[k] + dist_offsets_size[k];
        for (size_t i = start; i < end; i += 3) {
            p_data->distance_offset[j] = _compute_distance_offset(buffer, i, 0);
            p_data->distance_offset[j + 1] = _compute_distance_offset(buffer, i, 1);
            p_data->distance_offset[j + 2] = _compute_distance_offset(buffer, i, 2);
            p_data->distance_offset[j + 3] = _compute_distance_offset(buffer, i, 3);
            j += 4;
        }
    }

    // S2.0
    data8 = *((uint8_t *)(buffer + OPTICAL_OFFSET_X));
    if (data8 == 3) {
        p_data->optical_offset_x = -1;
    } else {
        p_data->optical_offset_x = (int8_t)data8;
    }

    // S2.0
    data8 = *((uint8_t *)(buffer + OPTICAL_OFFSET_Y));
    if (data8 == 3) {
        p_data->optical_offset_y = -1;
    } else {
        p_data->optical_offset_y = (int8_t)data8;
    }

    // S2.1
    data8 = *((uint8_t *)(buffer + RESIDUAL_OFFSET_X));
    p_data->residual_offset_x = (data8 & 0x3) / 2.0f;
    if ((data8 & 0x4) == 4) {
        p_data->residual_offset_x -= 2;
    }

    // S2.1
    data8 = *((uint8_t *)(buffer + RESIDUAL_OFFSET_Y));
    p_data->residual_offset_y = (data8 & 0x3) / 2.0f;
    if ((data8 & 0x4) == 4) {
        p_data->residual_offset_y -= 2;
    }

    // U1.7
    data8 = *((uint8_t *)(buffer + RAD2PERP_FOV_GAIN));
    if (data8 & (1 << 7)) {
        p_data->rad2perp_fov_gain = 1.0f + (data8 & 0x7f) / 128.0f;
    } else {
        p_data->rad2perp_fov_gain = (data8 & 0x7f) / 128.0f;
    }

    for (size_t i = 0; i < 35; i++) {
        p_data->amplitude_coeffs[i] = *((uint8_t *)(buffer + CAL_AMP_COEFF + i));
    }

    data8 = *((uint8_t *)(buffer + CAL_AMP_SCALER));
    p_data->amplitude_scaler = data8;

    data16 = *((uint16_t *)(buffer + CAL_AMP_DISTANCE));
    p_data->amplitude_distance = data16;

    data16 = *((uint16_t *)(buffer + CAL_AMP_EXPO));
    p_data->amplitude_exposure = data16;

    data8 = *((uint8_t *)(buffer + CAL_AMP_REFLECTANCE));
    p_data->amplitude_reflectance = data8;
}

int vl53l9_utils_parse_frame(uint8_t *buffer, size_t buffer_size, vl53l9_frame_t *p_frame) {

    p_frame->p_metadata = (vl53l9_metadata_t *)&buffer[buffer_size - sizeof(vl53l9_metadata_t)];

    size_t resolution = _get_raw_resolution(p_frame->p_metadata->binning);

    size_t dss_size = resolution / 2u;
    if (buffer_size < ((resolution * 3u * 2u) + dss_size)) {
        return 1; // invalid param
    }

    // TODO: copy data or just set pointer?

    size_t offset = 0;
    p_frame->p_distance = (vl53l9_distance_t *)&buffer[offset];
    offset += (uint16_t)(resolution * 2);

    p_frame->p_amplitude = (uint16_t *)&buffer[offset];
    offset += (uint16_t)(resolution * 2);

    p_frame->p_ambient = (uint16_t *)&buffer[offset];
    offset += (uint16_t)(resolution * 2);

    p_frame->p_dss_lut = &buffer[offset]; // NOTE: no check since dss is always enabled

    return 0;
}

int vl53l9_utils_get_resolution(uint8_t binning, uint8_t *p_width, uint8_t *p_height) {

    static const _resolution_lut_t lut[] = { { 2, 54, 42 }, { 4, 24, 20 }, { 6, 18, 14 },
                                             { 8, 12, 10 }, { 12, 8, 6 },  { 24, 4, 4 } };

    for (uint8_t i = 0; i < sizeof(lut) / sizeof(lut[0]); i++) {
        if (lut[i]._binning == binning) {
            *p_width = lut[i]._width;
            *p_height = lut[i]._height;
            return 0;
        }
    }
    return -1;
}

/* private functions */

/**
 * @brief Compute distance offset from buffer
 * @param p_buffer OTP buffer to extract data from
 * @param index Index of the OTP buffer to start from
 * @param pos Position of the requested offset starting from index (0 to 3 since handled 4 by 4)
 *
 * @return The computed distance offset
 */
static int8_t _compute_distance_offset(uint8_t *p_buffer, size_t index, uint8_t pos) {

    uint8_t value;

    switch (pos) {
    case 0:
        value = p_buffer[index] & 0x3F;
        break;
    case 1:
        value = ((p_buffer[index] & 0xC0) >> 6) | ((p_buffer[index + 1] & 0x0F) << 2);
        break;
    case 2:
        value = ((p_buffer[index + 1] & 0xF0) >> 4) | ((p_buffer[index + 2] & 0x03) << 4);
        break;
    case 3:
        value = (p_buffer[index + 2] & 0xFC) >> 2;
        break;

    default:
        return -1;
    }

    // convert to signed value if sign bit is set
    if (value & 0x20) {
        return (int8_t)((value & 0x1F) - 32);
    } else {
        return (int8_t)(value);
    }
}

/**
 * @brief Get the raw resolution of a frame given the binning factor
 * @param[in] binning Binning factor
 * @note The resolution is expressed in pixels and doesn't take into account the cropping
 * @return The raw resolution or 0 if the binning factor is not supported
 */
static size_t _get_raw_resolution(uint8_t binning) {

    size_t resolution = 0;

    static const _resolution_lut_t lut[] = { { 2, 54, 42 }, { 4, 24, 24 }, { 6, 18, 14 },
                                             { 8, 12, 10 }, { 12, 8, 8 },  { 24, 4, 4 } };

    for (uint8_t i = 0; i < sizeof(lut) / sizeof(lut[0]); i++) {
        if (lut[i]._binning == binning) {
            resolution = lut[i]._width * lut[i]._height;
            break;
        }
    }
    return resolution;
}
