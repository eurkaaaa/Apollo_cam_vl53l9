/********************************************************************************
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
 *******************************************************************************/
#pragma once

#ifndef DEVICE_H
#define DEVICE_H

#include "interface.h"

//mipi_csi
#define CONFIG_HW_SENSOR_FLEX
//i3c
//#define CONFIG_HW_SENSOR_SHIELD_ARDUINO

#if !defined(CONFIG_HW_SENSOR_FLEX) && !defined(CONFIG_HW_SENSOR_SHIELD_ARDUINO)
#define CONFIG_HW_SENSOR_FLEX
#endif

#if defined(CONFIG_HW_SENSOR_FLEX) && defined(CONFIG_HW_SENSOR_SHIELD_ARDUINO)
#define NB_DEVICES          (2U)
#else
#define NB_DEVICES          (1U)
#endif

extern vl53l9_device_t device[NB_DEVICES];


#endif // DEVICE_H
