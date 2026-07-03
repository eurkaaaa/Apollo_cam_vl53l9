/**
 *******************************************************************************
 * @file    extract.h
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

#ifndef EXTRACT_H
#define EXTRACT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Extract distance, amplitude and ambient from raw data buffer
 *
 * @param[in] input raw data buffer
 * @param[in] dss_coeffs_in dss coeffs LUT
 *
 * @param[out] distance_out raw distance output image
 * @param[out] amplitude_out raw amplitude output image
 * @param[out] ambient_out raw ambient output image
 * @param[out] msb_out main flag per pixel
 * @param[out] dss_lut dss LUT id per pixel
 * @param[out] dss_out aperture per pixel (effective spads)
 *
 * @param[in] frame_width raw image width
 * @param[in] frame_height raw image height
 * @param[in] crop
 * @param[in] crop_offset_x
 * @param[in] crop_offset_y
 * @param[in] crop_width
 * @param[in] crop_height
 * @param[in] binning
 */
 void vl53l9_algo_extract(const unsigned char *input, const float *dss_coeffs_in,

    float *distance_out, float *amplitude_out, float *ambient_out, bool *msb_out, unsigned char *dss_lut,float *dss_out,

    size_t frame_width, size_t frame_height, bool crop, size_t crop_offset_x, size_t crop_offset_y,
    size_t crop_width, size_t crop_height, unsigned char binning);

#endif // EXTRACT_H
