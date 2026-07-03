#include "cmw_vd55g0.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "vd55g0.h"
#include "cmw_camera.h"

#define VD55G0_CHIP_ID 0x53354730

#define container_of(ptr, type, member) (type *) ((unsigned char *)ptr - offsetof(type,member))

#define MIN(a, b)       ((a) < (b) ?  (a) : (b))

#define VD55G0_REG_MODEL_ID                           0x0000

static int CMW_VD55G0_Read8(CMW_VD55G0_t *pObj, uint16_t addr, uint8_t *value)
{
  return pObj->ReadReg(pObj->Address, addr, value, 1);
}

static int CMW_VD55G0_Read16(CMW_VD55G0_t *pObj, uint16_t addr, uint16_t *value)
{
  uint8_t data[2];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 2);
  if (ret)
    return ret;

  *value = (data[1] << 8) | data[0];

  return CMW_ERROR_NONE;
}

static int CMW_VD55G0_Read32(CMW_VD55G0_t *pObj, uint16_t addr, uint32_t *value)
{
  uint8_t data[4];
  int ret;

  ret = pObj->ReadReg(pObj->Address, addr, data, 4);
  if (ret)
    return ret;

  *value = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];

  return 0;
}

static int CMW_VD55G0_Write8(CMW_VD55G0_t *pObj, uint16_t addr, uint8_t value)
{
  return pObj->WriteReg(pObj->Address, addr, &value, 1);
}

static int CMW_VD55G0_Write16(CMW_VD55G0_t *pObj, uint16_t addr, uint16_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 2);
}

static int CMW_VD55G0_Write32(CMW_VD55G0_t *pObj, uint16_t addr, uint32_t value)
{
  return pObj->WriteReg(pObj->Address, addr, (uint8_t *) &value, 4);
}

static void VD55G0_ShutdownPin(struct VD55G0_Ctx *ctx, int value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  p_ctx->ShutdownPin(value);
}

static int VD55G0_Read8(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t *value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Read8(p_ctx, addr, value);
}

static int VD55G0_Read16(struct VD55G0_Ctx *ctx, uint16_t addr, uint16_t *value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Read16(p_ctx, addr, value);
}

static int VD55G0_Read32(struct VD55G0_Ctx *ctx, uint16_t addr, uint32_t *value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Read32(p_ctx, addr, value);
}

static int VD55G0_Write8(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Write8(p_ctx, addr, value);
}

static int VD55G0_Write16(struct VD55G0_Ctx *ctx, uint16_t addr, uint16_t value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Write16(p_ctx, addr, value);
}

static int VD55G0_Write32(struct VD55G0_Ctx *ctx, uint16_t addr, uint32_t value)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  return CMW_VD55G0_Write32(p_ctx, addr, value);
}

static int VD55G0_WriteArray(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t *data, int data_len)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);
  const unsigned int chunk_size = 128;
  uint16_t sz;
  int ret;

  while (data_len) {
    sz = MIN(data_len, chunk_size);
    ret = p_ctx->WriteReg(p_ctx->Address, addr, data, sz);
    if (ret)
      return ret;
    data_len -= sz;
    addr += sz;
    data += sz;
  }

  return 0;
}

static void VD55G0_Delay(struct VD55G0_Ctx *ctx, uint32_t delay_in_ms)
{
  CMW_VD55G0_t *p_ctx = container_of(ctx, CMW_VD55G0_t, ctx_driver);

  p_ctx->Delay(delay_in_ms);
}

static void VD55G0_Log(struct VD55G0_Ctx *ctx, int lvl, const char *format, va_list ap)
{
#if 0
  const int current_lvl = VD55G0_LVL_DBG(0);

  if (lvl > current_lvl)
    return ;

  vprintf(format, ap);
#endif
}

/**
  * @brief  Get the sensor info
  * @param  pObj  pointer to component object
  * @param  pInfo pointer to sensor info structure
  * @retval Component status
  */
static int32_t CMW_VD55G0_GetSensorInfo(void *io_ctx, ISP_SensorInfoTypeDef *info)
{
  if ((io_ctx ==  NULL) || (info == NULL))
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  /* Return the default full resolution */
  info->width = VD55G0_MAX_WIDTH;
  info->height = VD55G0_MAX_HEIGHT;

  return CMW_ERROR_NONE;
}

static int CMW_VD55G0_GetResType(uint32_t width, uint32_t height, VD55G0_Res_t *res)
{
  if (width == 320 && height == 240)
  {
    *res = VD55G0_RES_QVGA_320_240;
  }
  else if (width == 640 && height == 480)
  {
    *res = VD55G0_RES_VGA_640_480;
  }
  else if (width == 640 && height == 560)
  {
    *res = VD55G0_RES_640_560;
  }
  else if (width == 644 && height == 604)
  {
      *res = VD55G0_RES_FULL_644_604;
  }
  else
  {
    return CMW_ERROR_WRONG_PARAM;
  }
  return 0;
}

static VD55G0_MirrorFlip_t CMW_VD55G0_getMirrorFlipConfig(int32_t Config)
{
  VD55G0_MirrorFlip_t ret;

  switch (Config)
  {
    case CMW_MIRRORFLIP_NONE:
      ret = VD55G0_MIRROR_FLIP_NONE;
      break;
    case CMW_MIRRORFLIP_FLIP:
      ret = VD55G0_FLIP;
      break;
    case CMW_MIRRORFLIP_MIRROR:
      ret = VD55G0_MIRROR;
      break;
    case CMW_MIRRORFLIP_FLIP_MIRROR:
    default:
      ret = VD55G0_MIRROR_FLIP;
      break;
  }

  return ret;
}

static int32_t CMW_VD55G0_Init(void *io_ctx, CMW_Sensor_Init_t *initSensor)
{
  VD55G0_Config_t config = { 0 };
  int ret;
  int i;

  if (((CMW_VD55G0_t *)io_ctx)->IsInitialized)
  {
    return CMW_ERROR_NONE;
  }

  config.frame_rate = initSensor->fps;
  ret = CMW_VD55G0_GetResType(initSensor->width, initSensor->height, &config.resolution);
  if (ret)
  {
    return CMW_ERROR_WRONG_PARAM;
  }

  config.ext_clock_freq_in_hz = ((CMW_VD55G0_t *)io_ctx)->ClockInHz;
  config.flip_mirror_mode = CMW_VD55G0_getMirrorFlipConfig(initSensor->mirrorFlip);
  config.patgen = VD55G0_PATGEN_DISABLE;
  config.out_itf.data_rate_in_mps = VD55G0_MAX_DATARATE;
  config.out_itf.clock_lane_swap_enable = 1;
  config.out_itf.data_lane_swap_enable = 1;
  //55g0没有awu
  // config.awu.is_enable = 0;
  for (i = 0; i < VD55G0_GPIO_NB; i++)
  {
    config.gpio_ctrl[i] = VD55G0_GPIO_GPIO_IN;
  }

  ret = VD55G0_Init(&((CMW_VD55G0_t *)io_ctx)->ctx_driver, &config);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ((CMW_VD55G0_t *)io_ctx)->IsInitialized = 1;
  return CMW_ERROR_NONE;
}

static int32_t CMW_VD55G0_DeInit(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VD55G0_Stop(&((CMW_VD55G0_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ret = VD55G0_DeInit(&((CMW_VD55G0_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }

  ((CMW_VD55G0_t *)io_ctx)->IsInitialized = 0;
  return CMW_ERROR_NONE;
}


static int32_t CMW_VD55G0_MirrorFlipConfig(void *io_ctx, uint32_t Config)
{
  int32_t ret = CMW_ERROR_NONE;

  switch (Config) {
    case CMW_MIRRORFLIP_NONE:
      ret = VD55G0_SetFlipMirrorMode(&((CMW_VD55G0_t *)io_ctx)->ctx_driver, VD55G0_MIRROR_FLIP_NONE);
      break;
    case CMW_MIRRORFLIP_FLIP:
      ret = VD55G0_SetFlipMirrorMode(&((CMW_VD55G0_t *)io_ctx)->ctx_driver, VD55G0_FLIP);
      break;
    case CMW_MIRRORFLIP_MIRROR:
      ret = VD55G0_SetFlipMirrorMode(&((CMW_VD55G0_t *)io_ctx)->ctx_driver, VD55G0_MIRROR);
      break;
    case CMW_MIRRORFLIP_FLIP_MIRROR:
      ret = VD55G0_SetFlipMirrorMode(&((CMW_VD55G0_t *)io_ctx)->ctx_driver, VD55G0_MIRROR_FLIP);
      break;
    default:
      ret = CMW_ERROR_PERIPH_FAILURE;
  }

  return ret;
}

static int32_t CMW_VD55G0_Start(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;
  ret = VD55G0_Start(&((CMW_VD55G0_t *)io_ctx)->ctx_driver);
  if (ret) {
    VD55G0_DeInit(&((CMW_VD55G0_t *)io_ctx)->ctx_driver);
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}

static int32_t CMW_VD55G0_Stop(void *io_ctx)
{
  int ret = CMW_ERROR_NONE;

  ret = VD55G0_Stop(&((CMW_VD55G0_t *)io_ctx)->ctx_driver);
  if (ret)
  {
    return CMW_ERROR_PERIPH_FAILURE;
  }
  return CMW_ERROR_NONE;
}

static int32_t VD55G0_RegisterBusIO(CMW_VD55G0_t *io_ctx)
{
  int ret;

  if (!io_ctx)
    return CMW_ERROR_COMPONENT_FAILURE;

  if (!io_ctx->Init)
    return CMW_ERROR_COMPONENT_FAILURE;

  ret = io_ctx->Init();

  return ret;
}

static int32_t VD55G0_ReadID(CMW_VD55G0_t *io_ctx, uint32_t *Id)
{
  uint32_t reg32;
  int32_t ret;

  ret = CMW_VD55G0_Read32(io_ctx, VD55G0_REG_MODEL_ID, &reg32);
  if (ret)
    return ret;

  *Id = reg32;

  return CMW_ERROR_NONE;
}

static void CMW_VD55G0_PowerOn(CMW_VD55G0_t *io_ctx)
{
  /* Camera sensor Power-On sequence */
  /* Assert the camera  NRST pins */
  io_ctx->ShutdownPin(0);  /* Disable MB1723 2V8 signal  */
  io_ctx->Delay(200); /* NRST signals asserted during 200ms */
  /* De-assert the camera STANDBY pin (active high) */
  io_ctx->ShutdownPin(1);  /* Disable MB1723 2V8 signal  */
  io_ctx->Delay(20); /* NRST de-asserted during 20ms */
}

int CMW_VD55G0_Probe(CMW_VD55G0_t *io_ctx, CMW_Sensor_if_t *vd55g0_if)
{
  int ret = CMW_ERROR_NONE;
  uint32_t id;

  io_ctx->ctx_driver.shutdown_pin = VD55G0_ShutdownPin;
  io_ctx->ctx_driver.read8 = VD55G0_Read8;
  io_ctx->ctx_driver.read16 = VD55G0_Read16;
  io_ctx->ctx_driver.read32 = VD55G0_Read32;
  io_ctx->ctx_driver.write8 = VD55G0_Write8;
  io_ctx->ctx_driver.write16 = VD55G0_Write16;
  io_ctx->ctx_driver.write32 = VD55G0_Write32;
  io_ctx->ctx_driver.write_array = VD55G0_WriteArray;
  io_ctx->ctx_driver.delay = VD55G0_Delay;
  io_ctx->ctx_driver.log = VD55G0_Log;

  CMW_VD55G0_PowerOn(io_ctx);

  ret = VD55G0_RegisterBusIO(io_ctx);
  if (ret != CMW_ERROR_NONE)
  {
    return CMW_ERROR_COMPONENT_FAILURE;
  }

 ret = VD55G0_ReadID(io_ctx, &id);
 if (ret != CMW_ERROR_NONE)
 {
   return CMW_ERROR_COMPONENT_FAILURE;
 }
 if (id != VD55G0_CHIP_ID)
 {
   return CMW_ERROR_COMPONENT_FAILURE;
 }

  memset(vd55g0_if, 0, sizeof(*vd55g0_if));
  vd55g0_if->Init = CMW_VD55G0_Init;
  vd55g0_if->DeInit = CMW_VD55G0_DeInit;
  vd55g0_if->Start = CMW_VD55G0_Start;
  vd55g0_if->Stop = CMW_VD55G0_Stop;
  vd55g0_if->SetMirrorFlip = CMW_VD55G0_MirrorFlipConfig;
  vd55g0_if->GetSensorInfo = CMW_VD55G0_GetSensorInfo;
  return ret;
}
