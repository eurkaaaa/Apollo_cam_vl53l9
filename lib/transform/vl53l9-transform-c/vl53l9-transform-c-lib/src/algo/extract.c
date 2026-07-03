/**
 *******************************************************************************
 * @file    extract.c
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

#include "algo/extract.h"

/**
 * Extract data from the input buffer
 *
 *  - 1st block: distance (2 bytes per pixel)
 *   o distance: bits [0:14]
 *   o main_flag: bit [15]
 *
 *  - 2nd block: amplitude (2 bytes per pixel)
 *
 *  - 3rd block: ambient (2 bytes per pixel)
 *
 *  - 4th block: dss_id (4 bits per pixel)
 */

void vl53l9_algo_extract(const unsigned char *input, const float *dss_coeffs_in,

                         float *distance_out, float *amplitude_out, float *ambient_out, bool *msb_out,
                         unsigned char *dss_lut, float *dss_out,

                         size_t frame_width, size_t frame_height, bool crop, size_t crop_offset_x, size_t crop_offset_y,
                         size_t crop_width, size_t crop_height, unsigned char binning) {

    size_t frame_size = (crop) ? (crop_width * crop_height) : (frame_width * frame_height); // size in pixels
    size_t block_size = (frame_size * 2);                                                   // size in bytes
    const float binning_coeff = (binning * binning) / 4.0f;

    for (size_t i = 0; i < frame_size; i++) {
        unsigned int idx = (crop) ? ((frame_width * crop_offset_y + crop_offset_x + i) * 2) : (i * 2);
        unsigned int distance = (input[idx + 1] << 8) | input[idx];
        distance_out[i] = (float)(distance & 0x7FFF);
        msb_out[i] = (distance >> 15) & 0x1;
        amplitude_out[i] = (float)((input[idx + 1 + block_size] << 8) | (input[idx + block_size]));
        ambient_out[i] = (float)((input[idx + 1 + block_size * 2] << 8) | (input[idx + block_size * 2]));

        // dss_id are encoded on 4 bits, 2 dss_id per byte
        unsigned int dss_idx = (crop) ? ((frame_width * crop_offset_y + crop_offset_x + i) / 2) : (i / 2);
        unsigned int dss_id = (input[dss_idx + block_size * 3] >> ((i % 2) ? 4 : 0)) & 0b111;
        dss_lut[i] = (unsigned char)dss_id;
        dss_out[i] = dss_coeffs_in[dss_id] * binning_coeff;
    }
}
