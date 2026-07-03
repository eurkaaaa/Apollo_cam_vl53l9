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
 * @brief ratenorm module constants and constants computed and/or extracted from OTP
 *
 * @param bool fast_mode trigger ratenorm fast mode (no bicubic interpolation)
 * @param unsigned ref_mode Reference computation modes
 * @param unsigned ref_scaler reference scaler
 * @param unsigned ref_distance reference distance
 * @param unsigned ref_reflectance reference reflectance
 * @param unsigned ref_expo reference exposition
 * @param unsigned ref_amp_const Constant Reference Amplitude
 * @param unsigned nominal_width nominal reference image width
 * @param unsigned nominal_height nominal reference image height
 * @param unsigned nominal_binningnominal reference image binning
 * @param float ambient_window window size of ambient step in ns
 * @param float ambient_blanking Blank timing for windows in ns
 * @param float signal_factor conversion from amplitude to photon count
 * @param float main_scaler Scaler to refine all reference values
 * @param float side_scaler Scaler to refine direct neighbours
 * @param float corner_scaler Scaler to diagonal corner neighbours
 * @param float left_side_l_scaler Correcting left side column of coefficients
 * @param float right_side_l_scaler Correcting right side column of coefficients
 * @param float middle_section_l_scaler Correcting middle region coefficients
 * @param float center_region_l_scaler Correcting central region coefficients
 * @param float peak_l_scaler Correcting centre peak pixel
 * @param float top_row_l_scaler Correcting top row (excluding corners))
 * @param float bottom_row_l_scaler Correcting bottom row (excluding corners))
 * @param float corner_post_l_scaler Relative corner correction factor
 * @param float exposure_width_0 exposure width for 7 step captures
 * @param float exposure_width_1 exposure width for 6 step captures
 * @param float max_spads Max. SPADs of current device config
 * @param float max_spads_ref Max. SPAD state at reference measurement
 * @param float bicubic_coef bicubic interpolation coefficient, -0.75 is standard
 */
typedef struct ratenorm_params_t {
    bool fast_mode;
    unsigned ref_mode;
    unsigned ref_scaler;
    unsigned ref_distance;
    unsigned ref_reflectance;
    unsigned ref_expo;
    unsigned ref_amp_const;
    unsigned nominal_width;
    unsigned nominal_height;
    unsigned nominal_binning;
    float ambient_window;
    float ambient_blanking;
    float signal_factor;
    float main_scaler;
    float side_scaler;
    float corner_scaler;
    float left_side_l_scaler;
    float right_side_l_scaler;
    float middle_section_l_scaler;
    float center_region_l_scaler;
    float peak_l_scaler;
    float top_row_l_scaler;
    float bottom_row_l_scaler;
    float corner_post_l_scaler;
    float exposure_width_0;
    float exposure_width_1;
    float max_spads;
    float max_spads_ref;
    float bicubic_coef;
} ratenorm_params_t;

/**
 * @brief bicubic interpolation from input to output
 *
 * Based on Python algo R_1.8.4
 *
 * @param input input array
 *
 * @param output output array
 *
 * @param width_in input width
 * @param height_in input height
 * @param width_out output width
 * @param height_out output height
 * @param a bicubic interpolation coefficient, -0.75 is standard
 */
void vl53l9_algo_ratenorm_bicubic_resize(const float *input,

    float *output,

    unsigned width_in, unsigned height_in, unsigned width_out, unsigned height_out, float a);

/**
 * @brief compute nomilized maps at calibration time, once per stream
 *
 * Based on Python algo R_1.8.4
 *
 * @param reference_coefs OTP stored amplitude coefficients
 * @param r2p_coefs Radial to Perpendicular coeeficients
 *
 * @param ref_amp_no_expo amplitude reference without expoSF
 * @param ref_amp_rad_no_expo spherical amplitude reference without expoSF
 * @param coeff_norm_no_expo spatial rate correction without expoSF
 *
 * @param params ratenorm params
 * @param width output width
 * @param height output height
 * @param binning output binning
 *
 * @return int error code, 0 if success, 1 if error
 */
int vl53l9_algo_ratenorm_compute_norm_maps(const float* reference_coefs, const float* r2p_coefs,

    float* ref_amp_no_expo, float* ref_amp_rad_no_expo, float* coeff_norm_no_expo,

    ratenorm_params_t params, unsigned width, unsigned height, unsigned binning);

/**
 * @brief compute rates
 *
 * Based on Python algo R_1.8.4
 *
 * @param amplitude input amplitude
 * @param ambient input ambient
 * @param effective_spads input effective spads
 * @param main_flag input main flag
 * @param ref_amp_no_expo input amplitude reference without expoSF
 * @param ref_amp_rad_no_expo input spherical amplitude reference without expoSF
 * @param coeff_norm_no_expo input spatial rate correction without expoSF
 *
 * @param ref_amp output amplitude reference
 * @param ref_amp_rad output spherical amplitude reference
 * @param signal_photon_rate output signal photon_rate
 * @param ambient_photon_rate output ambient photon rate
 * @param rescaled_ambient output rescaled ambient
 *
 * @param params ratenorm params
 * @param size image size
 * @param step_number step number
 * @param expo_sf main exposure number
 * @param expo_sc close distance exposure number
 * @param expo_sa ambient exposure number
 * @param ambient_attenuation Ambient attenuation shift
 *
 * @return int error code, 0 if success, 1 if error
 */
int vl53l9_algo_ratenorm_compute_rates(const float* amplitude, const float* ambient, const float* effective_spads,
    const bool* main_flag, const float* ref_amp_no_expo, const float* ref_amp_rad_no_expo,
    const float* coeff_norm_no_expo,

    float* ref_amp, float* ref_amp_rad, float* signal_photon_rate, float* ambient_photon_rate, float* rescaled_ambient,

    ratenorm_params_t params, unsigned size, unsigned step_number, unsigned expo_sf, unsigned expo_sc,
    unsigned expo_sa, unsigned ambient_attenuation);

#ifdef __cplusplus
}
#endif
