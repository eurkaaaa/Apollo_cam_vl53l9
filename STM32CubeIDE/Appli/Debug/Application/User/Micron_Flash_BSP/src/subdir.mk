################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/Micron_Flash_BSP/src/mt25ql512abb.c 

OBJS += \
./Application/User/Micron_Flash_BSP/src/mt25ql512abb.o 

C_DEPS += \
./Application/User/Micron_Flash_BSP/src/mt25ql512abb.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Micron_Flash_BSP/src/%.o Application/User/Micron_Flash_BSP/src/%.su Application/User/Micron_Flash_BSP/src/%.cyclo: ../Application/User/Micron_Flash_BSP/src/%.c Application/User/Micron_Flash_BSP/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/sensors" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/sensors/imx335" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/sensors/vd55g0" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/sensors/vd55g1" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/sensors/vd6g" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/Third_Party/unity/inc" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/Third_Party/debug_print/inc" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/Test/inc" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/evision/Inc" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp/Inc" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp_param_conf" -I../../../DSP/Include -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/vl53l9" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/app" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/interface/vl53l9" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/lib/media-object/media-c/header-lib/include/public" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/lib/media-object/transform-c/header-lib/include/public" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/lib/vl53l9-utils/include" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/private" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/public" -I"D:/Apollo_cam_vl53l9/Apollo_cam_vl53l9/STM32CubeIDE/Appli/Application/User/BSP/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Micron_Flash_BSP-2f-src

clean-Application-2f-User-2f-Micron_Flash_BSP-2f-src:
	-$(RM) ./Application/User/Micron_Flash_BSP/src/mt25ql512abb.cyclo ./Application/User/Micron_Flash_BSP/src/mt25ql512abb.d ./Application/User/Micron_Flash_BSP/src/mt25ql512abb.o ./Application/User/Micron_Flash_BSP/src/mt25ql512abb.su

.PHONY: clean-Application-2f-User-2f-Micron_Flash_BSP-2f-src

