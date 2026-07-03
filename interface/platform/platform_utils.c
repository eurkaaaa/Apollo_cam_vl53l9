// private includes
#include "interface.h"
#include "main.h"
#include "stm32n6xx_hal.h"
#include "vl53l9_device.h"
#include "vl53l9_platform.h"
#include <stdio.h>
#include <string.h>

#define VL53L9_BOARD_NAME_STR "STM32N6570-DK"
#define FW_MAJOR              (0)
#define FW_MINOR              (5)
#define FW_PATCH              (0)

extern DCMIPP_HandleTypeDef hdcmipp;
extern I3C_HandleTypeDef hi3c2;
extern I2C_HandleTypeDef hi2c2;

volatile platform_event_t platform_evt;

// the following are used during the I3C DAA procedure
static unsigned int dev_count = 0;
static uint64_t dev_payload[NB_DEVICES];

/* private functions */

static int _timeout_expire(uint32_t to_start, uint32_t to_value) {
    uint64_t currentlong = HAL_GetTick();
    uint32_t current = (uint32_t)currentlong;

    return current >= to_start ? (current - to_start) >= to_value : (current + (0xffffffff - to_start) + 1) >= to_value;
}

int platform_start_csi_pipe(uint8_t *buff_csi) {
    if (HAL_OK != HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE0, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)buff_csi,
                                            DCMIPP_MODE_CONTINUOUS)) {
        return -1;
    }
    return 0;
}

int platform_stop_csi_pipe(void) {
    if (HAL_OK != HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE0, DCMIPP_VIRTUAL_CHANNEL0)) {
        return -1;
    }
    return 0;
}

void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe) {
    if (Pipe == DCMIPP_PIPE0) {
        platform_evt |= PLATFORM_CAM_PIPE_FRAME_EVT;
    }
}

int platform_enable_event(platform_event_t plt_evt) {
    uint8_t res = 0;
    switch (plt_evt) {
    case PLATFORM_GPIO_IT_EVT:
        platform_evt &= ~PLATFORM_GPIO_IT_EVT;
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
        HAL_NVIC_EnableIRQ(EXTI6_IRQn);
        break;
    case PLATFORM_CAM_PIPE_FRAME_EVT:
        platform_evt &= ~PLATFORM_CAM_PIPE_FRAME_EVT;
        HAL_NVIC_EnableIRQ(CSI_IRQn);
        break;
    default:
        res = -1;
        break;
    }
    return res;
}

int platform_disable_event(platform_event_t plt_evt) {
    uint8_t res = 0;
    switch (plt_evt) {
    case PLATFORM_GPIO_IT_EVT:
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
        HAL_NVIC_DisableIRQ(EXTI6_IRQn);
        platform_evt &= ~PLATFORM_GPIO_IT_EVT;
        break;
    case PLATFORM_CAM_PIPE_FRAME_EVT:
        HAL_NVIC_DisableIRQ(CSI_IRQn);
        platform_evt &= ~PLATFORM_CAM_PIPE_FRAME_EVT;
        break;
    default:
        res = -1;
        break;
    }
    return res;
}

int platform_acknowledge_event(platform_event_t plt_evt) {
    uint8_t res = 0;
    switch (plt_evt) {
    case PLATFORM_GPIO_IT_EVT:
        platform_evt &= ~PLATFORM_GPIO_IT_EVT;
        break;
    case PLATFORM_CAM_PIPE_FRAME_EVT:
        platform_evt &= ~PLATFORM_CAM_PIPE_FRAME_EVT;
        break;
    case PLATFORM_I3C_DMA_RX_EVT:
        platform_evt &= ~PLATFORM_I3C_DMA_RX_EVT;
        break;
    case PLATFORM_I3C_IBI_EVT:
        platform_evt &= ~PLATFORM_I3C_IBI_EVT;
    default:
        res = -1;
        break;
    }
    return res;
}

int platform_wait_for_event(platform_event_t plt_evt, uint32_t to_ms) {
    uint8_t res = 0;
    uint64_t to_startlong = HAL_GetTick();
    uint32_t to_start = (uint32_t)to_startlong;
    while (!(plt_evt & platform_evt) && !_timeout_expire(to_start, to_ms))
        ;

    if (!(plt_evt & platform_evt)) {
        res = -1;
    }
    return res;
}

int platform_get_event_status(platform_event_t plt_evt, bool *active) {
    *active = (platform_evt & plt_evt) ? true : false;
    return 0;
}

/**
 * @brief get firmware version
 * @param fw_ver: structure filled with firmware version details
 * @return 0 if success
 */
int platform_get_version(platform_version_t *version) {

    version->interface = (_version_t){ .major = INTERFACE_MAJOR, .minor = INTERFACE_MINOR, .patch = INTERFACE_PATCH };
    version->firmware = (_version_t){ .major = FW_MAJOR, .minor = FW_MINOR, .patch = FW_PATCH };
    version->driver =
        (_version_t){ .major = VL53L9_CORE_MAJOR, .minor = VL53L9_CORE_MINOR, .patch = VL53L9_CORE_PATCH };

    strncpy(version->board_name, VL53L9_BOARD_NAME_STR, BOARD_NAME_STR_SIZE);

    return 0;
}

int platform_set_device_address(uint8_t id, uint8_t address) {
    if (device[id].bus_type == PLATFORM_BUS_I2C) {
        device[id].address = address & 0x7F;
        return 0;
    } else {
        return -1;
    }
}

int platform_power_reset(uint8_t id) {
    HAL_GPIO_WritePin((GPIO_TypeDef *)device[id].xshut.port, device[id].xshut.pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin((GPIO_TypeDef *)device[id].xshut.port, device[id].xshut.pin, GPIO_PIN_SET);
    HAL_Delay(50);
    return 0;
}

int platform_power_enable(uint8_t id) {
    HAL_GPIO_WritePin((GPIO_TypeDef *)device[id].xshut.port, device[id].xshut.pin, GPIO_PIN_SET);
    HAL_Delay(50);
    return 0;
}

int platform_power_disable(uint8_t id) {
    HAL_GPIO_WritePin((GPIO_TypeDef *)device[id].xshut.port, device[id].xshut.pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    return 0;
}

int32_t platform_start_bus(uint8_t id, platform_bus_type_t bus_type) {
    // todo test if bus is already started
    if (bus_type == PLATFORM_BUS_I2C) {
        if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
            return -1;
        }
    } else if (bus_type == PLATFORM_BUS_I3C) {
        if (HAL_I3C_Init(&hi3c2) != HAL_OK) {
            return -1;
        }
    }
    device[id].bus_type = bus_type;

    return 0;
}

int platform_assign_dynamic_address() {
    // TODO: how to handle dynamic address assignment for multiple devices?
    // fixme: fast work arrond
    uint8_t address = 0x52;

    if (device[0].bus_type & PLATFORM_BUS_I3C) {
        HAL_StatusTypeDef status;
        uint64_t payload;

        if (HAL_I3C_DeInit(&hi3c2) != HAL_OK) {
            return -1;
        }
        // set i3c bus frequency to 1 MHz before dynamic address assignment
        hi3c2.Instance = I3C2;
        hi3c2.Mode = HAL_I3C_MODE_CONTROLLER;
        hi3c2.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_1_5;
        hi3c2.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
        hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x63;
        hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x63;
        hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x63;
        hi3c2.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x00;
        hi3c2.Init.CtrlBusCharacteristic.BusFreeDuration = 0x27;
        hi3c2.Init.CtrlBusCharacteristic.BusIdleDuration = 0xc6;
        if (HAL_I3C_Init(&hi3c2) != HAL_OK) {
            return -1;
        }

        // NOTE: for the moment apply static address as dynamic address
        do {
            status = HAL_I3C_Ctrl_DynAddrAssign(&hi3c2, &payload, I3C_RSTDAA_THEN_ENTDAA, 5000);
            if (status == HAL_BUSY) {
                HAL_I3C_Ctrl_SetDynAddr(&hi3c2, address & 0x7F);
                dev_count++;
                dev_payload[dev_count - 1] = payload;
            }
        } while (status == HAL_BUSY);

        // set i3c bus frequency to 10 MHz after dynamic address assignement

        if (HAL_I3C_DeInit(&hi3c2) != HAL_OK) {
            return -1;
        }
        hi3c2.Instance = I3C2;
        hi3c2.Mode = HAL_I3C_MODE_CONTROLLER;
        hi3c2.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_1_5;
        hi3c2.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
        hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x09;
        hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x09;
        hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x47;
        hi3c2.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x00;
        hi3c2.Init.CtrlBusCharacteristic.BusFreeDuration = 0x27;
        hi3c2.Init.CtrlBusCharacteristic.BusIdleDuration = 0xc6;
        if (HAL_I3C_Init(&hi3c2) != HAL_OK) {
            return -1;
        }

        // NOTE: the platform supports notifications only for four devices
        for (int i = 0; i < dev_count && i < 4; i++) {
            //    I3C_DeviceConfTypeDef DeviceConf;
            //    DeviceConf.DeviceIndex = i + 1; // 1 <= DeviceIndex <=4
            //    DeviceConf.TargetDynamicAddr = device[i].address;
            //    DeviceConf.IBIAck = __HAL_I3C_GET_IBI_CAPABLE(__HAL_I3C_GET_BCR(dev_payload[i]));
            //    DeviceConf.IBIPayload = __HAL_I3C_GET_IBI_PAYLOAD(__HAL_I3C_GET_BCR(dev_payload[i]));
            //    DeviceConf.CtrlRoleReqAck = __HAL_I3C_GET_CR_CAPABLE(__HAL_I3C_GET_BCR(dev_payload[i]));
            //    DeviceConf.CtrlStopTransfer = DISABLE;
            //    HAL_I3C_Ctrl_ConfigBusDevices(&hi3c2, &DeviceConf, 1); // TODO: check returned value

            // change bus property to work on i3c
            // device[i].bus_property |= PLATFORM_BUS_PROPERTY_I3C_IBI;
        }
    }
    return 0;
}

// uint64_t paylaod_dump[NB_DEVICES];
// uint64_t paylaod_dump_index = 0;

__attribute__((optimize("O0"))) int platform_assign_dynamic_address_multisensor() {
    // TODO: how to handle dynamic address assignment for multiple devices?

    HAL_StatusTypeDef status;
    uint64_t payload;
    I3C_ENTDAAPayloadTypeDef payload_info;
    uint8_t address;

    // set i3c bus frequency to 1 MHz before dynamic address assignment
    hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x7c;
    hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x7c;
    hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x7c;
    if (HAL_I3C_Init(&hi3c2) != HAL_OK) {
        return -1;
    }

    // NOTE: for the moment apply static address as dynamic address
    do {
        status = HAL_I3C_Ctrl_DynAddrAssign(&hi3c2, &payload, I3C_RSTDAA_THEN_ENTDAA, 5000);
        if (status == HAL_BUSY) {
            HAL_I3C_Get_ENTDAA_Payload_Info(&hi3c2, payload, &payload_info);
            //            paylaod_dump[paylaod_dump_index] = payload;
            //            paylaod_dump_index++;

            for (int i = 0; i < NB_DEVICES; i++) {
                if (device[i].instance_id == payload_info.PID.MIPIID) {
                    address = device[i].address;
                    // printf(" device %u (%u) address %X ", device[i].instance_id, device[i].instance_id & 0xf,
                    // address);
                    break;
                }
            }
            HAL_I3C_Ctrl_SetDynAddr(&hi3c2, address & 0x7F);
            // printf("payload info : %u, %u, %u, %u \n", payload_info.PID.MIPIID, payload_info.PID.IDTSEL,
            //        payload_info.PID.PartID, payload_info.PID.MIPIMID);
        }
    } while (status == HAL_BUSY);

    // set i3c bus frequency to 12 MHz after dynamic address assignement
    hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x0a;
    hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x09;
    hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x59;
    if (HAL_I3C_Init(&hi3c2) != HAL_OK) {
        return -1;
    }

    return 0;
}

int platform_ibi_enable() {
    HAL_StatusTypeDef status;
    status = HAL_I3C_ActivateNotification(&hi3c2, NULL, LL_I3C_IER_IBIIE);
    return (status == HAL_OK ? 0 : -1);
}

int platform_ibi_disable() {
    HAL_StatusTypeDef status;
    status = HAL_I3C_DeactivateNotification(&hi3c2, LL_I3C_IER_IBIIE);
    return (status == HAL_OK ? 0 : -1);
}

int platform_profiler_enable() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // enable counter
    return 0;
}

int platform_profiler_disable() {
    // TODO: clear CoreDebug_DEMCR_TRCENA_Msk in DEMCR register
    // TODO: disable counter
    return 0;
}

uint32_t platform_profiler_get_timestamp() {
    return DWT->CYCCNT;
}

uint32_t platform_profiler_convert_to_us(uint32_t timestamp) {
    uint32_t tick_per_1us = SystemCoreClock / 1000000;
    return timestamp / tick_per_1us;
}

int platform_ctrl_gpio(platform_gpio_t gpio, platform_gpio_state_t state) {
    switch (state) {
    case PLATFORM_GPIO_STATE_RESET:
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_RESET);
        break;
    case PLATFORM_GPIO_STATE_SET:
        HAL_GPIO_WritePin(gpio.port, gpio.pin, GPIO_PIN_SET);
        break;
    case PLATFORM_GPIO_STATE_TOGGLE:
        HAL_GPIO_TogglePin(gpio.port, gpio.pin);
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

/* HAL callbacks */

void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c, uint64_t targetPayload) {

    dev_payload[dev_count] = targetPayload;

    HAL_I3C_Ctrl_SetDynAddr(hi3c, device[dev_count++].address);
}

void HAL_I3C_CtrlDAACpltCallback(I3C_HandleTypeDef *hi3c) {
    platform_evt |= PLATFORM_I3C_DAA_EVT;
}

void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t eventId) {

    if ((eventId & EVENT_ID_IBI) == EVENT_ID_IBI) {
        I3C_CCCInfoTypeDef CCCInfo;
        if (HAL_I3C_GetCCCInfo(hi3c, EVENT_ID_IBI, &CCCInfo) != HAL_OK) {
            while (1)
                ; // error in handling event
        }
        if (CCCInfo.IBITgtPayload == 0) {
            platform_evt |= PLATFORM_I3C_IBI_EVT;
        }
    }
}

void HAL_I3C_CtrlRxCpltCallback(I3C_HandleTypeDef *hi3c) {
    platform_evt |= PLATFORM_I3C_DMA_RX_EVT;
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
    for (int i = 0; i < NB_DEVICES; i++) {
        if (GPIO_Pin == device[i].intr.pin) {
            platform_evt |= PLATFORM_GPIO_IT_EVT;
        }
    }
}
