################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.c \
../Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.c \
../Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.c 

OBJS += \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.o \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.o \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.o 

C_DEPS += \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.d \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.d \
./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/STM32_ISP_Library/isp/USB_Device/Src/%.o Application/User/STM32_ISP_Library/isp/USB_Device/Src/%.su Application/User/STM32_ISP_Library/isp/USB_Device/Src/%.cyclo: ../Application/User/STM32_ISP_Library/isp/USB_Device/Src/%.c Application/User/STM32_ISP_Library/isp/USB_Device/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../Appli/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/BSP" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/imx335" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g0" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd55g1" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/sensors/vd6g" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/unity/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Third_Party/debug_print/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/Test/inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/evision/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp/Inc" -I"D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Application/User/STM32_ISP_Library/isp_param_conf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-STM32_ISP_Library-2f-isp-2f-USB_Device-2f-Src

clean-Application-2f-User-2f-STM32_ISP_Library-2f-isp-2f-USB_Device-2f-Src:
	-$(RM) ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.cyclo ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.d ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.o ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usb_device.su ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.cyclo ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.d ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.o ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_cdc_if.su ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.cyclo ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.d ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.o ./Application/User/STM32_ISP_Library/isp/USB_Device/Src/usbd_desc.su

.PHONY: clean-Application-2f-User-2f-STM32_ISP_Library-2f-isp-2f-USB_Device-2f-Src

