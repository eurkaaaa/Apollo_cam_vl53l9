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

#include "algo/distance_calibration.h"
#include <math.h>

/**
 * @note Implementation based on reference v2.1.6
 */

void vl53l9_algo_distance_calibration(const float *const distance_in, const float *const calibration_in,
                                      const float *const signal_rate_in, const float *const ambient_rate_in,
                                      const unsigned char *const dss_in,

                                      float *const distance_out,

                                      distance_calibration_params_t params, size_t size, unsigned int step_number) {

    if (!distance_in || !calibration_in || !distance_out) {
        return; // Missing mandatory input and output buffers
    }

    if ((params.nlc_mode) && (!dss_in)) {
        return; // Missing required inputs for NLC mode
    }

    if ((params.nlc_mode == 3) && (!signal_rate_in || !ambient_rate_in)) {
        return; // Missing required inputs for NLC mode 3
    }

    for (size_t i = 0; i < size; i++) {

        // TODO: check if this is needed when using NLC
        distance_out[i] = fmaxf(distance_in[i] + calibration_in[i], 0.0f);

        if (params.gain_correction) {
            distance_out[i] *= 1 + params.gain_correction;
        }

        if (params.nlc_mode == 0) {
            continue;
        } else if (params.nlc_mode == 1) {
            if (dss_in[i] > 2 && dss_in[i] < 6) {
                distance_out[i] += 10.0f;
            }
        } else {
            const int *lut_corr = (step_number == 7) ? params.lut_prec : params.lut_range;
            int constant_offset = (step_number == 7) ? params.constant_prec : params.constant_range;

            distance_out[i] += lut_corr[dss_in[i]] + constant_offset;

            if (params.nlc_mode == 3) {
                const float rate = signal_rate_in[i] + ambient_rate_in[i];
                if (!dss_in[i] && distance_out[i] < params.pileup_short_dist) {
                    if (distance_out[i] > 0) {
                        distance_out[i] += params.pileup_base_short * params.pileup_short_dist / distance_out[i];
                    }
                }
                if (rate >= params.pileup_rate_point) {
                    distance_out[i] += params.pileup_base;
                } else {
                    distance_out[i] += rate * params.pileup_base / params.pileup_rate_point;
                }
            }
        }

        distance_out[i] = fmaxf(0.0f, distance_out[i]);
    }
}
