#include <stdint.h>
#include "cmw_sensors_if.h"
#include "cmw_errno.h"
#include "vd55g0.h"

typedef struct
{
  uint16_t Address;
  uint32_t ClockInHz;
  VD55G0_Ctx_t ctx_driver;
  uint8_t IsInitialized;
  int32_t (*Init)(void);
  int32_t (*DeInit)(void);
  int32_t (*WriteReg)(uint16_t, uint16_t, uint8_t*, uint16_t);
  int32_t (*ReadReg) (uint16_t, uint16_t, uint8_t*, uint16_t);
  int32_t (*GetTick) (void);
  void (*Delay)(uint32_t delay_in_ms);
  void (*ShutdownPin)(int value);
  void (*EnablePin)(int value);
} CMW_VD55G0_t;

int CMW_VD55G0_Probe(CMW_VD55G0_t *io_ctx, CMW_Sensor_if_t *vd55g0_if);