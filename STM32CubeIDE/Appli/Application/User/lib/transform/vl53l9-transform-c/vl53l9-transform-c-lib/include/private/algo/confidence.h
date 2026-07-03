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

#include <stdbool.h>
#include <stddef.h>

typedef struct confidence_params_t {
    bool cover_glass; // true if cover glass is present
    float signal_factor;
    float scaling_factor_short;
    float scaling_factor_main;
    float threshold_short;
    float threshold_main;
    float xtalk_coeff;
} confidence_params_t;

/**
 * @brief
 *
 * @note If noise_reduction_in is set to NULL it is not applied on confidence_out.
 * @note If android_out is set to NULL it is not calculated.
 */
void vl53l9_algo_confidence(const float *const ambient_in, const float *const amplitude_in, const bool *const msb_in,
                            const float *const effective_spads_in, const float *const noise_reduction_in,

                            float *const confidence_out, float *const threshold_out, bool *const validity_out,
                            float *const xtalk_est_out, unsigned char *const android_out,

                            confidence_params_t params, size_t size);
