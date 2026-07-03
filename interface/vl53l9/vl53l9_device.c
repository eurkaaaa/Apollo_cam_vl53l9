#include "vl53l9_device.h"
#include "interface.h"
// TODO: use symbol to hide i3c instance
#include "main.h"
#include "stm32n6xx_hal.h"
extern I2C_HandleTypeDef hi2c2;
extern I3C_HandleTypeDef hi3c2;

vl53l9_device_t device[NB_DEVICES] = {
#ifdef CONFIG_HW_SENSOR_SHIELD_ARDUINO
    { .bus = &hi3c2,
      .bus_type = PLATFORM_BUS_I3C,
      .address = VL53L9_DEFAULT_ADDRESS,
      .vdda = VDDA_2V8,   // ardunio shield = 2.8V
      .vddio = VDDIO_1V8, // ardunio shield = 1.8V
      .ext_clock = 12e6,  // 12.5MHz = flex shield |  12MHz = SPIDER shield | 12MHz = ardunio shield
      .intr = { .pin = INTR_ARD_Pin, .port = INTR_ARD_GPIO_Port },
      .xshut = { .pin = XSHUT_ARD_Pin, .port = XSHUT_ARD_GPIO_Port } },
#endif
#ifdef CONFIG_HW_SENSOR_FLEX
    { .bus = &hi2c2,
      .bus_type = PLATFORM_BUS_I2C | PLATFORM_BUS_CSI,
      .address = VL53L9_DEFAULT_ADDRESS,
      .vdda = VDDA_2V8,
      .vddio = VDDIO_1V8,
      .ext_clock = 12.5e6, // 12.5MHz = flex shield |  12MHz = SPIDER shield
      .intr = { .pin = INTR_Pin, .port = INTR_GPIO_Port },
      .xshut = { .pin = XSHUT_Pin, .port = XSHUT_GPIO_Port } },
#endif
};
