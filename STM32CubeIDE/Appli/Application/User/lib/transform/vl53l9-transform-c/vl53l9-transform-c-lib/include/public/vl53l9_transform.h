/**
 *******************************************************************************
 * @file    vl53l9_transform.h
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

#ifndef VL53L9_TRANSFORM_C_H
#define VL53L9_TRANSFORM_C_H

#include "transform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VL53L9_TRANSFORM_MAJOR (0)
#define VL53L9_TRANSFORM_MINOR (3)
#define VL53L9_TRANSFORM_PATCH (2)

#define VL53L9_TRANSFORM_DEBUG (0)

transform_t *vl53l9_transform_create();
void vl53l9_transform_destroy(transform_t *self);

#ifdef __cplusplus
}
#endif

#endif // VL53L9_TRANSFORM_C_H
