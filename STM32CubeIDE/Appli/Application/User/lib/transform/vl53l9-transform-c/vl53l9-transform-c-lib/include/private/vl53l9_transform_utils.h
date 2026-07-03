/**
 *******************************************************************************
 * @file    vl53l9_transform_utils.h
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

#ifndef VL53L9_TRANSFORM_UTILS_H
#define VL53L9_TRANSFORM_UTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crop an image
 * @param input[in] The input image
 * @param input_width[in] The width of the input image
 * @param output[out] The output image
 * @param crop_width[in] The width of the output image
 * @param crop_height[in] The height of the output image
 * @param x[in] The x coordinate of the top-left corner of the crop
 * @param y[in] The y coordinate of the top-left corner of the crop
 */
void crop(const float *input, size_t input_width, float *output, size_t crop_width, size_t crop_height, int x, int y);

/**
 * @brief Compute the Hadamard product of two matrices
 * @param a[in] The first matrix
 * @param b[in] The second matrix
 * @param result[out] The result of the Hadamard product
 * @param width[in] The width of the matrices
 * @param height[in] The height of the matrices
 *
 * @note The result matrix must be pre-allocated
 * @note The size of the matrices must be the same
 * @note The matrices must be stored in row-major order
 */
void matrix_hadamard_product(const float *a, const float *b, float *result, size_t width, size_t height);

/**
 * @brief Resize an image using bicubic interpolation
 * @param input[in] The input image
 * @param width_in[in] The width of the input image
 * @param height_in[in] The height of the input image
 * @param output[out] The output image
 * @param width_out[in] The width of the output image
 * @param height_out[in] The height of the output image
 * @param a[in] The bicubic kernel coefficient
 */
void bicubic_resize(const float *input, size_t width_in, size_t height_in, float *output, size_t width_out,
                    size_t height_out, float a);

#ifdef __cplusplus
}
#endif

#endif // VL53L9_TRANSFORM_UTILS_H
