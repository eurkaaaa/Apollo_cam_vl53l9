/*
 * vd55g1_driver.h
 *
 *  Created on: Jul 9, 2025
 *      Author: Lenovo
 */
#ifndef VD55G0_H
#define VD55G0_H

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

enum {
  VD55G0_MIN_FPS = 2,
  VD55G0_MAX_FPS = 168,
};

typedef enum {
  VD55G0_RES_QVGA_320_240,
  VD55G0_RES_VGA_640_480,
  VD55G0_RES_640_560,
  VD55G0_RES_FULL_644_604,
} VD55G0_Res_t;

typedef enum {
  VD55G0_MIRROR_FLIP_NONE,
  VD55G0_FLIP,
  VD55G0_MIRROR,
  VD55G0_MIRROR_FLIP
} VD55G0_MirrorFlip_t;

typedef enum {
  VD55G0_PATGEN_DISABLE,
  VD55G0_PATGEN_DIAGONAL_GRAYSCALE,
  VD55G0_PATGEN_PSEUDO_RANDOM,
} VD55G0_PatGen_t;

typedef enum {
  VD55G0_GPIO_GPIO_IN = 1,
  VD55G0_GPIO_STROBE = 2,
  VD55G0_GPIO_PWM_STROBE = 3,
  VD55G0_GPIO_PWM = 4,
  VD55G0_GPIO_OUT = 5,
  VD55G0_GPIO_VSYNC_OUT_MODE0 = 6,
  VD55G0_GPIO_VSYNC_OUT_MODE1 = 7,
  VD55G0_GPIO_VSYNC_OUT_MODE2 = 8,
  VD55G0_GPIO_VT_SLAVE_MODE = 0xa,
} VD55G0_GPIO_Mode_t;

typedef enum {
  VD55G0_GPIO_LOW = (0 << 4),
  VD55G0_GPIO_HIGH = (1 << 4),
} VD55G0_GPIO_Value_t;

typedef enum {
  VD55G0_GPIO_NO_INVERSION = (0 << 5),
  VD55G0_GPIO_INVERTED = (1 << 5),
} VD55G0_GPIO_Polarity_t;

typedef enum {
  VD55G0_GPIO_0,
  VD55G0_GPIO_1,
  VD55G0_GPIO_2,
  VD55G0_GPIO_3,
  VD55G0_GPIO_NB
} VD55G0_GPIO_t;

enum {
	VD55G0_CUSTOM_DATARATE = 100000000,
  VD55G0_MIN_DATARATE = 250000000,
  VD55G0_DEFAULT_DATARATE = 804000000,
  VD55G0_MAX_DATARATE = 1200000000,
};

/* Output interface configuration */
typedef struct {
  int data_rate_in_mps;
  int clock_lane_swap_enable;
  int data_lane_swap_enable;
} VD55G0_OutItf_Config_t;

/* VD55G0 configuration */
typedef struct {
  int ext_clock_freq_in_hz;
  VD55G0_Res_t resolution;
  int frame_rate;
  VD55G0_MirrorFlip_t flip_mirror_mode;
  VD55G0_PatGen_t patgen;
  VD55G0_OutItf_Config_t out_itf;
  /* VD55G0_GPIO_Mode_t | VD55G0_GPIO_Value_t | VD55G0_GPIO_Polarity_t */
  uint8_t gpio_ctrl[VD55G0_GPIO_NB];
} VD55G0_Config_t;

typedef struct VD55G0_Ctx
{
  /* API client must set these values */
  void (*shutdown_pin)(struct VD55G0_Ctx *ctx, int value);
  int (*read8)(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t *value);
  int (*read16)(struct VD55G0_Ctx *ctx, uint16_t addr, uint16_t *value);
  int (*read32)(struct VD55G0_Ctx *ctx, uint16_t addr, uint32_t *value);
  int (*write8)(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t value);
  int (*write16)(struct VD55G0_Ctx *ctx, uint16_t addr, uint16_t value);
  int (*write32)(struct VD55G0_Ctx *ctx, uint16_t addr, uint32_t value);
  int (*write_array)(struct VD55G0_Ctx *ctx, uint16_t addr, uint8_t *data, int data_len);
  void (*delay)(struct VD55G0_Ctx *ctx, uint32_t delay_in_ms);
  void (*log)(struct VD55G0_Ctx *ctx, int lvl, const char *format, va_list ap);
  /* driver internals. do not touch */
  struct drv_ctx_0 {
    int state;
    int cut_version;
    uint32_t pclk;
    VD55G0_Config_t config_save;
  } ctx;
} VD55G0_Ctx_t;


// Register from UM3224
//1. Status register
#define STATUS_DEVICE_MODEL_ID		 0x0000
	#define VD55G0_MODEL_ID          0x53354730

#define STATUS_DEVICE_REVISION		 0x0004
	#define VD55G0_REVISION_CUT1	 0x1111
	#define VD55G0_REVISION_CUT2	 0x1120

#define STATUS_ROM_REVISION	 0x0018

#define STATUS_FWPATCH_REVISION		 0x0022

#define STATUS_SYSTEM_FSM            0x002C
	#define HW_STBY                  0x00
	#define READY_TO_BOOT            0x01
	#define SW_STBY                  0x02
	#define STREAMING                0x03
	#define SYSTEM_ERROR             0xFF

#define STATUS_CONTEXT_REPEAT_COUNT  0x0054

#define STATUS_CURRENT_CONTEXT	   0x0056

#define STATUS_NEXT_CONTEXT		  0x0057

#define STATUS_VIRTUAL_CHANNEL 		0x005A
//2. CMD registers
#define CMD_BOOT      0x0200
	#define VD55G0_CMD_ACK                              0
	#define VD55G0_BOOT_BOOT                            1
	#define VD55G0_BOOT_PATCH_AND_BOOT                  2

#define CMD_STBY      0x0201
	#define VD55G0_CMD_ACK            					0x00
	#define VD55G0_STBY_START_STREAM       				0x01
	#define VD55G0_STBY_NVM_READ           				0x02
	#define VD55G0_STBY_NVM_PROG           				0x03
	#define VD55G0_STBY_END_VTRAM_UPDATE    			0xFF

#define CMD_STREAMING 0x0202
	#define VD55G0_CMD_ACK        						0
	#define VD55G0_STREAM_STOP_STREAM    				1
	#define VD55G0_STREAM_VT_FSYNC_IN_I2C 				2

//3. Sensor registers
#define SENSOR_EXT_CLOCK                         		0x0220
//default 0xb71b00

#define SENSOR_MIPI_DATA_RATE                     		0x0224
//default 0x47868C00

//4. STREAM_STATICS registers
#define  STREAM_STATICS_LINE_LENGTH						0x0300
//default 0x468

#define  STREAM_STATICS_ORIENTATION						0x0302
	#define MIRRORFLIP_NONE								0
	#define MIRRORFLIP_MIRROR							1
	#define MIRRORFLIP_FLIP								2
	#define MIRRORFLIP_MIRRORFLIP						3
#define  STREAM_STATICS_VIDEO_TIMING_CONTROLS			0x0309
//0x0

#define  STREAM_STATICS_FORMAT_CTRL						0x030a
//0xa

#define  STREAM_STATICS_OIF_CTRL						0x030c
//0x0

#define  STREAM_STATICS_OIF_VC_CTRL					0x030e
//0x0

#define  STREAM_STATICS_OIF_IMG_CTRL					0x030f
//0x2B

#define  STREAM_STATICS_OIF_ISL_CTRL					0x0310
//0x12

#define  STREAM_STATICS_OIF_ULPM						0x0311
//0x0

#define STREAM_STATICS_DUSTER_CTRL			0x0316
  #define VD55G0_DUSTER_DISABLE                       0
#define STREAM_STATICS_DARKCAL_CTRL			0x032C
  #define VD55G0_DARKCAL_BYPASS                       0
  #define VD55G0_DARKCAL_BYPASS_DARKAVG               2

#define  STREAM_STATICS_CONTEXT_REPEAT_COUNT_CTX0		0x03dc
#define  STREAM_STATICS_CONTEXT_REPEAT_COUNT_CTX1		0x03de
#define  STREAM_STATICS_CONTEXT_REPEAT_COUNT_CTX2		0x03e0
#define  STREAM_STATICS_CONTEXT_REPEAT_COUNT_CTX3		0x03e2

#define  STREAM_STATICS_CONTEXT_NEXT_CONTEXT			0x03e4
#define  STREAM_STATICS_DARKCAL_CTRL					0x032A

//5. STREAM_DYNAMICS registers
#define  STREAM_DYNAMICS_PATGEN_CTRL					0x0400
	#define VD55G0_PATGEN_CTRL_DISABLE                  0x0000
	#define VD55G0_PATGEN_CTRL_DIAG_GRAY                0x0221
	#define VD55G0_PATGEN_CTRL_PSN                      0x0281

#define  STREAM_DYNAMICS_DARKCAL_PEDESTAL 				0x0416

#define  STREAM_DYNAMICS_AE_ROI_START_H				 	0x0436
#define  STREAM_DYNAMICS_AE_ROI_START_V					0x0438
#define  STREAM_DYNAMICS_AE_ROI_END_H					0x043A
#define  STREAM_DYNAMICS_AE_ROI_END_V					0x043C

#define  STREAM_DYNAMICS_EXPOSURE_COMPILER_CONTROL_A	0x0482
	#define VD55G0_FLICKER_FREE_NONE					0x00
	#define VD55G0_FLICKER_FREE_50HZ					0x01
	#define VD55G0_FLICKER_FREE_60HZ					0x02

//6. STREAM_CTX0 registers
#define STREAM_CTX0_EXPOSURE_MODE					 	0x044C
	#define VD55G0_EXP_MODE_AUTO                        0
	#define VD55G0_EXP_MODE_FREEZE                      1
	#define VD55G0_EXP_MODE_MANUAL                      2


#define STREAM_CTX0_MANUAL_ANALOG_GAIN				    0x044D

#define STREAM_CTX0_MANUAL_COARSE_EXPOSURE			    0x044E

#define STREAM_CTX0_MANUAL_DIGITAL_GAIN				    0x0450

#define STREAM_CTX0_FRAME_LENGTH						0x0458
//0x2c8

#define STREAM_CTX0_Y_START								0x045A
//0

#define STREAM_CTX0_Y_END							 	0x045C
//0x25b

#define	STREAM_CTX0_OUT_ROI_X_START						0x045E
//0x0

#define STREAM_CTX0_OUT_ROI_X_END							0x0460
// 0x283

#define STREAM_CTX0_OUT_ROI_Y_START						0x0462
//0x0

#define STREAM_CTX0_OUT_ROI_Y_END							0x0464
// 0x25b

#define STREAM_CTX0_GPIO_x(_i_)                        	(0x0467 + _i_)

#define STREAM_CTX0_READOUT_CTRL						 0x047A

//other register
#define START_PATCH_ADDRESS 		0x2000

//parameters
#define CAMERA_VD55G0_ADDRESS          0x20U
#define CAMERA_VD55G0_FREQ_IN_HZ       12000000U

#define VD55G0_MIPI_DATA_RATE_HZ                      1200000000U
#define VD55G0_FRAME_LENGTH							  2659

//#define VD55G1_MIPI_DATA_RATE_HZ                      804000000
#define VD55G0_MIN_LINE_LEN_ADC_9                     978
#define VD55G0_MIN_LINE_LEN_ADC_10                    1128
#define VD55G0_MIPI_BPP                               8
#define VD55G0_MIPI_MARGIN                            900
#define VD55G0_MIN_VBLANK                             86
#define VD55G0_EXPO_DEF					200

#define VD55G0_LVL_ERROR 0
#define VD55G0_LVL_WARNING 1
#define VD55G0_LVL_NOTICE 2
#define VD55G0_LVL_DBG(l) (3 + (l))
#define VD55G0_MAX_WIDTH  644
#define VD55G0_MAX_HEIGHT 604
/* Camera resolution options */
#define VD55G0_OK                      (0)
#define VD55G0_ERROR                   (-1)


int VD55G0_Init(VD55G0_Ctx_t *ctx, VD55G0_Config_t *config);
int VD55G0_DeInit(VD55G0_Ctx_t *ctx);
int VD55G0_Start(VD55G0_Ctx_t *ctx);
int VD55G0_Stop(VD55G0_Ctx_t *ctx);
int VD55G0_SetFlipMirrorMode(VD55G0_Ctx_t *ctx, VD55G0_MirrorFlip_t mode);

#ifdef __cplusplus
}
#endif

#endif
