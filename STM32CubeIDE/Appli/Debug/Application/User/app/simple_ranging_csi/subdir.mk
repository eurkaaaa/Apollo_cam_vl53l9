################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/app/simple_ranging_csi/simple_ranging_csi.c 

OBJS += \
./Application/User/app/simple_ranging_csi/simple_ranging_csi.o 

C_DEPS += \
./Application/User/app/simple_ranging_csi/simple_ranging_csi.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/app/simple_ranging_csi/%.o Application/User/app/simple_ranging_csi/%.su Application/User/app/simple_ranging_csi/%.cyclo: ../Application/User/app/simple_ranging_csi/%.c Application/User/app/simple_ranging_csi/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/imx335" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g0" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g1" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd6g" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/unity/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/debug_print/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Test/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/evision/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp_param_conf" -I../../../DSP/Include -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/vl53l9" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/app" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/interface/vl53l9" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/media-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/transform-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/vl53l9-utils/include" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/private" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/BSP/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-app-2f-simple_ranging_csi

clean-Application-2f-User-2f-app-2f-simple_ranging_csi:
	-$(RM) ./Application/User/app/simple_ranging_csi/simple_ranging_csi.cyclo ./Application/User/app/simple_ranging_csi/simple_ranging_csi.d ./Application/User/app/simple_ranging_csi/simple_ranging_csi.o ./Application/User/app/simple_ranging_csi/simple_ranging_csi.su

.PHONY: clean-Application-2f-User-2f-app-2f-simple_ranging_csi

