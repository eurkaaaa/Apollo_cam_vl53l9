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

#include "algo/reflectance.h"

#include <math.h>
#include <stdlib.h>

int vl53l9_algo_reflectance(const float *depth, const float *amplitude, const bool *main_flag,
                            const float *effective_spads, const float *amp_ref,

                            float *reflectance, bool *low_refl_valid, bool *high_refl_valid,

                            reflectance_params_t params, unsigned size, unsigned expo_sf, unsigned expo_sc,
                            unsigned step_number) {
    // if one of the input is missing, return
    // if all outputs are missing, return
    if (!depth || !amplitude || !main_flag || !effective_spads || !amp_ref ||
        !(reflectance || low_refl_valid || high_refl_valid)) {
        return EXIT_FAILURE;
    }

    float xt_correction;
    if (params.cover_glass) {
        if (step_number == 7) {
            if (params.min_refl_thr > 1.0f) {
                xt_correction = 1.1f - params.min_refl_thr / 80.0f;
            } else {
                xt_correction = 2.25f - params.min_refl_thr;
            }
        } else {
            xt_correction = 0.9f;
        }
    } else {
        xt_correction = 1.0f;
    }

    for (unsigned i = 0; i < size; ++i) {
        const float depth_val = fmaxf(params.cutoff_distance, depth[i]);
        const float coef =
            (main_flag[i] ? 1.0f : (float)expo_sc / expo_sf) * (step_number == 6 ? params.six_step_scaler : 1.0f);
        float amp_1pc = coef * xt_correction * amp_ref[i] * 1e6f * effective_spads[i] * params.correction_factor /
                        (powf(depth_val, params.sq_law_exponent) * params.max_spads);
        amp_1pc = fmaxf(0.0f, fminf(amp_1pc, 65535.0f));

        if (reflectance) {
            reflectance[i] = fmax(0.0f, amplitude[i] / amp_1pc);
        }
        if (low_refl_valid) {
            low_refl_valid[i] = reflectance[i] >= params.min_refl_thr;
        }
        if (high_refl_valid) {
            high_refl_valid[i] = reflectance[i] < params.max_refl_thr;
        }
    }

    return EXIT_SUCCESS;
}
