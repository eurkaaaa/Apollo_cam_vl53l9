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

#include "algo/distance_check.h"

/**
 * @note Based on version R_1.4.10
 */

void vl53l9_algo_distance_check(const float *const distance_in, const float *const dmax_in,
                                const bool *const r2p_valid_in, const bool *const confidence_valid_in,
                                const bool *const reflectance_valid_in, const bool *const sharpener_valid_in,
                                const bool *const fp_valid_in,

                                float *const distance_out, unsigned char *const status_out,

                                size_t size, float invalid_distance_default, bool r2p_filter, bool confidence_filter,
                                bool reflectance_filter, bool sharpener_filter, bool fp_filter, bool dmax_select) {

    for (size_t i = 0; i < size; i++) {

        float dmax = (dmax_select) ? dmax_in[i] : invalid_distance_default;

        bool is_r2p_valid = ((r2p_valid_in != NULL && r2p_filter) ? r2p_valid_in[i] : true);
        bool is_confidence_valid = ((confidence_valid_in != NULL && confidence_filter) ? confidence_valid_in[i] : true);
        bool is_reflectance_valid =
            ((reflectance_valid_in != NULL && reflectance_filter) ? reflectance_valid_in[i] : true);
        bool is_sharpener_valid = ((sharpener_valid_in != NULL && sharpener_filter) ? sharpener_valid_in[i] : true);
        bool is_fp_valid = ((fp_valid_in != NULL && fp_filter) ? fp_valid_in[i] : true);

        distance_out[i] =
            (is_confidence_valid && is_reflectance_valid && is_sharpener_valid && is_r2p_valid && is_fp_valid)
                ? distance_in[i]
                : dmax;

        // use inverse logic for constant valid status equal to zero
        status_out[i] = (!is_confidence_valid & 0x1) | ((!is_reflectance_valid & 0x1) << 1) |
                        ((!is_sharpener_valid & 0x1) << 2) | ((!is_r2p_valid & 0x1) << 3) | ((!is_fp_valid & 0x1) << 4);
    }
}
