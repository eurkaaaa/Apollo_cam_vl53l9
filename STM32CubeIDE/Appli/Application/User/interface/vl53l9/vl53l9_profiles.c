#include "interface.h"
#include "vl53l9.h"

#define USEC_PER_SEC           (1000000U)
#define FPS_TO_FRAME_PERIOD(x) ((uint32_t)((1 / (float)x) * USEC_PER_SEC))

vl53l9_profile_t g_ranging_profiles[VL53L9_NB_USECASES] = {
    {
        .id = VL53L9_USECASE_AR_RANGE,
        .sync = VL53L9_SYNC_AUTONOMOUS,
        .power = VL53L9_POWER_REGULAR,
        .context = VL53L9_CONTEXT_LONG,
        .frame_period_us = FPS_TO_FRAME_PERIOD(30),
        .binning = 2,
        .exposure_ms = 8,
    },
    {
        .id = VL53L9_USECASE_AR_PRECISION,
        .sync = VL53L9_SYNC_AUTONOMOUS,
        .power = VL53L9_POWER_REGULAR,
        .context = VL53L9_CONTEXT_SHORT,
        .frame_period_us = FPS_TO_FRAME_PERIOD(30),
        .binning = 2,
        .exposure_ms = 10,
    },
    {
        .id = VL53L9_USECASE_AF_RANGE,
        .sync = VL53L9_SYNC_AUTONOMOUS,
        .power = VL53L9_POWER_REGULAR,
        .context = VL53L9_CONTEXT_LONG,
        .frame_period_us = FPS_TO_FRAME_PERIOD(30),
        .binning = 4,
        .exposure_ms = 4,
    },
    {
        .id = VL53L9_USECASE_AF,
        .sync = VL53L9_SYNC_AUTONOMOUS,
        .power = VL53L9_POWER_REGULAR,
        .context = VL53L9_CONTEXT_SHORT,
        .frame_period_us = FPS_TO_FRAME_PERIOD(30),
        .binning = 4,
        .exposure_ms = 5,
    },
};

int platform_set_profile(vl53l9_device_t *p_dev, vl53l9_profile_t *p_profile) {
    int ret;

    ret = vl53l9_set_sync_mode(p_dev, p_profile->sync);
    if (ret) {
        return ret;
    }

    ret = vl53l9_set_power_mode(p_dev, p_profile->power);
    if (ret) {
        return ret;
    }

    ret = vl53l9_set_frame_period(p_dev, p_profile->frame_period_us);
    if (ret) {
        return ret;
    }

    ret = vl53l9_set_context_selection(p_dev, p_profile->context);
    if (ret) {
        return ret;
    }

    ret = vl53l9_set_context_binning(p_dev, p_profile->context, p_profile->binning);
    if (ret) {
        return ret;
    }

    ret = vl53l9_set_context_exposure(p_dev, p_profile->context, p_profile->exposure_ms);
    if (ret) {
        return ret;
    }

    return 0;
}
