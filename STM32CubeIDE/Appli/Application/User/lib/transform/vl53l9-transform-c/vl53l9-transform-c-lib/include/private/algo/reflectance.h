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

#include <stdbool.h>

/**
 * @brief refletance module constants and constants computed and/or extracted from OTP
 *
 * @param float max_spads maximum number of SPADs per zone
 * @param float min_refl_thr lowest allowed reflectance value in percent
 * @param float max_refl_thr highest allowed reflectance value in percent
 * @param float correction_factor can be applied to fine tune threshold by scaling relative amplitude
 * @param float sq_law_exponent can be applied to modify square law exponent
 * @param float six_step_scaler coef applied to estimated reflectance for 6 steps capture
 * @param float cutoff_distance minimum distance to clip
 * @param bool cover_glass cover glass presence
 */
typedef struct reflectance_params_t {
    float max_spads;
    float min_refl_thr;
    float max_refl_thr;
    float correction_factor;
    float sq_law_exponent;
    float six_step_scaler;
    float cutoff_distance;
    bool cover_glass;
} reflectance_params_t;

/**
 * @brief compute reflectance and validation map for low and high reflectance pixels
 *
 * Based on Python algo R_1.4.14
 *
 * @details at least one of the output buffers needs to be not NULL : reflectance, low_refl_valid, high_refl_valid
 *
 * @param depth input depth buffer
 * @param amplitude input amplitude buffer
 * @param main_flag input main flag buffer
 * @param effective_spads input effective spads buffer
 * @param amp_ref input amplitude reference buffer
 *
 * @param reflectance output estimated reflectance
 * @param low_refl_valid output low reflectance validation buffer
 * @param high_refl_valid output high reflectance validation buffer
 *
 * @param params releftance constant parameters
 * @param size image size in pixels
 * @param expo_sf main exposure number
 * @param expo_sc close distance exposure number
 * @param step_number number of dToF capture steps
 */
int vl53l9_algo_reflectance(const float* depth, const float* amplitude, const bool* main_flag,
    const float* effective_spads, const float* amp_ref,

    float* reflectance, bool* low_refl_valid, bool* high_refl_valid,

    reflectance_params_t params, unsigned size, unsigned expo_sf, unsigned expo_sc, unsigned step_number);
