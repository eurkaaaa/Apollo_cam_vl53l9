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

#include "algo/confidence.h"
#include <math.h>

static float _compute_confidence_with_cover_glass(float amplitude, float ambient, float xtalk_est);
static float _compute_confidence_without_cover_glass(float amplitude, float ambient);
static unsigned char _compute_android_format(float confidence, float threshold);

/**
 * @note Implementation based on reference v1.10.2
 */

void vl53l9_algo_confidence(const float *const ambient_in, const float *const amplitude_in, const bool *const msb_in,
                            const float *const effective_spads_in, const float *const noise_reduction_in,

                            float *const confidence_out, float *const threshold_out, bool *const validity_out,
                            float *const xtalk_est_out, unsigned char *const android_out,

                            confidence_params_t params, size_t size) {

    // calculate confidence
    if (params.cover_glass) {
        for (size_t i = 0; i < size; i++) {
            float scaling_factor = (msb_in[i] == 0) ? params.scaling_factor_short : params.scaling_factor_main;
            xtalk_est_out[i] = params.xtalk_coeff * effective_spads_in[i];
            confidence_out[i] = _compute_confidence_with_cover_glass(amplitude_in[i] * params.signal_factor,
                                                                     ambient_in[i] * scaling_factor, xtalk_est_out[i]);
        }
    } else {
        for (size_t i = 0; i < size; i++) {
            float scaling_factor = (msb_in[i] == 0) ? params.scaling_factor_short : params.scaling_factor_main;
            xtalk_est_out[i] = 0.0f;
            confidence_out[i] = _compute_confidence_without_cover_glass(amplitude_in[i] * params.signal_factor,
                                                                        ambient_in[i] * scaling_factor);
        }
    }

    if (noise_reduction_in != NULL) {
        for (size_t i = 0; i < size; i++) {
            confidence_out[i] /= noise_reduction_in[i];
        }
    }

    // calculate threshold
    for (size_t i = 0; i < size; i++) {
        threshold_out[i] = (msb_in[i] == (unsigned short)0) ? params.threshold_short : params.threshold_main;
    }

    // calculate validity
    for (size_t i = 0; i < size; i++) {
        validity_out[i] = (msb_in[i] == (unsigned short)0) ? (confidence_out[i] > params.threshold_short)
                                                           : (confidence_out[i] > params.threshold_main);
    }

    // calculate android output
    if (android_out != NULL) {
        for (size_t i = 0; i < size; i++) {
            android_out[i] = _compute_android_format(confidence_out[i], threshold_out[i]);
        }
    }
}

static float _compute_confidence_with_cover_glass(float amplitude, float ambient, float xtalk_est) {
    const float num = (amplitude * amplitude - xtalk_est * xtalk_est);
    const float den = (amplitude + ambient + xtalk_est);

    if ((num > 0.0f) && (den > 0.0f)) {
        return sqrtf(num / den);
    } else {
        return 0.0f;
    }
}

static float _compute_confidence_without_cover_glass(float amplitude, float ambient) {
    if (amplitude + ambient != (unsigned short)0) {
        return amplitude / sqrtf(amplitude + ambient);
    } else {
        return 0.0f;
    }
}

static unsigned char _compute_android_format(float confidence, float threshold) {

    if (confidence <= threshold) {
        return 1;
    } else if (confidence <= 3 * threshold) {
        return 3;
    } else if (confidence <= 6 * threshold) {
        return 4;
    } else if (confidence <= 8 * threshold) {
        return 5;
    } else if (confidence <= 10 * threshold) {
        return 6;
    } else if (confidence <= 12 * threshold) {
        return 7;
    } else {
        return 0;
    }
}
