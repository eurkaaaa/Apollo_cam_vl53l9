################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/User/Startup/startup_stm32n657b0hxq.s 

OBJS += \
./Application/User/Startup/startup_stm32n657b0hxq.o 

S_DEPS += \
./Application/User/Startup/startup_stm32n657b0hxq.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Startup/%.o: ../Application/User/Startup/%.s Application/User/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m55 -g3 -DDEBUG -c -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/BSP" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Test" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party" -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/unity/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/imx335" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g0" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g1" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd6g" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp_param_conf" -I"D:/STM32/STM32CubeIDE/Apollo/DSP/Include" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Debug/Application/User/vl53l9" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/app" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/media-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/transform-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/vl53l9-utils/include" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/private" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/public" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Application-2f-User-2f-Startup

clean-Application-2f-User-2f-Startup:
	-$(RM) ./Application/User/Startup/startup_stm32n657b0hxq.d ./Application/User/Startup/startup_stm32n657b0hxq.o

.PHONY: clean-Application-2f-User-2f-Startup

