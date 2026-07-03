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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief r2p module constants and constants computed and/or extracted from OTP
 *
 * @param float efl effective focal length in um
 * @param float residual_offset_x residual offset in x in SPAD
 * @param float residual_offset_y residual offset in y in SPAD
 * @param unsigned max_distance maximum clipping distance (ISP driven)
 * @param bool parallax_correction Enable parallax correction
 * @param unsigned parallax_limit minimum distance where parallax correction is computed
 * @param float alpha K1 distortion coeff
 * @param float beta K2 distortion coeff
 * @param float gamma K3 distortion coeff
 * @param float kappa K4 distortion coeff
 * @param unsigned max_spads_x full sensor spad width
 * @param unsigned max_spads_y full sensor spad height
 * @param float spad_size_um spad size in um
 */
typedef struct radial_to_perp_params_t {
    float efl;
    float residual_offset_x;
    float residual_offset_y;
    unsigned max_distance;
    bool parallax_correction;
    unsigned parallax_limit;
    float alpha;
    float beta;
    float gamma;
    float kappa;
    unsigned max_spads_x;
    unsigned max_spads_y;
    float spad_size_um;
} radial_to_perp_params_t;

/**
 * @brief compute r2p conversion
 *
 * Based on Python algo R_1.3.6
 *
 * @param depth input depth
 *
 * @param output_z output perpendicular distance
 * @param output_x output x coordinates of distance in cartesian coordinates
 * @param output_y output y coordinates of distance in cartesian coordinates
 * @param r2p_valid output validity map of r2p
 *
 * @param params r2p params
 * @param width image width
 * @param height image height
 * @param binning image binning
 *
 * @return int 0 if success, not 0 otherwise
 */
int vl53l9_algo_radial_to_perp(const float *depth,

                               float *output_z, float *center_x, float *distorsion, bool *r2p_valid,

                               radial_to_perp_params_t params, unsigned width, unsigned height, unsigned binning);

/**
 * TODO: add doc
 */
int vl53l9_algo_pointcloud(const float *depth, const unsigned char *android_conf, const float *center_x,
                           const float *distorsion,

                           float *pointcloud,

                           radial_to_perp_params_t params, unsigned width, unsigned height, unsigned binning);

#ifdef __cplusplus
}
#endif
