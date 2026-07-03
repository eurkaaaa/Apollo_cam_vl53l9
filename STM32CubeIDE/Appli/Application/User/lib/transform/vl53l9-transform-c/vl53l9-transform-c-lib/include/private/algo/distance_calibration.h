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

#include <stddef.h>

typedef struct distance_calibration_params_t {
    float gain_correction;   // Gain correction factor (applies 1 + gain_correction, i.e. 0.1 means 10% gain)
    int pileup_base;         // Base value for pile-up correction
    float pileup_rate_point; // Pile-up rate point
    int pileup_base_short;   // Base value for short distance pile-up correction
    float pileup_short_dist; // Short distance pile-up correction factor
    unsigned int nlc_mode;   // Non linear correction mode (0: off, 1, 2, 3)
    int constant_prec;       // Constant offset value (for 7 steps)
    int constant_range;      // Constant offset value (for 6 steps)
    int lut_prec[8];         // LUT X precision offsets (for 7 steps)
    int lut_range[8];        // LUT X range offsets (for 6 steps)
} distance_calibration_params_t;

/**
 * @brief Apply calibration map to depth image
 *
 * @param[in] distance_in
 * @param[in] calibration_in
 * @param[in] signal_rate_in
 * @param[in] ambient_rate_in
 * @param[in] dss_in
 *
 * @param[out] distance_out
 *
 * @param[in] size
 * @param[in] step_number
 * @param[in] params
 *
 * @note input and output buffers are assumed to be row-major
 * @note input and output buffers are assumed to have the same size
 */
void vl53l9_algo_distance_calibration(const float *const distance_in, const float *const calibration_in,
                                      const float *const signal_rate_in, const float *const ambient_rate_in,
                                      const unsigned char *const dss_in,

                                      float *const distance_out,

                                      distance_calibration_params_t params, size_t size, unsigned int step_number);
