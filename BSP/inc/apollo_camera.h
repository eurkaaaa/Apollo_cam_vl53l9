#ifndef APOLLO_CAMERA_H
#define APOLLO_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * 对用户只暴露抽象 pipe 编号，不暴露 DCMIPP_PIPE0/1/2。
 */
typedef enum
{
    APOLLO_CAMERA_PIPE_0 = 0,
    APOLLO_CAMERA_PIPE_1 = 1,
    APOLLO_CAMERA_PIPE_2 = 2,
} ApolloCamera_Pipe_t;

/*
 * 用户唯一需要调用的初始化接口：
 * length: 图像长度，这里对应图像高度 height
 * width : 图像宽度
 * fps   : 帧率
 * pipe  : 使用哪一路图像管道
 */
int32_t ApolloCamera_Init(uint32_t length,
                          uint32_t width,
                          uint32_t fps,
                          ApolloCamera_Pipe_t pipe);

/*
 * 启动相机采集。
 */
int32_t ApolloCamera_Start(uint8_t *frame_buffer);

/*
 * 停止采集。
 */
int32_t ApolloCamera_Stop(void);

/*
 * 释放相机资源。
 */
int32_t ApolloCamera_DeInit(void);

/*
 * 运行相机 ISP 更新。
 */
int32_t ApolloCamera_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* APOLLO_CAMERA_H */
