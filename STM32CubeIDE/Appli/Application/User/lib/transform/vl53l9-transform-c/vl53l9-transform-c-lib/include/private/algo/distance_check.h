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

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Filter unrealible values from input image depending on validity flags
 */
void vl53l9_algo_distance_check(const float *const distance_in, const float *const dmax_in,
                                const bool *const r2p_valid_in, const bool *const confidence_valid_in,
                                const bool *const reflectance_valid_in, const bool *const sharpener_valid_in,
                                const bool *const fp_valid_in,

                                float *const distance_out, unsigned char *const status_out,

                                size_t size, float invalid_distance_default, bool r2p_filter, bool confidence_filter,
                                bool reflectance_filter, bool sharpener_filter, bool fp_filter, bool dmax_select);
