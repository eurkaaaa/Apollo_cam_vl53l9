/*
 * vd55g1_driver.c
 *
 *  Created on: Jul 9, 2025
 *      Author: Lenovo
 */

#include <assert.h>
#include <stdlib.h>

#include "vd55g0.h"

#include "vd55g0_patch_cut_1.c"
#include "vd55g0_patch_cut_2.c"

#define CAMERA_ADDR 0x10

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

enum vd55g0_bin_mode {
  VD55G0_BIN_MODE_NORMAL,
  VD55G0_BIN_MODE_DIGITAL_X2,
  VD55G0_BIN_MODE_DIGITAL_X4,
};

enum {
  VD55G0_ST_IDLE,
  VD55G0_ST_STREAMING,
};

struct vd55g0_rect {
  int32_t left;
  int32_t top;
  uint32_t width;
  uint32_t height;
};

struct vd55g0_mode {
  uint32_t width;
  uint32_t height;
  enum vd55g0_bin_mode bin_mode;
  struct vd55g0_rect crop;
};

static const struct vd55g0_mode vd55g0_supported_modes[] = {
  {
    .width = VD55G0_MAX_WIDTH,
    .height = VD55G0_MAX_HEIGHT,
    .bin_mode = VD55G0_BIN_MODE_NORMAL,
    .crop = {
      .left = 0,
      .top = 0,
      .width = VD55G0_MAX_WIDTH,
      .height = VD55G0_MAX_HEIGHT,
    },
  },
  {
    .width = 640,
    .height = 560,
    .bin_mode = VD55G0_BIN_MODE_NORMAL,
    .crop = {
      .left = 2,
      .top = 52,
      .width = 640,
      .height = 560,
    },
  },
  {
    .width = 640,
    .height = 480,
    .bin_mode = VD55G0_BIN_MODE_NORMAL,
    .crop = {
      .left = 82,
      .top = 112,
      .width = 640,
      .height = 480,
    },
  },
  {
    .width = 320,
    .height = 240,
    .bin_mode = VD55G0_BIN_MODE_DIGITAL_X2,
    .crop = {
      .left = 82,
      .top = 112,
      .width = 640,
      .height = 480,
    },
  },
};


#define VD55G0_TraceError(_ctx_,_ret_) do { \
  if (_ret_) VD55G0_error(_ctx_, "Error on %s:%d : %d\n", __func__, __LINE__, _ret_); \
  if (_ret_) display_error(_ctx_); \
  if (_ret_) return _ret_; \
} while(0)

static const struct vd55g0_mode *VD55G0_Resolution2Mode(VD55G0_Res_t resolution)
{
  switch (resolution) {
  case VD55G0_RES_QVGA_320_240:
    return &vd55g0_supported_modes[3];
    break;
  case VD55G0_RES_VGA_640_480:
    return &vd55g0_supported_modes[2];
    break;
  case VD55G0_RES_640_560:
    return &vd55g0_supported_modes[1];
    break;
  case VD55G0_RES_FULL_644_604:
    return &vd55g0_supported_modes[0];
    break;
  default:
    return NULL;
  }
}

static void VD55G0_log_impl(VD55G0_Ctx_t *ctx, int lvl, const char *format, ...)
{
  va_list ap;

  if (!ctx->log)
    return ;

  va_start(ap, format);
  ctx->log(ctx, lvl, format, ap);
  va_end(ap);
}

#define VD55G0_dbg(_ctx_, _lvl_, _fmt_, ...) do { \
  VD55G0_log_impl(_ctx_, VD55G0_LVL_DBG(_lvl_), "VD55G0_DG%d-%d : " _fmt_, _lvl_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VD55G0_notice(_ctx_, _fmt_, ...) do { \
  VD55G0_log_impl(_ctx_, VD55G0_LVL_NOTICE, "VD55G0_NOT-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VD55G0_warn(_ctx_, _fmt_, ...) do { \
  VD55G0_log_impl(_ctx_, VD55G0_LVL_WARNING, "VD55G0_WRN-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)

#define VD55G0_error(_ctx_, _fmt_, ...) do { \
  VD55G0_log_impl(_ctx_, VD55G0_LVL_ERROR, "VD55G0_ERR-%d : " _fmt_, __LINE__, ##__VA_ARGS__); \
} while(0)


/* Camera mode definitions */
static void display_error(VD55G0_Ctx_t *ctx)
{
  uint16_t reg16;
  int ret;

  ret = ctx->read16(ctx, 0x0000, &reg16);
  assert(ret == 0);
  VD55G0_error(ctx, "ERROR_CODE : 0x%04x\n", reg16);
}

static int VD55G0_Copy8(VD55G0_Ctx_t *ctx, uint16_t dst, uint16_t src)
{
  uint8_t reg8;
  int ret;

  ret = ctx->read8(ctx, src, &reg8);
  VD55G0_TraceError(ctx, ret);
  ret = ctx->write8(ctx, dst, reg8);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_Copy16(VD55G0_Ctx_t *ctx, uint16_t dst, uint16_t src)
{
  uint16_t reg16;
  int ret;

  ret = ctx->read16(ctx, src, &reg16);
  VD55G0_TraceError(ctx, ret);
  ret = ctx->write16(ctx, dst, reg16);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_PollReg8(VD55G0_Ctx_t *ctx, uint16_t addr, uint8_t poll_val)
{
  const unsigned int loop_delay_ms = 10;
  const unsigned int timeout_ms = 500;
  int loop_nb = timeout_ms / loop_delay_ms;
  uint8_t val;
  int ret;

  while (--loop_nb) {
    ret = ctx->read8(ctx, addr, &val);
    if (ret < 0)
      return ret;
    if (val == poll_val)
      return 0;
    ctx->delay(ctx, loop_delay_ms);
  }

  VD55G0_dbg(ctx, 0, "current state %d\n", val);

  return -1;
}

static int VD55G0_IsStreaming(VD55G0_Ctx_t *ctx)
{
  uint8_t state;
  int ret;

  ret = ctx->read8(ctx, STATUS_SYSTEM_FSM, &state);
  if (ret)
    return ret;

  return state == STREAMING;
}

static int VD55G0_WaitState(VD55G0_Ctx_t *ctx, int state)
{
  int ret = VD55G0_PollReg8(ctx, STATUS_SYSTEM_FSM, state);

  if (ret)
    VD55G0_warn(ctx, "Unable to reach state %d\n", state);
  else
    VD55G0_dbg(ctx, 0, "reach state %d\n", state);

  return ret;
}

static int VD55G0_CheckModelId(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  uint32_t reg32;
  uint16_t reg16;
  int ret;

  ret = ctx->read32(ctx, STATUS_DEVICE_MODEL_ID, &reg32);
  VD55G0_TraceError(ctx, ret);
  VD55G0_dbg(ctx, 0, "model_id = 0x%04x\n", reg32);
  if (reg32 != VD55G0_MODEL_ID) {
    VD55G0_error(ctx, "Bad model id expected 0x%04x / got 0x%04x\n", VD55G0_MODEL_ID, reg32);
    return -1;
  }

  ret = ctx->read16(ctx, STATUS_DEVICE_REVISION, &reg16);
  VD55G0_TraceError(ctx, ret);
  VD55G0_dbg(ctx, 0, "revision = 0x%04x\n", reg16);
  switch (reg16) {
  case VD55G0_REVISION_CUT1:
    drv_ctx->cut_version = VD55G0_REVISION_CUT1;
    break;
  case VD55G0_REVISION_CUT2:
    drv_ctx->cut_version = VD55G0_REVISION_CUT2;
    break;
  default:
    VD55G0_error(ctx, "Unsupported revision0x%04x\n", reg16);
    return -1;
  }

  ret = ctx->read32(ctx, STATUS_ROM_REVISION, &reg32);
  VD55G0_TraceError(ctx, ret);
  VD55G0_dbg(ctx, 0, "rom = 0x%04x\n", reg32);

  return 0;
}

static int VD55G0_ApplyPatchCommon(VD55G0_Ctx_t *ctx, uint8_t *patch_array, int patch_len, uint8_t patch_major,
                                   uint8_t patch_minor)
{
  uint16_t reg16;
  int ret;

  ret = ctx->write_array(ctx, START_PATCH_ADDRESS, patch_array, patch_len);
  VD55G0_TraceError(ctx, ret);

  ret = ctx->write8(ctx, CMD_BOOT, VD55G0_BOOT_BOOT);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_PollReg8(ctx, CMD_BOOT, VD55G0_CMD_ACK);
  VD55G0_TraceError(ctx, ret);

  ret = ctx->read16(ctx, STATUS_FWPATCH_REVISION, &reg16);
  VD55G0_TraceError(ctx, ret);
  VD55G0_dbg(ctx, 0, "patch = 0x%04x\n", reg16);

  uint16_t test = (patch_major << 8) + patch_minor;

//  if (reg16 != test) {
//    VD55G0_error(ctx, "bad patch version expected %d.%d got %d.%d\n", patch_major, patch_minor, reg16 >> 8, reg16 & 0xff);
//    return -1;
//  }
  VD55G0_notice(ctx, "patch %d.%d applied\n", reg16 >> 8, reg16 & 0xff);

  ret = VD55G0_WaitState(ctx, SW_STBY);
  VD55G0_TraceError(ctx, ret);

  VD55G0_notice(ctx, "sensor boot successfully\n");

  return 0;
}

static int VD55G0_ApplyPatch(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;

  switch (drv_ctx->cut_version) {
  case VD55G0_REVISION_CUT1:
    return VD55G0_ApplyPatchCommon(ctx, (uint8_t *) patch_array_cut_1, sizeof(patch_array_cut_1),
                                   VD55G0_FWPATCH_REVISION_MAJOR_CUT_1, VD55G0_FWPATCH_REVISION_MINOR_CUT_1);
  case VD55G0_REVISION_CUT2:
    return VD55G0_ApplyPatchCommon(ctx, (uint8_t *) patch_array_cut_2, sizeof(patch_array_cut_2),
                                   VD55G0_FWPATCH_REVISION_MAJOR_CUT_2, VD55G0_FWPATCH_REVISION_MINOR_CUT_2);
  default:
    assert(0);
  }

  return -1;
}

static int VD55G0_Gpios(VD55G0_Ctx_t *ctx)
 {
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int ret;
  int i;

  for (i = 0 ; i < VD55G0_GPIO_NB; i++)
  {
    ret = ctx->write8(ctx, STREAM_CTX0_GPIO_x(i), drv_ctx->config_save.gpio_ctrl[i]);
    VD55G0_TraceError(ctx, ret);
  }

  return 0;
}

static int VD55G0_Boot(VD55G0_Ctx_t *ctx)
{
  int ret;

  ret = VD55G0_WaitState(ctx, READY_TO_BOOT);
  if (ret)
    return ret;

  ret = VD55G0_CheckModelId(ctx);
  if (ret)
    return ret;

  ret = VD55G0_ApplyPatch(ctx);
  if (ret)
    return ret;

  ret = VD55G0_Gpios(ctx);
  if (ret)
    return ret;

  return 0;
}

static uint32_t VD55G0_GetSystemClock(VD55G0_Ctx_t *ctx)
{
  uint32_t mipi_data_rate;
  int ret;

  ret = ctx->read32(ctx, SENSOR_MIPI_DATA_RATE, &mipi_data_rate);
  if (ret)
    return 0;

  if (mipi_data_rate <= 1200000000 && mipi_data_rate > 600000000)
    return mipi_data_rate;
  else if (mipi_data_rate <= 600000000 && mipi_data_rate > 300000000)
    return mipi_data_rate * 2;
  else if (mipi_data_rate <= 300000000 && mipi_data_rate >= 250000000)
    return mipi_data_rate * 4;

  return 0;
}

static uint32_t VD55G0_GetPixelClock(VD55G0_Ctx_t *ctx)
{
  uint32_t system_clk;

  system_clk = VD55G0_GetSystemClock(ctx);
  if (!system_clk)
    return 0;

  if (system_clk <= 1200000000 && system_clk > 900000000)
    return system_clk / 8;
  else if (system_clk <= 900000000 && system_clk > 780000000)
    return system_clk / 6;
  else if (system_clk <= 780000000 && system_clk >= 600000000)
    return system_clk / 5;

  return 0;
}

static int VD55G0_SetupClocks(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int ret;

  if (drv_ctx->config_save.out_itf.data_rate_in_mps < VD55G0_MIN_DATARATE ||
      drv_ctx->config_save.out_itf.data_rate_in_mps > VD55G0_MAX_DATARATE)
    return -1;

  ret = ctx->write32(ctx, SENSOR_EXT_CLOCK, drv_ctx->config_save.ext_clock_freq_in_hz);
  VD55G0_TraceError(ctx, ret);

  ret = ctx->write32(ctx, SENSOR_MIPI_DATA_RATE, drv_ctx->config_save.out_itf.data_rate_in_mps);
  VD55G0_TraceError(ctx, ret);

  drv_ctx->pclk = VD55G0_GetPixelClock(ctx);
  if (!drv_ctx->pclk)
    return -1;

  return 0;
}

static int VD55G0_SetupOutput(VD55G0_Ctx_t *ctx)
{
  VD55G0_OutItf_Config_t *out_itf = &ctx->ctx.config_save.out_itf;
  uint16_t oif_ctrl;
  int ret;

  /* Be sure we got value 0 or 1 */
  out_itf->clock_lane_swap_enable = !!out_itf->clock_lane_swap_enable;
  out_itf->data_lane_swap_enable = !!out_itf->data_lane_swap_enable;

  /* raw8 */
  ret = ctx->write8(ctx, STREAM_STATICS_FORMAT_CTRL, 8);
  VD55G0_TraceError(ctx, ret);

  /* csi lanes */
  oif_ctrl = out_itf->data_lane_swap_enable << 6 |
             out_itf->clock_lane_swap_enable << 3;
  ret = ctx->write16(ctx, STREAM_STATICS_OIF_CTRL, oif_ctrl);
  VD55G0_TraceError(ctx, ret);

  /* data type */
  ret = ctx->write8(ctx, STREAM_STATICS_OIF_IMG_CTRL, 0x2a);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_SetupSize(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  const struct vd55g0_mode *mode;
  int ret;

  mode = VD55G0_Resolution2Mode(drv_ctx->config_save.resolution);
  if (!mode)
    return -1;

  ret = ctx->write8(ctx, STREAM_CTX0_READOUT_CTRL, mode->bin_mode);
  VD55G0_TraceError(ctx, ret);

  ret = ctx->write16(ctx, STREAM_CTX0_OUT_ROI_X_START, mode->crop.left);
  VD55G0_TraceError(ctx, ret);
  ret = ctx->write16(ctx, STREAM_CTX0_OUT_ROI_X_END, mode->crop.width);
  VD55G0_TraceError(ctx, ret);
  ret = ctx->write16(ctx, STREAM_CTX0_OUT_ROI_Y_START, mode->crop.top);
  VD55G0_TraceError(ctx, ret);
  ret = ctx->write16(ctx, STREAM_CTX0_OUT_ROI_Y_END, mode->crop.height);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_SetupLineLen(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
//   int min_line_len_mipi;
  uint16_t line_len;
//   uint16_t width;
  int ret;

//   ret = ctx->read16(ctx, STREAM_CTX0_OUT_ROI_X_END, &width);
//   VD55G0_TraceError(ctx, ret);

//   min_line_len_mipi = ((width * VD55G0_MIPI_BPP + VD55G0_MIPI_MARGIN) * (uint64_t)drv_ctx->pclk)
//                       / VD55G0_MIPI_DATA_RATE_HZ;
//   line_len = MAX(VD55G0_MIN_LINE_LEN_ADC_10, min_line_len_mipi);
	//fix the line len to 1128
  line_len = VD55G0_MIN_LINE_LEN_ADC_10;

  ret = ctx->write16(ctx, STREAM_STATICS_LINE_LENGTH, line_len);
  VD55G0_TraceError(ctx, ret);
  VD55G0_dbg(ctx, 1, "line_length = %d\n", line_len);

  return 0;
}

static int VD55G0_ComputeFrameLength(VD55G0_Ctx_t *ctx, int fps, uint16_t *frame_length)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int min_frame_length;
  int req_frame_length;
  uint16_t line_length;
  uint16_t height;
  int ret;

  ret = ctx->read16(ctx, STREAM_STATICS_LINE_LENGTH, &line_length);
  VD55G0_TraceError(ctx, ret);

  ret = ctx->read16(ctx, STREAM_CTX0_OUT_ROI_Y_END, &height);
  VD55G0_TraceError(ctx, ret);

  min_frame_length = height + VD55G0_MIN_VBLANK;
  req_frame_length = drv_ctx->pclk / (line_length * fps);
  *frame_length = MIN(MAX(min_frame_length, req_frame_length), 65535);

  VD55G0_dbg(ctx, 1, "frame_length to MAX(%d, %d) = %d to reach %d fps\n", min_frame_length, req_frame_length,
             *frame_length, fps);

  return 0;
}

static int VD55G0_SetupFrameRate(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  uint16_t frame_length;
  int ret;

  ret = VD55G0_SetupLineLen(ctx);
  if (ret)
    return ret;

  ret = VD55G0_ComputeFrameLength(ctx, drv_ctx->config_save.frame_rate, &frame_length);
  if (ret)
    return ret;

  frame_length = 712;
  VD55G0_dbg(ctx, 1, "Set frame_length to %d to reach %d fps\n", frame_length, drv_ctx->config_save.frame_rate);
  ret = ctx->write16(ctx, STREAM_CTX0_FRAME_LENGTH, frame_length);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_SetupExposure(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  uint16_t frame_length;
  int max_fps;
  uint8_t reg;
  int ret;

  /* max integration lines */
   /* first get minimum frame len */
  max_fps = drv_ctx->config_save.frame_rate;

//   ret = VD55G0_ComputeFrameLength(ctx, max_fps, &frame_length);
//   if (ret)
//     return ret;
   /* set max integration lines to this value minus 10 lines */
//   ret = ctx->write16(ctx, VD55G0_REG_MAX_COARSE_INTEGRATION_LINES, frame_length - 10);
//   VD55G0_TraceError(ctx, ret);
//   VD55G0_dbg(ctx, 1, "Max coarse lines = %d\n", frame_length - 10);

  /* turn on auto exposure except when patgen is active */
  reg = drv_ctx->config_save.patgen = VD55G0_EXP_MODE_AUTO;
  ret = ctx->write8(ctx, STREAM_CTX0_EXPOSURE_MODE, reg);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_SetupMirrorFlip(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  uint8_t mode;
  int ret;

  switch (drv_ctx->config_save.flip_mirror_mode) {
  case VD55G0_MIRROR_FLIP_NONE:
    mode = 0;
    break;
  case VD55G0_FLIP:
    mode = 2;
    break;
  case VD55G0_MIRROR:
    mode = 1;
    break;
  case VD55G0_MIRROR_FLIP:
    mode = 3;
    break;
  default:
    return -1;
  }

  ret = ctx->write8(ctx, STREAM_STATICS_ORIENTATION, mode);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_SetupPatGen(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  uint16_t value = VD55G0_PATGEN_CTRL_DISABLE;
  int ret;

  switch (drv_ctx->config_save.patgen) {
  case VD55G0_PATGEN_DISABLE:
    value = VD55G0_PATGEN_CTRL_DISABLE;
    break;
  case VD55G0_PATGEN_DIAGONAL_GRAYSCALE:
    value = VD55G0_PATGEN_CTRL_DIAG_GRAY;
    break;
  case VD55G0_PATGEN_PSEUDO_RANDOM:
    value = VD55G0_PATGEN_CTRL_PSN;
    break;
  default:
    return -1;
  }

  if (drv_ctx->config_save.patgen != VD55G0_PATGEN_CTRL_DISABLE)
  {
    ret = ctx->write8(ctx, STREAM_STATICS_DUSTER_CTRL, VD55G0_DUSTER_DISABLE);
    VD55G0_TraceError(ctx, ret);
    ret = ctx->write8(ctx, STREAM_STATICS_DARKCAL_CTRL, VD55G0_DARKCAL_BYPASS_DARKAVG);
    VD55G0_TraceError(ctx, ret);
  }

  ret = ctx->write16(ctx, STREAM_DYNAMICS_PATGEN_CTRL, value);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_Setup(VD55G0_Ctx_t *ctx)
{
  int ret;

  ret = VD55G0_SetupClocks(ctx);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_SetupOutput(ctx);
  VD55G0_TraceError(ctx, ret);

//   ret = VD55G0_SetupSize(ctx);
//   VD55G0_TraceError(ctx, ret);

  ret = VD55G0_SetupFrameRate(ctx);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_SetupExposure(ctx);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_SetupMirrorFlip(ctx);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_SetupPatGen(ctx);
  VD55G0_TraceError(ctx, ret);

  return 0;
}

static int VD55G0_StartStreaming(VD55G0_Ctx_t *ctx)
{
  int ret;

  ret = ctx->write8(ctx, CMD_STBY, VD55G0_STBY_START_STREAM);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_PollReg8(ctx, CMD_STBY, VD55G0_CMD_ACK);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_WaitState(ctx, STREAMING);
  VD55G0_TraceError(ctx, ret);

  VD55G0_notice(ctx, "Streaming is on\n");

  return 0;
}

static int VD55G0_StopStreaming(VD55G0_Ctx_t *ctx)
{
  int ret;

  ret = ctx->write8(ctx, CMD_STREAMING, VD55G0_STREAM_STOP_STREAM);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_PollReg8(ctx, CMD_STREAMING, VD55G0_CMD_ACK);
  VD55G0_TraceError(ctx, ret);

  ret = VD55G0_WaitState(ctx, SW_STBY);
  VD55G0_TraceError(ctx, ret);

  VD55G0_notice(ctx, "Streaming is off\n");

  return 0;
}

int VD55G0_Init(VD55G0_Ctx_t *ctx, VD55G0_Config_t *config)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int ret;

  if (config->frame_rate < VD55G0_MIN_FPS)
    return -1;
  if (config->frame_rate > VD55G0_MAX_FPS)
    return -1;

  if ((config->resolution != VD55G0_RES_QVGA_320_240) &&
      (config->resolution != VD55G0_RES_VGA_640_480) &&
      (config->resolution != VD55G0_RES_640_560) &&
      (config->resolution != VD55G0_RES_FULL_644_604)) {
    return -1;
  }

  drv_ctx->config_save = *config;

  ctx->shutdown_pin(ctx, 0);
  ctx->delay(ctx, 10);
  ctx->shutdown_pin(ctx, 1);
  ctx->delay(ctx, 10);

  ret = VD55G0_Boot(ctx);
  if (ret)
    return ret;

  drv_ctx->state = VD55G0_ST_IDLE;

  return 0;
}

int VD55G0_DeInit(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;

  if (drv_ctx->state == VD55G0_ST_STREAMING)
    return -1;

  ctx->shutdown_pin(ctx, 0);
  ctx->delay(ctx, 10);

  return 0;
}

int VD55G0_Start(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int ret;

  ret = VD55G0_Setup(ctx);
  if (ret)
    return ret;

  ret = VD55G0_StartStreaming(ctx);
  if (ret)
    return ret;
  drv_ctx->state = VD55G0_ST_STREAMING;

  return 0;
}

int VD55G0_Stop(VD55G0_Ctx_t *ctx)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int ret;

  ret = VD55G0_StopStreaming(ctx);
  if (ret)
    return ret;
  drv_ctx->state = VD55G0_ST_IDLE;

  return 0;
}

int VD55G0_SetFlipMirrorMode(VD55G0_Ctx_t *ctx, VD55G0_MirrorFlip_t mode)
{
  struct drv_ctx_0 *drv_ctx = &ctx->ctx;
  int is_streaming;
  int ret;

  is_streaming = VD55G0_IsStreaming(ctx);
  if (is_streaming < 0)
    return is_streaming;

  if (is_streaming) {
    ret = VD55G0_Stop(ctx);
    if (ret)
      return ret;
  }

  drv_ctx->config_save.flip_mirror_mode = mode;

  if (is_streaming) {
    ret = VD55G0_Start(ctx);
    if (ret)
      return ret;
  }

  return 0;
}

//int VD55G0_GetSensorInfo(VD55G0_Ctx_t * pObj, VD55G0_Config_t * Info)
//{
//	if ((!pObj) || (Info == NULL))
//	{
//		return VD55G0_ERROR;
//	}
//	else
//	{
//		return VD55G0_ERROR;
//	}
//	Info->bayer_pattern = IMX335_BAYER_PATTERN;
//	Info->color_depth = IMX335_COLOR_DEPTH;
//	Info->width = IMX335_WIDTH;
//	Info->height = IMX335_HEIGHT;
//	Info->gain_min = IMX335_GAIN_MIN;
//	Info->gain_max = IMX335_GAIN_MAX;
//	Info->exposure_min = IMX335_EXPOSURE_MIN;
//	Info->exposure_max = IMX335_EXPOSURE_MAX;
//
//	return IMX335_OK;
//}
