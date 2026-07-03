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

#include "algo/radial_to_perp.h"

#include <math.h>
#include <stdlib.h>

static inline float maxf(float a, float b) {
    return a > b ? a : b;
}

static inline float rsqrt(float number) {
    union {
        float f;
        unsigned i;
    } conv = { .f = number };
    conv.i = 0x5F375A86 - (conv.i >> 1);
    conv.f *= 1.5f - (number * 0.5f * conv.f * conv.f);
    conv.f *= 1.5f - (number * 0.5f * conv.f * conv.f);
    return conv.f;
}

static inline float compute_distortion(float rsq, float alpha, float beta, float gamma, float kappa, float binning) {
    const float distorsion = alpha * rsq + beta * rsq * rsq + gamma * rsq * rsq * rsq + kappa;

    const float binning_distortion =
        binning == 2 ? 1.0f : fabsf(sqrtf(2.0f) * ((binning * binning / 4.0f) - binning / 2));

    return 1 + binning_distortion * distorsion;
}

int vl53l9_algo_radial_to_perp(const float *depth,

                               float *output_z, float *center_x, float *distorsion, bool *r2p_valid,

                               radial_to_perp_params_t params, unsigned width, unsigned height, unsigned binning) {
    // if one of the input is missing, return
    // if all outputs are missing, return
    if (!depth || !(output_z || center_x || distorsion || r2p_valid)) {
        return EXIT_FAILURE;
    }

    const float mspads_x = params.max_spads_x / 2.0f;
    const float mspads_y = params.max_spads_y / 2.0f;
    const float mpix = binning * 2.0f;
    const float focal = params.efl / (params.spad_size_um * mpix);

    const float x_center = (mspads_x + params.residual_offset_x - (mspads_x - binning * width)) / mpix - 0.5f;
    const float y_center = (mspads_y + params.residual_offset_y - (mspads_y - binning * height)) / mpix - 0.5f;

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned linear_id = y * width + x;
            float dist_from_center_x = x - x_center;
            const float dist_from_center_y = y - y_center;
            float dist_from_center_sq =
                dist_from_center_x * dist_from_center_x + dist_from_center_y * dist_from_center_y;
            float dist_from_center_ud = compute_distortion(dist_from_center_sq, params.alpha, params.beta, params.gamma,
                                                           params.kappa, (float)binning);

            float depth_perp =
                depth[linear_id] *
                rsqrt(1.0f + dist_from_center_sq / (dist_from_center_ud * dist_from_center_ud * focal * focal));

            float new_x_center = x_center;
            if (params.parallax_correction) {
                new_x_center =
                    x_center - (params.efl * 7.166f /
                                (maxf((float)params.parallax_limit, depth_perp) * params.spad_size_um * mpix));
                if (center_x) {
                    center_x[linear_id] = new_x_center;
                }
                dist_from_center_x = x - new_x_center;
                dist_from_center_sq = dist_from_center_x * dist_from_center_x + dist_from_center_y * dist_from_center_y;
                dist_from_center_ud = compute_distortion(dist_from_center_sq, params.alpha, params.beta, params.gamma,
                                                         params.kappa, (float)binning);
                if (distorsion) {
                    distorsion[linear_id] = dist_from_center_ud;
                }

                depth_perp =
                    depth[linear_id] *
                    rsqrt(1.0f + dist_from_center_sq / (dist_from_center_ud * dist_from_center_ud * focal * focal));
            }

            if (output_z) {
                output_z[linear_id] = depth_perp;
            }
            if (r2p_valid) {
                r2p_valid[linear_id] = depth_perp <= params.max_distance;
            }
        }
    }

    return EXIT_SUCCESS;
}

int vl53l9_algo_pointcloud(const float *depth, const unsigned char *android_conf, const float *center_x,
                           const float *distorsion,

                           float *pointcloud,

                           radial_to_perp_params_t params, unsigned width, unsigned height, unsigned binning) {

    // if parralax is enabled and center_x is missing, return
    if (params.parallax_correction && !center_x) {
        return EXIT_FAILURE;
    }
    // if one of the other input is missing, return
    // if all outputs are missing, return
    if (!depth || !android_conf || !distorsion || !(pointcloud)) {
        return EXIT_FAILURE;
    }

    const float mspads_x = params.max_spads_x / 2.0f;
    const float mspads_y = params.max_spads_y / 2.0f;
    const float mpix = binning * 2.0f;
    const float focal = params.efl / (params.spad_size_um * mpix);

    const float x_center = (mspads_x + params.residual_offset_x - (mspads_x - binning * width)) / mpix - 0.5f;
    const float y_center = (mspads_y + params.residual_offset_y - (mspads_y - binning * height)) / mpix - 0.5f;

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned linear_id = y * width + x;
            const float dist_from_center_x = x - (params.parallax_correction ? center_x[linear_id] : x_center);
            const float dist_from_center_y = y - y_center;
            const float distorted_z = depth[linear_id] / (distorsion[linear_id] * focal);

            const float pcx = dist_from_center_x * distorted_z;
            const float pcy = dist_from_center_y * distorted_z;

            pointcloud[linear_id * 4] = pcx;
            pointcloud[linear_id * 4 + 1] = pcy;
            pointcloud[linear_id * 4 + 2] = depth[linear_id];
            pointcloud[linear_id * 4 + 3] = android_conf[linear_id] ? (android_conf[linear_id] - 1) / 7.0f : 1.0f;
        }
    }

    return EXIT_SUCCESS;
}
