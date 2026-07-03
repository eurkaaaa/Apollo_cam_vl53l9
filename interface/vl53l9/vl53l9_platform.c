#include "vl53l9_platform.h"
#include "interface.h"
#include "stm32n6xx_hal.h"
#include "stm32n6xx_hal_i3c.h"
#include "vl53l9.h"
#include <string.h>

#define VL53L9_STATIC_BUF_SIZE  8194
static uint8_t s_i3c_write_buf[VL53L9_STATIC_BUF_SIZE];

static uint8_t _i3c_read(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor, I3C_XferTypeDef *aContextBuffers);
static uint8_t _i3c_read_async(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor,
                               I3C_XferTypeDef *aContextBuffers);
static uint8_t _i3c_write(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor, I3C_XferTypeDef *aContextBuffers);

int vl53l9_read(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_values == NULL || size == 0) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Read(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, p_values, size,
                             HAL_MAX_DELAY) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
        return ret;
    }
    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[2];
        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;

        uint32_t cb[2];
        uint32_t sb[2];
        I3C_PrivateTypeDef pd[2] = { { p_device->address, { data_write, 2 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
                                     { p_device->address, { NULL, 0 }, { p_values, size }, HAL_I3C_DIRECTION_READ } };
        I3C_XferTypeDef ctxtb[2] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 2 }, { NULL, 0 } },
                                     { { &cb[1], 1 }, { &sb[1], 1 }, { NULL, 0 }, { p_values, size } } };

        ret = _i3c_read(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_read_async(void *const p_dev, uint16_t address, volatile uint8_t *p_values, uint32_t size) {

    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_values == NULL || size == 0) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        return VL53L9_ERROR_PLATFORM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[2];
        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;

        uint32_t cb[2];
        uint32_t sb[2];
        I3C_PrivateTypeDef pd[2] = {
            { p_device->address, { data_write, 2 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
            { p_device->address, { NULL, 0 }, { (uint8_t *)p_values, size }, HAL_I3C_DIRECTION_READ }
        };
        I3C_XferTypeDef ctxtb[2] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 2 }, { NULL, 0 } },
                                     { { &cb[1], 1 }, { &sb[1], 1 }, { NULL, 0 }, { (uint8_t *)p_values, size } } };

        ret = _i3c_read_async(p_device, pd, ctxtb);
        return ret;
    }

    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_read8(void *const p_dev, uint16_t address, uint8_t *p_value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_value == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Read(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, p_value,
                             sizeof(*p_value), HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[2];
        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;

        uint32_t cb[2];
        uint32_t sb[2];

        I3C_PrivateTypeDef pd[2] = { { p_device->address, { data_write, 2 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
                                     { p_device->address, { NULL, 0 }, { p_value, 1 }, HAL_I3C_DIRECTION_READ } };
        I3C_XferTypeDef ctxtb[2] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 2 }, { NULL, 0 } },
                                     { { &cb[1], 1 }, { &sb[1], 1 }, { NULL, 0 }, { p_value, 1 } } };

        ret = _i3c_read(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_read16(void *const p_dev, uint16_t address, uint16_t *p_value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_value == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Read(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)p_value,
                             sizeof(*p_value), HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[2];
        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;

        uint32_t cb[2];
        uint32_t sb[2];

        I3C_PrivateTypeDef pd[2] = {
            { p_device->address, { data_write, 2 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
            { p_device->address, { NULL, 0 }, { (uint8_t *)p_value, 2 }, HAL_I3C_DIRECTION_READ }
        };
        I3C_XferTypeDef ctxtb[2] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 2 }, { NULL, 0 } },
                                     { { &cb[1], 1 }, { &sb[1], 1 }, { NULL, 0 }, { (uint8_t *)p_value, 2 } } };

        ret = _i3c_read(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_read32(void *const p_dev, uint16_t address, uint32_t *p_value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_value == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Read(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)p_value,
                             sizeof(*p_value), HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[2];
        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;

        uint32_t cb[2];
        uint32_t sb[2];

        I3C_PrivateTypeDef pd[2] = {
            { p_device->address, { data_write, 2 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
            { p_device->address, { NULL, 0 }, { (uint8_t *)p_value, 4 }, HAL_I3C_DIRECTION_READ }
        };
        I3C_XferTypeDef ctxtb[2] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 2 }, { NULL, 0 } },
                                     { { &cb[1], 1 }, { &sb[1], 1 }, { NULL, 0 }, { (uint8_t *)p_value, 4 } } };

        ret = _i3c_read(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

//int vl53l9_write(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size) {
//    uint8_t ret = VL53L9_ERROR_NONE;
//    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
////    uint8_t data_write[2 + size];
//
//    if (p_device == NULL || p_values == NULL || size == 0) {
//        return VL53L9_ERROR_INVALID_PARAM;
//    }
//
//    if (p_device->bus_type & PLATFORM_BUS_I2C) {
//        if (HAL_I2C_Mem_Write(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, p_values, size,
//                              HAL_MAX_DELAY) != HAL_OK) {
//            return VL53L9_ERROR_PLATFORM;
//        }
//        return VL53L9_ERROR_NONE;
//    }
//
//    if (p_device->bus_type & PLATFORM_BUS_I3C) {
//    	s_i3c_write_buf[0] = (address >> 8) & 0xFF;
//    	s_i3c_write_buf[1] = address & 0xFF;
//    	memcpy(&s_i3c_write_buf[2], p_values, size);
////        data_write[0] = (address >> 8) & 0xFF;
////        data_write[1] = address & 0xFF;
////        memcpy(&data_write[2], p_values, size);
//
//        uint32_t cb[1];
//        uint32_t sb[1];
//        I3C_PrivateTypeDef pd[1] = {
//            { p_device->address, { s_i3c_write_buf, sizeof(s_i3c_write_buf) }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
//        };
//        I3C_XferTypeDef ctxtb[1] = {
//            { { &cb[0], 1 }, { &sb[0], 1 }, { s_i3c_write_buf, sizeof(s_i3c_write_buf) }, { NULL, 0 } }
//        };
////        I3C_PrivateTypeDef pd[1] = {
////            { p_device->address, { data_write, sizeof(data_write) }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
////        };
////        I3C_XferTypeDef ctxtb[1] = {
////            { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, sizeof(data_write) }, { NULL, 0 } }
////        };
//
//        ret = _i3c_write(p_device, pd, ctxtb);
//        return ret;
//    }
//    return VL53L9_ERROR_INVALID_STATE;
//}
int vl53l9_write(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size)
{
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL || p_values == NULL || size == 0) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    /* ================= 分包配置 ================= */
    #define CHUNK_SIZE  64U  // I3C 可酌情调大到 128/256
    uint32_t offset = 0;

    /* ================= I2C 路径（保留原逻辑+分包） ================= */
    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        while (offset < size) {
            uint32_t chunk = (size - offset > CHUNK_SIZE) ? CHUNK_SIZE : (size - offset);
            if (HAL_I2C_Mem_Write(p_device->bus, p_device->address,
                                  address + offset, I2C_MEMADD_SIZE_16BIT,
                                  p_values + offset, chunk, HAL_MAX_DELAY) != HAL_OK) {
                return VL53L9_ERROR_PLATFORM;
            }
            offset += chunk;
        }
        return VL53L9_ERROR_NONE;
    }

    /* ================= I3C 路径（实际使用 + 分包+验证） ================= */
    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t verify_buf[4];  // 回读验证缓冲区

        while (offset < size) {
            uint32_t chunk = (size - offset > CHUNK_SIZE) ? CHUNK_SIZE : (size - offset);
            uint16_t cur_addr = address + offset;  // VL53L9 支持地址自动递增

            /* ---- 1. 执行写入 ---- */
            s_i3c_write_buf[0] = (cur_addr >> 8) & 0xFF;
            s_i3c_write_buf[1] = cur_addr & 0xFF;
            memcpy(&s_i3c_write_buf[2], p_values + offset, chunk);

            uint32_t cb[1], sb[1];
            I3C_PrivateTypeDef pd_write[1] = {
                { p_device->address, { s_i3c_write_buf, 2 + chunk }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
            };
            I3C_XferTypeDef ctxtb_write[1] = {
                { { &cb[0], 1 }, { &sb[0], 1 }, { s_i3c_write_buf, 2 + chunk }, { NULL, 0 } }
            };

            ret = _i3c_write(p_device, pd_write, ctxtb_write);
            if (ret != VL53L9_ERROR_NONE) {
                return ret;
            }

            /* ---- 2. 回读验证（纯 I3C 实现） ---- */
//            #if 1  // 调试开启，量产可关闭
//            {
//                // 构造读命令：先写地址指针，再读数据
//                uint8_t read_addr_buf[2] = { (cur_addr >> 8) & 0xFF, cur_addr & 0xFF };
//
//                I3C_PrivateTypeDef pd_read[1] = {
//                    // 先发送2字节地址（写方向），然后切换为读方向接收数据
//                    { p_device->address, { read_addr_buf, 2 }, { verify_buf, 4 }, HAL_I3C_DIRECTION_READ },
//                };
//                I3C_XferTypeDef ctxtb_read[1] = {
//                    { { &cb[0], 1 }, { &sb[0], 1 }, { read_addr_buf, 2 }, { verify_buf, 4 } }
//                };
//
//                ret = _i3c_read(p_device, pd_read, ctxtb_read);
//
//                // 对比验证：只校验头部4字节，平衡速度与可靠性
//                if (ret == VL53L9_ERROR_NONE && memcmp(verify_buf, p_values + offset, 4) != 0) {
//                    return VL53L9_ERROR_PLATFORM;  // 验证失败，数据不一致
//                }
//                // 如果读失败，可选择忽略继续（避免偶发干扰导致卡死），或返回错误
//            }
//            #endif

            offset += chunk;
        }
        return VL53L9_ERROR_NONE;
    }

    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_write8(void *const p_dev, uint16_t address, uint8_t value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Write(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, &value, sizeof(value),
                              HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[3];

        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;
        data_write[2] = value & 0xFF;

        uint32_t cb[1];
        uint32_t sb[1];
        I3C_PrivateTypeDef pd[1] = {
            { p_device->address, { data_write, 3 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
        };
        I3C_XferTypeDef ctxtb[1] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 3 }, { NULL, 0 } } };
        ret = _i3c_write(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_write16(void *const p_dev, uint16_t address, uint16_t value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Write(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&value,
                              sizeof(value), HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[4];

        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;
        data_write[2] = (value >> 0) & 0xFF;
        data_write[3] = (value >> 8) & 0xFF;

        uint32_t cb[1];
        uint32_t sb[1];
        I3C_PrivateTypeDef pd[1] = {
            { p_device->address, { data_write, 4 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
        };
        I3C_XferTypeDef ctxtb[1] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 4 }, { NULL, 0 } } };
        ret = _i3c_write(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

int vl53l9_write32(void *const p_dev, uint16_t address, uint32_t value) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    if (p_device == NULL) {
        return VL53L9_ERROR_INVALID_PARAM;
    }

    if (p_device->bus_type & PLATFORM_BUS_I2C) {
        if (HAL_I2C_Mem_Write(p_device->bus, p_device->address, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&value,
                              sizeof(value), HAL_MAX_DELAY) != HAL_OK) {
            return VL53L9_ERROR_PLATFORM;
        }
        return VL53L9_ERROR_NONE;
    }

    if (p_device->bus_type & PLATFORM_BUS_I3C) {
        uint8_t data_write[6];

        data_write[0] = (address >> 8) & 0xFF;
        data_write[1] = address & 0xFF;
        data_write[2] = (value >> 0) & 0xFF;
        data_write[3] = (value >> 8) & 0xFF;
        data_write[4] = (value >> 16) & 0xFF;
        data_write[5] = (value >> 24) & 0xFF;

        uint32_t cb[1];
        uint32_t sb[1];
        I3C_PrivateTypeDef pd[1] = {
            { p_device->address, { data_write, 6 }, { NULL, 0 }, HAL_I3C_DIRECTION_WRITE },
        };
        I3C_XferTypeDef ctxtb[1] = { { { &cb[0], 1 }, { &sb[0], 1 }, { data_write, 6 }, { NULL, 0 } } };
        ret = _i3c_write(p_device, pd, ctxtb);
        return ret;
    }
    return VL53L9_ERROR_INVALID_STATE;
}

__attribute__((optimize(0))) static uint8_t _i3c_read(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor,
                                                      I3C_XferTypeDef *aContextBuffers) {

    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    I3C_HandleTypeDef *p_hi3c = (I3C_HandleTypeDef *)p_device->bus;

    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_AddDescToFrame(p_hi3c, NULL, &aPrivateDescriptor[0], &aContextBuffers[0],
                                   aContextBuffers[0].CtrlBuf.Size, I3C_PRIVATE_WITHOUT_ARB_RESTART) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }

    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_Ctrl_Transmit(p_hi3c, &aContextBuffers[0], 100) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
        while ((HAL_I3C_GetState(p_hi3c) != HAL_I3C_STATE_READY) &&
               (HAL_I3C_GetState(p_hi3c) != HAL_I3C_STATE_LISTEN)) {
        }
    }
    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_AddDescToFrame(p_hi3c, NULL, &aPrivateDescriptor[1], &aContextBuffers[1],
                                   aContextBuffers[1].CtrlBuf.Size, I3C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret == VL53L9_ERROR_NONE) {
        if ((HAL_I3C_Ctrl_Receive(p_hi3c, &aContextBuffers[1], 100)) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret != VL53L9_ERROR_NONE) {
        ret = VL53L9_ERROR_INVALID_OPERATION;
    }

    return ret;
}

__attribute__((optimize(0))) static uint8_t _i3c_read_async(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor,
                                                            I3C_XferTypeDef *aContextBuffers) {

    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    I3C_HandleTypeDef *p_hi3c = (I3C_HandleTypeDef *)p_device->bus;

    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_AddDescToFrame(p_hi3c, NULL, &aPrivateDescriptor[0], &aContextBuffers[0],
                                   aContextBuffers[0].CtrlBuf.Size, I3C_PRIVATE_WITHOUT_ARB_RESTART) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }

    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_Ctrl_Transmit(p_hi3c, &aContextBuffers[0], 100) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
        while ((HAL_I3C_GetState(p_hi3c) != HAL_I3C_STATE_READY) &&
               (HAL_I3C_GetState(p_hi3c) != HAL_I3C_STATE_LISTEN)) {
        }
    }
    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_AddDescToFrame(p_hi3c, NULL, &aPrivateDescriptor[1], &aContextBuffers[1],
                                   aContextBuffers[1].CtrlBuf.Size, I3C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret == VL53L9_ERROR_NONE) {
        if ((HAL_I3C_Ctrl_Receive_DMA(p_hi3c, &aContextBuffers[1])) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret != VL53L9_ERROR_NONE) {
        ret = VL53L9_ERROR_INVALID_OPERATION;
    }

    return ret;
}

__attribute__((optimize(0))) static uint8_t _i3c_write(void *const p_dev, I3C_PrivateTypeDef *aPrivateDescriptor,
                                                       I3C_XferTypeDef *aContextBuffers) {

    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;

    I3C_HandleTypeDef *p_hi3c = (I3C_HandleTypeDef *)p_device->bus;

    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_AddDescToFrame(p_hi3c, NULL, &aPrivateDescriptor[0], &aContextBuffers[0],
                                   aContextBuffers[0].CtrlBuf.Size, I3C_PRIVATE_WITHOUT_ARB_STOP) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret == VL53L9_ERROR_NONE) {
        if (HAL_I3C_Ctrl_Transmit(p_hi3c, &aContextBuffers[0], 100) != HAL_OK) {
            ret = VL53L9_ERROR_INTERNAL;
        }
    }
    if (ret != VL53L9_ERROR_NONE) {
        ret = VL53L9_ERROR_INVALID_OPERATION;
    }
    return ret;
}

int vl53l9_wait_ms(void *const p_dev, uint32_t delay_ms) {
    (void)p_dev;
    HAL_Delay(delay_ms);
    return 0;
}

int vl53l9_get_config_vddio(void *const p_dev, vl53l9_vddio_t *voltage) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
    *voltage = p_device->vddio;
    return ret;
}
int vl53l9_get_config_vdda(void *const p_dev, vl53l9_vdda_t *voltage) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
    *voltage = p_device->vdda;
    return ret;
}

int vl53l9_get_config_ext_clock(void *const p_dev, uint32_t *ext_clock) {
    uint8_t ret = VL53L9_ERROR_NONE;
    vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
    *ext_clock = p_device->ext_clock;
    return ret;
}
