/**
 *******************************************************************************
 * @file    vl53l9_transform_utils.c
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

#include "vl53l9_transform_utils.h"

#include <math.h>
#include <stdlib.h>

static float cubic_kernel(float d, float a) {
    if (fabs(d) < 1) {
        return (a + 2) * powf(fabs(d), 3) - (a + 3) * powf(d, 2) + 1;
    } else if (fabs(d) < 2) {
        return a * powf(fabs(d), 3) - 5 * a * powf(d, 2) + 8 * a * fabs(d) - 4 * a;
    } else {
        return 0.0f;
    }
}

static void modf_custom(float num, int *int_part, float *frac_part) {
    *int_part = (int)num;
    *frac_part = num - *int_part;
}

void crop(const float *input, size_t input_width, float *output, size_t crop_width, size_t crop_height, int x, int y) {
    for (size_t i = 0; i < crop_height; ++i) {
        for (size_t j = 0; j < crop_width; ++j) {
            int src_x = x + j;
            int src_y = y + i;
            src_x = src_x < 0 ? 0 : (src_x >= input_width ? input_width - 1 : src_x);
            src_y = src_y < 0 ? 0 : (src_y >= input_width ? input_width - 1 : src_y);
            output[i * crop_width + j] = input[src_y * input_width + src_x];
        }
    }
}

void matrix_hadamard_product(const float *a, const float *b, float *result, size_t width, size_t height) {
    for (size_t column = 0; column < width; ++column) {
        for (size_t line = 0; line < height; ++line) {
            result[line * width + column] = a[line * width + column] * b[line * width + column];
        }
    }
}

void bicubic_resize(const float *input, size_t width_in, size_t height_in, float *output, size_t width_out,
                    size_t height_out, float a) {
    float scale_x = (float)width_in / width_out;
    float scale_y = (float)height_in / height_out;

    size_t pad_x = (width_out > width_in) ? (size_t)ceil((width_out - width_in) / 2.0f)
                                          : (size_t)ceil((width_out + width_in) / 2.0f);
    size_t pad_y = (height_out > height_in) ? (size_t)ceil((height_out - height_in) / 2.0f)
                                            : (size_t)ceil((height_out + height_in) / 2.0f);

    size_t padded_width = width_in + pad_x * 2;
    size_t padded_height = height_in + pad_y * 2;
    float *padded_input = (float *)malloc(padded_width * padded_height * sizeof(float));

    for (size_t x = 0; x < padded_width; ++x) {
        for (size_t y = 0; y < padded_height; ++y) {
            int src_x = (int)x - (int)pad_x;
            int src_y = (int)y - (int)pad_y;
            src_x = src_x < 0 ? 0 : (src_x >= width_in ? width_in - 1 : src_x);
            src_y = src_y < 0 ? 0 : (src_y >= height_in ? height_in - 1 : src_y);
            padded_input[y * padded_width + x] = input[src_y * width_in + src_x];
        }
    }

    float offset_x = (1.0f + (width_out - 1.0f) * ((float)width_in / width_out) - width_in) / 2.0f + 1.0f;
    float offset_y = (1.0f + (height_out - 1.0f) * ((float)height_in / height_out) - height_in) / 2.0f + 1.0f;

    for (size_t x1 = 0; x1 < width_out; ++x1) {
        for (size_t y1 = 0; y1 < height_out; ++y1) {
            float x2 = x1 * scale_x + pad_x - offset_x;
            float y2 = y1 * scale_y + pad_y - offset_y;

            int x2_floor;
            float dx2;
            modf_custom(x2, &x2_floor, &dx2);

            int y2_floor;
            float dy2;
            modf_custom(y2, &y2_floor, &dy2);

            float weights[4 * 4];
            for (int nx = -1; nx < 3; ++nx) {
                for (int ny = -1; ny < 3; ++ny) {
                    weights[(ny + 1) * 4 + (nx + 1)] = cubic_kernel(dx2 - nx, a) * cubic_kernel(dy2 - ny, a);
                }
            }

            float sum = 0.0f;
            for (int i = 0; i < 16; ++i) {
                sum += weights[i];
            }
            for (int i = 0; i < 16; ++i) {
                weights[i] /= sum;
            }

            float cropped_padded_input[4 * 4];
            crop(padded_input, padded_width, cropped_padded_input, 4, 4, x2_floor, y2_floor);

            float product[4 * 4];
            matrix_hadamard_product(weights, cropped_padded_input, product, 16, 1);

            float interpolated_value = 0.0f;
            for (int i = 0; i < 16; ++i) {
                interpolated_value += product[i];
            }

            output[y1 * width_out + x1] = interpolated_value;
        }
    }

    free(padded_input);
}
