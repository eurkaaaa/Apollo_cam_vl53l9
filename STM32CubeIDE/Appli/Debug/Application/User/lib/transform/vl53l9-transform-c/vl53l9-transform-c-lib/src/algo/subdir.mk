################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.c \
../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.c 

OBJS += \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.o \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.o 

C_DEPS += \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.d \
./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/%.o Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/%.su Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/%.cyclo: ../Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/%.c Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/imx335" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g0" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g1" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd6g" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/unity/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/debug_print/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Test/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/evision/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp_param_conf" -I../../../DSP/Include -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/vl53l9" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/app" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/interface/vl53l9" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/media-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/media-object/transform-c/header-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/vl53l9-utils/include" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/private" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/include/public" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/BSP/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-lib-2f-transform-2f-vl53l9-2d-transform-2d-c-2f-vl53l9-2d-transform-2d-c-2d-lib-2f-src-2f-algo

clean-Application-2f-User-2f-lib-2f-transform-2f-vl53l9-2d-transform-2d-c-2f-vl53l9-2d-transform-2d-c-2d-lib-2f-src-2f-algo:
	-$(RM) ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/confidence.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_calibration.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/distance_check.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/extract.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/radial_to_perp.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/ratenorm.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/reflectance.su ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.cyclo ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.d ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.o ./Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/sharpener.su

.PHONY: clean-Application-2f-User-2f-lib-2f-transform-2f-vl53l9-2d-transform-2d-c-2f-vl53l9-2d-transform-2d-c-2d-lib-2f-src-2f-algo

