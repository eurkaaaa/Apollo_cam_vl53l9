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

#include "algo/ratenorm.h"

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define REF_COEF_MAP_WIDTH    (7)
#define REF_COEF_MAP_HEIGH    (5)
#define REF_COEF_MAP_SIZE     (REF_COEF_MAP_WIDTH * REF_COEF_MAP_HEIGH)
#define MAX_IMAGE_WIDTH       (108)
#define MAX_IMAGE_HEIGHT      (84)
#define MAX_IMAGE_SIZE        (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT)
#define MAX_PADDED_IMAGE_SIZE ((MAX_IMAGE_WIDTH * 2) * (MAX_IMAGE_HEIGHT * 2))

void vl53l9_algo_ratenorm_bicubic_resize(const float *input,

                                         float *output,

                                         unsigned width_in, unsigned height_in, unsigned width_out, unsigned height_out,
                                         float a);

int vl53l9_algo_ratenorm_compute_norm_maps(const float *reference_coefs, const float *r2p_coefs,

                                           float *ref_amp_no_expo, float *ref_amp_rad_no_expo,
                                           float *coeff_norm_no_expo,

                                           ratenorm_params_t params, unsigned width, unsigned height, unsigned binning);

int vl53l9_algo_ratenorm_compute_rates(const float *amplitude, const float *ambient, const float *effective_spads,
                                       const bool *main_flag, const float *ref_amp_no_expo,
                                       const float *ref_amp_rad_no_expo, const float *coeff_norm_no_expo,

                                       float *ref_amp, float *ref_amp_rad, float *signal_photon_rate,
                                       float *ambient_photon_rate, float *rescaled_ambient,

                                       ratenorm_params_t params, unsigned size, unsigned step_number, unsigned expo_sf,
                                       unsigned expo_sc, unsigned expo_sa, unsigned ambient_attenuation);

static inline float maxf(float a, float b) {
    return a > b ? a : b;
}
static inline float over_op(float a, float b) {
    (void)a;
    return b;
}
static inline float mult_op(float a, float b) {
    return a * b;
}

/**
 * @brief bicubic interpolation helper
 *
 * @param d bicubic input
 * @param a bicubic coef (-0.75 is standard)
 * @return float kernel result
 */
static inline float cubic_kernel(float d, float a);

/**
 * @brief modified signature for unsigned modf
 *
 * @param num number to process
 * @param uint_part unsigned part
 * @param frac_part fractionnal part
 */
static inline void modf_custom(float num, unsigned *uint_part, float *frac_part);

/**
 * @brief unsigned clamp function
 *
 * @param value valeu to clamp
 * @param min min included
 * @param max max included
 * @return unsigned clamped value
 */
static inline unsigned clamp(unsigned value, unsigned min, unsigned max);

/**
 * @brief crop input array to output array
 *
 * @param input input array
 * @param output output array
 * @param input_width input width
 * @param crop_width crop width
 * @param crop_height crop height
 * @param offset_x crop offset from left
 * @param offset_y crop offset from top
 */
static void crop(const float *input, float *output, unsigned input_width, unsigned crop_width, unsigned crop_height,
                 unsigned offset_x, unsigned offset_y);

/**
 * @brief appy binning to input array in output array
 *
 * @param input input array
 * @param output output array
 * @param width input width
 * @param height input height
 * @param bin_x horizontal binning
 * @param bin_y vertical binning
 */
static void bin(const float *input, float *output, unsigned width, unsigned height, unsigned bin_x, unsigned bin_y);

/**
 * @brief apply hadamard product of 2 matrices and write it to output
 *
 * @param a input array 1
 * @param b input array 2
 * @param result outptu array
 * @param width input/output width
 * @param height input/output height
 */
static void matrix_hadamard_product(const float *a, const float *b, float *result, unsigned width, unsigned height);

/**
 * @brief apply binary operator to each value of an array and a constant
 *
 * @param array array to iterate over
 * @param width array width
 * @param height array height
 * @param coef rhs coef
 * @param range range to transform same form as python
 * @param op binary operator to use
 */
static void transform(float *array, unsigned width, unsigned height, float coef, const char *range,
                      float op(float, float));

/**
 * @brief pad input array to output with nearest neighbor behaviour on edges
 *
 * @param input input array
 * @param output outptu array
 * @param width input array width
 * @param height input array height
 * @param pad_left left padding
 * @param pad_right right padding
 * @param pad_top top padding
 * @param pad_bottom bottom padding
 */
static void pad_array_nn(const float *input, float *output, unsigned width, unsigned height, unsigned pad_left,
                         unsigned pad_right, unsigned pad_top, unsigned pad_bottom);

/**
 * @brief split str according to first separator encountered
 *
 * @param to_split string to split
 * @param str1 output left split string
 * @param str1_size output left split string size
 * @param str2 output right split string
 * @param str2_size output right split string size
 * @param size input size
 * @param sep separator
 */
static void split_str(const char *to_split, char *str1, unsigned *str1_size, char *str2, unsigned *str2_size,
                      unsigned size, char sep);

/**
 * @brief returns true if all str characters are digits
 *
 * @param str intput string
 * @param size input string size
 * @return true all characters are digits
 * @return false at least one character is not a digit
 */
static bool is_number(const char *str, unsigned size);

void vl53l9_algo_ratenorm_bicubic_resize(const float *input,

                                         float *output,

                                         unsigned width_in, unsigned height_in, unsigned width_out, unsigned height_out,
                                         float a) {
    float scale_x = (float)width_in / width_out;
    float scale_y = (float)height_in / height_out;

    unsigned pad_x = (width_out > width_in) ? (unsigned)ceil((width_out - width_in) / 2.0f)
                                            : (unsigned)ceil((width_out + width_in) / 2.0f);
    unsigned pad_y = (height_out > height_in) ? (unsigned)ceil((height_out - height_in) / 2.0f)
                                              : (unsigned)ceil((height_out + height_in) / 2.0f);

    unsigned padded_width = width_in + pad_x * 2;
    unsigned padded_height = height_in + pad_y * 2;

    float padded_input[MAX_PADDED_IMAGE_SIZE];

    for (unsigned x = 0; x < padded_width; ++x) {
        for (unsigned y = 0; y < padded_height; ++y) {
            int src_x = (int)x - (int)pad_x;
            int src_y = (int)y - (int)pad_y;
            src_x = src_x < 0 ? 0 : (src_x >= (int)width_in ? (int)width_in - 1 : src_x);
            src_y = src_y < 0 ? 0 : (src_y >= (int)height_in ? (int)height_in - 1 : src_y);
            padded_input[y * padded_width + x] = input[src_y * width_in + src_x];
        }
    }

    float offset_x = (1.0f + (width_out - 1.0f) * ((float)width_in / width_out) - width_in) / 2.0f + 1.0f;
    float offset_y = (1.0f + (height_out - 1.0f) * ((float)height_in / height_out) - height_in) / 2.0f + 1.0f;

    for (unsigned x1 = 0; x1 < width_out; ++x1) {
        for (unsigned y1 = 0; y1 < height_out; ++y1) {
            float x2 = x1 * scale_x + pad_x - offset_x;
            float y2 = y1 * scale_y + pad_y - offset_y;

            unsigned x2_floor;
            float dx2;
            modf_custom(x2, &x2_floor, &dx2);

            unsigned y2_floor;
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
            crop(padded_input, cropped_padded_input, padded_width, 4, 4, x2_floor, y2_floor);

            float product[4 * 4];
            matrix_hadamard_product(weights, cropped_padded_input, product, 16, 1);

            float interpolated_value = 0.0f;
            for (int i = 0; i < 16; ++i) {
                interpolated_value += product[i];
            }

            output[y1 * width_out + x1] = interpolated_value;
        }
    }
}

int vl53l9_algo_ratenorm_compute_norm_maps(const float *reference_coefs, const float *r2p_coefs,

                                           float *ref_amp_no_expo, float *ref_amp_rad_no_expo,
                                           float *coeff_norm_no_expo,

                                           ratenorm_params_t params, unsigned width, unsigned height,
                                           unsigned binning) {
    float bin_factor = binning != 2 ? binning / 2.0f : 1.0f;

    if (params.fast_mode) {
        float amp_ref_val = 0.0f;
        for (unsigned i = 0; i < REF_COEF_MAP_SIZE; ++i) {
            amp_ref_val = maxf(amp_ref_val, reference_coefs[i]);
        }

        amp_ref_val *= params.ref_scaler * params.ref_distance * params.ref_distance * params.peak_l_scaler *
                       bin_factor * params.max_spads /
                       (params.ref_expo * params.ref_reflectance * params.max_spads_ref * 1e6f);

        for (unsigned i = 0; i < width * height; ++i) {
            const float r2p_coef_sq = r2p_coefs[i] * r2p_coefs[i];
            const float ref_amp_rad_no_expo_tmp = amp_ref_val * r2p_coef_sq * r2p_coef_sq;
            if (ref_amp_rad_no_expo) {
                ref_amp_rad_no_expo[i] = ref_amp_rad_no_expo_tmp;
            }
            if (ref_amp_no_expo) {
                ref_amp_no_expo[i] = ref_amp_rad_no_expo_tmp * r2p_coef_sq;
            }
            if (coeff_norm_no_expo) {
                coeff_norm_no_expo[i] = amp_ref_val / (ref_amp_rad_no_expo_tmp * r2p_coef_sq);
            }
        }
    }
    // else {
    //     float lcoefs[REF_COEF_MAP_SIZE] = { 0.0f };
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, 1.0f, ":, :", over_op);

    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.left_side_l_scaler, "0:5, 0", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.right_side_l_scaler, "0:5, 6", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.middle_section_l_scaler, "0:5, 1", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.middle_section_l_scaler, "0:5, 5", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.middle_section_l_scaler, "1, 1:6", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.middle_section_l_scaler, "3, 1:6", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.top_row_l_scaler, "0, 1:6", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.top_row_l_scaler, "1, 1", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.top_row_l_scaler, "1, 5", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.bottom_row_l_scaler, "4, 1:6", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.bottom_row_l_scaler, "3, 1", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.bottom_row_l_scaler, "3, 5", over_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.center_region_l_scaler, "1:4, 2:5",
    //     over_op); transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.peak_l_scaler, "2, 3", over_op);

    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.corner_post_l_scaler, "0, 0", mult_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.corner_post_l_scaler, "4, 0", mult_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.corner_post_l_scaler, "0, 6", mult_op);
    //     transform(lcoefs, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, params.corner_post_l_scaler, "4, 6", mult_op);

    //     float corr_ref[REF_COEF_MAP_SIZE] = { 0.0f };
    //     for (unsigned i = 0; i < REF_COEF_MAP_SIZE; ++i) {
    //         corr_ref[i] = reference_coefs[i] * lcoefs[i];
    //     }

    //     float corr_ref_padded[(REF_COEF_MAP_WIDTH + 2) * (REF_COEF_MAP_HEIGH + 2)];
    //     pad_array_nn(corr_ref, corr_ref_padded, REF_COEF_MAP_WIDTH, REF_COEF_MAP_HEIGH, 1, 1, 1, 1);

    //     float (*mul)(float, float) = mult_op; // convenience alias
    //     transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH + 2, params.side_scaler, "1:6, 0",
    //     mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH + 2, params.side_scaler, "1:6,
    //     8", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH + 2, params.side_scaler, "0,
    //     1:8", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH + 2, params.side_scaler,
    //     "6, 1:8", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH + 2,
    //     params.corner_scaler, "0, 0", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH +
    //     2, params.corner_scaler, "6, 0", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2, REF_COEF_MAP_HEIGH
    //     + 2, params.corner_scaler, "0, 8", mul); transform(corr_ref_padded, REF_COEF_MAP_WIDTH + 2,
    //     REF_COEF_MAP_HEIGH + 2, params.corner_scaler, "6, 8", mul);

    //     unsigned output_width_bin = width;
    //     unsigned output_height_bin = height;
    //     if (bin_factor > 1.0f && !remainderf((float)binning, (float)params.nominal_binning)) {
    //         output_width_bin = params.nominal_width;
    //         output_height_bin = params.nominal_height;
    //     }

    //     float corr_ref_padded_resized[MAX_PADDED_IMAGE_SIZE] = { 0.0f };
    //     vl53l9_algo_ratenorm_bicubic_resize(corr_ref_padded, corr_ref_padded_resized, REF_COEF_MAP_WIDTH + 2,
    //                                         REF_COEF_MAP_HEIGH + 2, output_width_bin + 10, output_height_bin + 8,
    //                                         params.bicubic_coef);
    //     float corr_ref_padded_resized_cropped[MAX_PADDED_IMAGE_SIZE] = { 0.0f };
    //     crop(corr_ref_padded_resized, corr_ref_padded_resized_cropped, output_width_bin + 10, output_width_bin,
    //          output_height_bin, 5, 4);
    //     transform(corr_ref_padded_resized_cropped, output_width_bin, output_height_bin, params.main_scaler,
    //               ":, :", mult_op);

    //     if (bin_factor > 1.0f && !remainderf((float)binning, (float)params.nominal_binning)) {
    //         unsigned bin_x = (params.nominal_width * params.nominal_binning < width * binning)
    //                              ? params.nominal_width / width * params.nominal_binning
    //                              : binning;
    //         unsigned bin_y = (params.nominal_height * params.nominal_binning < height * binning)
    //                              ? params.nominal_height / height * params.nominal_binning
    //                              : binning;

    //         unsigned offset_x =
    //             (params.nominal_width * params.nominal_binning - width * bin_x) / (2 * params.nominal_binning);
    //         unsigned offset_y =
    //             (params.nominal_height * params.nominal_binning - height * bin_y) / (2 * params.nominal_binning);

    //         float corr_ref_padded_resized_cropped_cropped[MAX_IMAGE_SIZE] = { 0.0f };
    //         crop(corr_ref_padded_resized_cropped, corr_ref_padded_resized_cropped_cropped, params.nominal_width,
    //              params.nominal_width - 2 * offset_x, params.nominal_height - 2 * offset_y, offset_x, offset_y);
    //         size_t const image_size = MAX_IMAGE_SIZE * sizeof(float);
    //         memset(corr_ref_padded_resized_cropped, 0, image_size);
    //         bin(corr_ref_padded_resized_cropped_cropped, corr_ref_padded_resized_cropped,
    //             params.nominal_width - 2 * offset_x, params.nominal_height - 2 * offset_y,
    //             bin_x / params.nominal_binning, bin_y / params.nominal_binning);
    //     }

    //     if (remainderf((float)binning, (float)params.nominal_binning)) {
    //         transform(corr_ref_padded_resized_cropped, output_width_bin, output_height_bin, bin_factor * bin_factor,
    //                   ":, :", mult_op);
    //     }

    //     const float coef = params.ref_scaler * params.ref_distance * params.ref_distance * params.max_spads /
    //                        (params.ref_expo * params.ref_reflectance * 1e6f * params.max_spads_ref);

    //     transform(corr_ref_padded_resized_cropped, width, height, coef, ":, :", mult_op);

    //     if (ref_amp_no_expo) {
    //         memcpy(ref_amp_no_expo, corr_ref_padded_resized_cropped, width * height * sizeof(float));
    //     }

    //     float ref_max = 0.0f;
    //     for (unsigned i = 0; i < width * height; ++i) {
    //         ref_max = maxf(ref_max, corr_ref_padded_resized_cropped[i]);
    //     }

    //     float ref_amp_rad_no_expo_tmp[MAX_IMAGE_SIZE] = { 0.0f };
    //     switch (params.ref_mode) {
    //     default:
    //     case 0:
    //         for (unsigned i = 0; i < width * height; ++i) {
    //             ref_amp_rad_no_expo_tmp[i] = corr_ref_padded_resized_cropped[i] / (r2p_coefs[i] * r2p_coefs[i]);
    //         }
    //         break;

    //     case 1:
    //         for (unsigned i = 0; i < width * height; ++i) {
    //             ref_amp_rad_no_expo_tmp[i] = ref_max * r2p_coefs[i] * r2p_coefs[i];
    //         }
    //         break;

    //     case 2:
    //         for (unsigned i = 0; i < width * height; ++i) {
    //             ref_amp_rad_no_expo_tmp[i] = (float)params.ref_amp_const * r2p_coefs[i] * r2p_coefs[i];
    //         }
    //         break;
    //     }

    //     if (coeff_norm_no_expo) {
    //         for (unsigned i = 0; i < width * height; ++i) {
    //             coeff_norm_no_expo[i] = ref_max / ref_amp_rad_no_expo_tmp[i];
    //         }
    //     }

    //     for (unsigned i = 0; i < width * height; ++i) {
    //         ref_amp_rad_no_expo_tmp[i] /= r2p_coefs[i] * r2p_coefs[i];
    //     }

    //     if (ref_amp_rad_no_expo) {
    //         memcpy(ref_amp_rad_no_expo, ref_amp_rad_no_expo_tmp, width * height * sizeof(float));
    //     }
    // }

    return EXIT_SUCCESS;
}

int vl53l9_algo_ratenorm_compute_rates(const float *amplitude, const float *ambient, const float *effective_spads,
                                       const bool *main_flag, const float *ref_amp_no_expo,
                                       const float *ref_amp_rad_no_expo, const float *coeff_norm_no_expo,

                                       float *ref_amp, float *ref_amp_rad, float *signal_photon_rate,
                                       float *ambient_photon_rate, float *rescaled_ambient,

                                       ratenorm_params_t params, unsigned size, unsigned step_number, unsigned expo_sf,
                                       unsigned expo_sc, unsigned expo_sa, unsigned ambient_attenuation) {
    const float expo_width = 1e-9f * (step_number == 7 ? params.exposure_width_0 : params.exposure_width_1);
    for (unsigned i = 0; i < size; ++i) {
        const unsigned expo = main_flag[i] ? expo_sf : expo_sc;
        float ref_amp_rad_tmp = ref_amp_rad_no_expo[i] * expo_sf;
        if (ref_amp_rad) {
            ref_amp_rad[i] = ref_amp_rad_tmp;
        }
        float ref_amp_tmp = ref_amp_no_expo[i] * expo_sf;
        if (ref_amp) {
            ref_amp[i] = ref_amp_tmp;
        }

        if (signal_photon_rate) {
            signal_photon_rate[i] = fmaxf(0.0f, coeff_norm_no_expo[i] * params.signal_factor * amplitude[i] /
                                                    (expo_width * expo * effective_spads[i]));
        }

        const float rescaled_ambient_tmp = ambient[i] * (1u << ambient_attenuation);
        if (ambient_photon_rate) {
            ambient_photon_rate[i] =
                fmaxf(0.0f, rescaled_ambient_tmp / ((params.ambient_window + params.ambient_blanking) * 1e-9f *
                                                    expo_sa * effective_spads[i]));
        }
        if (rescaled_ambient) {
            rescaled_ambient[i] = rescaled_ambient_tmp;
        }
    }

    return EXIT_SUCCESS;
}

inline float cubic_kernel(float d, float a) {
    if (fabsf(d) < 1) {
        return (a + 2) * powf(fabsf(d), 3) - (a + 3) * powf(d, 2) + 1;
    } else if (fabsf(d) < 2) {
        return a * powf(fabsf(d), 3) - 5 * a * powf(d, 2) + 8 * a * fabsf(d) - 4 * a;
    } else {
        return 0.0f;
    }
}

inline void modf_custom(float num, unsigned *uint_part, float *frac_part) {
    float fuint_part;
    *frac_part = modff(num, &fuint_part);
    *uint_part = (unsigned)fuint_part;
}

inline unsigned clamp(unsigned value, unsigned min, unsigned max) {
    return value < min ? min : (value > max ? max : value);
}

void crop(const float *input, float *output, unsigned input_width, unsigned crop_width, unsigned crop_height,
          unsigned offset_x, unsigned offset_y) {
    for (unsigned i = 0; i < crop_height; ++i) {
        for (unsigned j = 0; j < crop_width; ++j) {
            unsigned src_x = offset_x + j;
            unsigned src_y = offset_y + i;
            src_x = src_x >= input_width ? input_width - 1 : src_x;
            src_y = src_y >= input_width ? input_width - 1 : src_y;
            output[i * crop_width + j] = input[src_y * input_width + src_x];
        }
    }
}

void bin(const float *input, float *output, unsigned width, unsigned height, unsigned bin_x, unsigned bin_y) {
    for (unsigned line = 0; line < height; ++line) {
        for (unsigned col = 0; col < width; ++col) {
            unsigned input_id = line * width + col;
            unsigned output_id = line / bin_y * width / bin_x + col / bin_x;
            output[output_id] += input[input_id];
        }
    }
}

void matrix_hadamard_product(const float *a, const float *b, float *result, unsigned width, unsigned height) {
    for (unsigned column = 0; column < width; ++column) {
        for (unsigned line = 0; line < height; ++line) {
            result[line * width + column] = a[line * width + column] * b[line * width + column];
        }
    }
}

void transform(float *array, unsigned width, unsigned height, float coef, const char *range, float op(float, float)) {
    char range_filtered[50] = { 0 };
    unsigned j = 0;
    for (unsigned i = 0; i < 50; ++i) {
        if (range[i] != ' ') {
            range_filtered[j++] = range[i];
        }
    }

    unsigned column_start, column_end, line_start, line_end;

    char columns[50] = { 0 };
    unsigned columns_size = 0;
    char lines[50] = { 0 };
    unsigned lines_size = 0;
    split_str(range_filtered, lines, &lines_size, columns, &columns_size, 50, ',');

    if (lines[0] == ':') {
        line_start = 0;
        line_end = height - 1;
    } else if (is_number(lines, lines_size)) {
        line_start = strtoul(lines, NULL, 10);
        line_end = line_start;
    } else {
        char line_start_str[50] = { 0 };
        unsigned line_start_str_size = 0;
        char line_end_str[50] = { 0 };
        unsigned line_end_str_size = 0;
        split_str(lines, line_start_str, &line_start_str_size, line_end_str, &line_end_str_size, 50, ':');
        line_start = strtoul(line_start_str, NULL, 10);
        line_end = strtoul(line_end_str, NULL, 10) - 1;
    }

    if (columns[0] == ':') {
        column_start = 0;
        column_end = width - 1;
    } else if (is_number(columns, columns_size)) {
        column_start = strtoul(columns, NULL, 10);
        column_end = column_start;
    } else {
        char column_start_str[50] = { 0 };
        unsigned column_start_str_size = 0;
        char column_end_str[50] = { 0 };
        unsigned column_end_str_size = 0;
        split_str(columns, column_start_str, &column_start_str_size, column_end_str, &column_end_str_size, 50, ':');
        column_start = strtoul(column_start_str, NULL, 10);
        column_end = strtoul(column_end_str, NULL, 10) - 1;
    }

    for (unsigned line = line_start; line <= line_end; ++line) {
        for (unsigned column = column_start; column <= column_end; ++column) {
            unsigned linear_id = line * width + column;
            array[linear_id] = op(array[linear_id], coef);
        }
    }
}

void pad_array_nn(const float *input, float *output, unsigned width, unsigned height, unsigned pad_left,
                  unsigned pad_right, unsigned pad_top, unsigned pad_bottom) {
    for (unsigned line = 0; line < height + pad_top + pad_bottom; ++line) {
        for (unsigned column = 0; column < width + pad_left + pad_right; ++column) {
            unsigned input_id = (clamp(line, pad_top, height + pad_top - 1u) - pad_top) * width +
                                (clamp(column, pad_left, width + pad_left - 1u) - pad_left);
            output[line * (width + pad_left + pad_right) + column] = input[input_id];
        }
    }
}

void split_str(const char *to_split, char *str1, unsigned *str1_size, char *str2, unsigned *str2_size, unsigned size,
               char sep) {
    bool sep_encountered = false;
    *str2_size = 0;
    *str1_size = 0;
    for (unsigned i = 0; i < size; ++i) {
        if (sep_encountered) {
            if (to_split[i] != '\0') {
                str2[(*str2_size)++] = to_split[i];
            } else {
                // EOL of str2 encountered, returning
                return;
            }
        } else {
            if (to_split[i] == sep) {
                sep_encountered = true;
                *str1_size = i;
            } else {
                str1[i] = to_split[i];
            }
        }
    }

    ++(*str2_size);
}

bool is_number(const char *str, unsigned size) {
    for (unsigned i = 0; i < size; ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
