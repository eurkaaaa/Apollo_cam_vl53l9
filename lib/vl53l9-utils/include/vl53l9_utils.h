/**
 *******************************************************************************
 * @file    vl53l9_utils.h
 *******************************************************************************
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

#ifndef VL53L9_UTILS_H
#define VL53L9_UTILS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t frame_counter;
    uint16_t temperature;
    uint16_t reserved_ldd[15];
    uint16_t reserved_ref[8];
    uint16_t frame_width;
    uint16_t frame_height;

    // static settings
    uint8_t sync_mode : 2;
    uint8_t power_mode : 2;
    uint8_t format : 2;
    uint8_t acquisition_mode : 2;

    uint8_t ambient_attenuation;

    // dynamic settings
    uint16_t reserved_dyn : 4;
    uint16_t dss_mode : 2;
    uint16_t binning : 5;
    uint16_t context : 1;
    uint16_t nb_step : 4;

    uint16_t error_code;
    uint8_t error_status;

    uint8_t reserved_ldd_error[5];
    uint32_t frame_period;

    uint32_t crop_x_size : 6;
    uint32_t crop_y_size : 6;
    uint32_t crop_x_offset : 6;
    uint32_t crop_y_offset : 6;
    uint32_t crop_enable : 1;

    uint8_t nb_shot_step1_lsb;
    uint8_t nb_shot_step1_mid;
    uint8_t nb_shot_step1_msb;

    uint8_t nb_shot_step4_5_lsb;
    uint8_t nb_shot_step4_5_mid;
    uint8_t nb_shot_step4_5_msb;

    uint8_t nb_shot_step6_lsb;
    uint8_t nb_shot_step6_mid;
    uint8_t nb_shot_step6_msb;

    uint8_t nb_shot_step7_lsb;
    uint8_t nb_shot_step7_mid;
    uint8_t nb_shot_step7_msb;

    uint32_t sest_reserved[3];

} vl53l9_metadata_t;

typedef struct {
    uint16_t value : 15;
    uint16_t flag : 1;
} vl53l9_distance_t;

/**
 * @struct vl53l9_frame_t
 */
typedef struct {
    vl53l9_distance_t *p_distance;
    uint16_t *p_amplitude;
    uint16_t *p_ambient;
    uint8_t *p_dss_lut;
    vl53l9_metadata_t *p_metadata;
} vl53l9_frame_t;

typedef struct {
    int16_t global_offset;
    int8_t distance_offset[54 * 42];
    int8_t optical_offset_x;
    int8_t optical_offset_y;
    float residual_offset_x;
    float residual_offset_y;
    float rad2perp_fov_gain;
    uint8_t amplitude_coeffs[35]; // 5x7 coefficients grid (starting from bottom left)
    uint8_t amplitude_scaler;
    uint16_t amplitude_distance;
    uint16_t amplitude_exposure;
    uint8_t amplitude_reflectance;
    float dss_short_effective_spad[8];
    float dss_long_effective_spad[8];
} vl53l9_calib_data_t;

/**
 * @brief Parse and process raw calibration data
 * @param[in] buffer The OTP buffer to extract calibration data from
 * @param[out] p_data The calibration data structure to fill
 *
 * @note The buffer consists of the concatenation of otp sections 1 and 2
 */
void vl53l9_utils_parse_calib_data(uint8_t *buffer, vl53l9_calib_data_t *p_data);

/**
 * @brief Parse the raw data buffer and fill the frame structure
 * @param[in] buffer Pointer to the buffer containing the frame
 * @param[in] buffer_size Size of the buffer
 * @param[out] p_frame Pointer to the frame structure
 * @return See @ref VL53L9_ERROR
 */
int vl53l9_utils_parse_frame(uint8_t *buffer, size_t buffer_size, vl53l9_frame_t *p_frame);

/**
 * @brief Get the output frame resolution depending on the binning value
 * @param[in] binning The binning value
 * @param[out] p_width The width of the frame
 * @param[out] p_height The height of the frame
 * @return 0 in case of success, -1 otherwise
 *
 * @note The width and height values don't include the cropped area
 */
int vl53l9_utils_get_resolution(uint8_t binning, uint8_t *p_width, uint8_t *p_height);

#ifdef __cplusplus
}
#endif

#endif // VL53L9_UTILS_H
