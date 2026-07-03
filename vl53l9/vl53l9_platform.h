/**
 *******************************************************************************
 * @file    vl53l9_platform.h
 * @brief   VL53L9 platform header file
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

#ifndef VL53L9_PLATFORM_H
#define VL53L9_PLATFORM_H

#include "vl53l9.h"
#include <stdint.h>

/**
 * An implementation of the following methods must be provided by the integrator to enable register-level operations.
 * The implementation must return VL53L9_ERROR_NONE on success, or VL53L9_ERROR_PLATFORM code otherwise.
 */

int vl53l9_read(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size);
int vl53l9_read8(void *const p_dev, uint16_t address, uint8_t *p_value);
int vl53l9_read16(void *const p_dev, uint16_t address, uint16_t *p_value);
int vl53l9_read32(void *const p_dev, uint16_t address, uint32_t *p_value);

int vl53l9_read_async(void *const p_dev, uint16_t address, volatile uint8_t *p_values, uint32_t size);

int vl53l9_write(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size);
int vl53l9_write8(void *const p_dev, uint16_t address, uint8_t value);
int vl53l9_write16(void *const p_dev, uint16_t address, uint16_t value);
int vl53l9_write32(void *const p_dev, uint16_t address, uint32_t value);

int vl53l9_wait_ms(void *const p_dev, uint32_t delay_ms);



int vl53l9_get_config_vddio(void *const p_dev, vl53l9_vddio_t *voltage);
int vl53l9_get_config_vdda(void *const p_dev, vl53l9_vdda_t *voltage);
int vl53l9_get_config_ext_clock(void *const p_dev, uint32_t *ext_clock);

#endif // VL53L9_PLATFORM_H
