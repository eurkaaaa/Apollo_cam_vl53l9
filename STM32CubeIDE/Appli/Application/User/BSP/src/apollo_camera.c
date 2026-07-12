#include "apollo_camera.h"

#include <stdint.h>
#include <string.h>

#include "main.h"
#include "cmw_camera.h"
#include "app_config.h"

/*
 * 说明：
 * 1. 这一层是 BSP 内部封装，所以允许 include cmw_camera.h。
 * 2. 用户层不要直接 include cmw_camera.h。
 * 3. 默认输出 RGB565，所以 bpp = 2。
 */

#define APOLLO_CAMERA_OUTPUT_BPP    (2U)

static ApolloCamera_Pipe_t s_camera_pipe = APOLLO_CAMERA_PIPE_1;
static uint32_t s_camera_pitch = 0U;
static uint8_t s_camera_initialized = 0U;

static void ApolloCamera_SelectAndResetSensor(void)
{
    /*
     * 当前硬件上 CSI_SEL = RESET 表示选择普通相机通道。
     * VL53L9 分支原来使用 CSI_SEL = SET。
     */
    HAL_GPIO_WritePin(CSI_SEL_GPIO_Port, CSI_SEL_Pin, GPIO_PIN_RESET);
    HAL_Delay(200);

    /*
     * 复位相机传感器。
     * 这部分原来写在 main.c，现在移动到 BSP 封装内部。
     */
    HAL_GPIO_WritePin(XSHUTDOWN_GPIO_Port, XSHUTDOWN_Pin, GPIO_PIN_RESET);
    HAL_Delay(200);

    HAL_GPIO_WritePin(XSHUTDOWN_GPIO_Port, XSHUTDOWN_Pin, GPIO_PIN_SET);
    HAL_Delay(200);
}

static int32_t ApolloCamera_MapPipe(ApolloCamera_Pipe_t pipe, uint32_t *real_pipe)
{
    if (real_pipe == NULL)
    {
        return -1;
    }

    switch (pipe)
    {
    case APOLLO_CAMERA_PIPE_0:
        *real_pipe = DCMIPP_PIPE0;
        return 0;

    case APOLLO_CAMERA_PIPE_1:
        *real_pipe = DCMIPP_PIPE1;
        return 0;

    case APOLLO_CAMERA_PIPE_2:
        *real_pipe = DCMIPP_PIPE2;
        return 0;

    default:
        return -1;
    }
}

int32_t ApolloCamera_Init(uint32_t length,
                          uint32_t width,
                          uint32_t fps,
                          ApolloCamera_Pipe_t pipe)
{
    int32_t ret;
    CMW_CameraInit_t cam_conf;
    CMW_DCMIPP_Conf_t dcmipp_conf;
    uint32_t real_pipe;

    if ((length == 0U) || (width == 0U) || (fps == 0U))
    {
        return -1;
    }

    memset(&cam_conf, 0, sizeof(cam_conf));
    memset(&dcmipp_conf, 0, sizeof(dcmipp_conf));

    if (ApolloCamera_MapPipe(pipe, &real_pipe) != 0)
    {
        return -1;
    }

    s_camera_pipe = pipe;

    ApolloCamera_SelectAndResetSensor();

    /*
     * 用户只传 length / width / fps / pipe。
     * 下面这些底层参数都在 BSP 内部固定。
     */
    cam_conf.width = width;
    cam_conf.height = length;
    cam_conf.fps = (int)fps;
    cam_conf.pixel_format = 0;
    cam_conf.anti_flicker = 0;
    cam_conf.mirror_flip = CAMERA_FLIP;

    ret = CMW_CAMERA_Init(&cam_conf);
    if (ret != CMW_ERROR_NONE)
    {
        return ret;
    }

    dcmipp_conf.output_width = width;
    dcmipp_conf.output_height = length;
    dcmipp_conf.output_format = DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1;
    dcmipp_conf.output_bpp = APOLLO_CAMERA_OUTPUT_BPP;
    dcmipp_conf.enable_swap = 0;
    dcmipp_conf.enable_gamma_conversion = 0;
    dcmipp_conf.mode = CMW_Aspect_ratio_fit;

    ret = CMW_CAMERA_SetPipeConfig(real_pipe, &dcmipp_conf, &s_camera_pitch);
    if (ret != CMW_ERROR_NONE)
    {
        return ret;
    }

    s_camera_initialized = 1U;

    return 0;
}

int32_t ApolloCamera_Start(uint8_t *frame_buffer)
{
    uint32_t real_pipe;

    if ((s_camera_initialized == 0U) || (frame_buffer == NULL))
    {
        return CMW_ERROR_WRONG_PARAM;
    }

    if (ApolloCamera_MapPipe(s_camera_pipe, &real_pipe) != 0)
    {
        return -1;
    }

    return CMW_CAMERA_Start(real_pipe, frame_buffer, CMW_MODE_CONTINUOUS);
}

int32_t ApolloCamera_Stop(void)
{
    uint32_t real_pipe;

    if (s_camera_initialized == 0U)
    {
        return CMW_ERROR_NONE;
    }

    if (ApolloCamera_MapPipe(s_camera_pipe, &real_pipe) != 0)
    {
        return -1;
    }

    return CMW_CAMERA_Suspend(real_pipe);
}

int32_t ApolloCamera_DeInit(void)
{
    int32_t ret;

    ret = CMW_CAMERA_DeInit();

    if (ret == CMW_ERROR_NONE)
    {
        s_camera_initialized = 0U;
    }

    return ret;
}

int32_t ApolloCamera_Run(void)
{
    return CMW_CAMERA_Run();
}
