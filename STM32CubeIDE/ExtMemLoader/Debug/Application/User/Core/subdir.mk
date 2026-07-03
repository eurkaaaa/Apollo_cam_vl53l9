################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/extmem_manager.c \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/extmemloader_init.c \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/gpio.c \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/stm32n6xx_hal_msp.c \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/xspi.c \
D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/xspim.c 

OBJS += \
./Application/User/Core/extmem_manager.o \
./Application/User/Core/extmemloader_init.o \
./Application/User/Core/gpio.o \
./Application/User/Core/stm32n6xx_hal_msp.o \
./Application/User/Core/xspi.o \
./Application/User/Core/xspim.o 

C_DEPS += \
./Application/User/Core/extmem_manager.d \
./Application/User/Core/extmemloader_init.d \
./Application/User/Core/gpio.d \
./Application/User/Core/stm32n6xx_hal_msp.d \
./Application/User/Core/xspi.d \
./Application/User/Core/xspim.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Core/extmem_manager.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/extmem_manager.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/extmemloader_init.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/extmemloader_init.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/gpio.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/gpio.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/stm32n6xx_hal_msp.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/stm32n6xx_hal_msp.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/xspi.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/xspi.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/xspim.o: D:/STM32/STM32CubeIDE/Apollo/ExtMemLoader/Core/Src/xspim.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Core

clean-Application-2f-User-2f-Core:
	-$(RM) ./Application/User/Core/extmem_manager.cyclo ./Application/User/Core/extmem_manager.d ./Application/User/Core/extmem_manager.o ./Application/User/Core/extmem_manager.su ./Application/User/Core/extmemloader_init.cyclo ./Application/User/Core/extmemloader_init.d ./Application/User/Core/extmemloader_init.o ./Application/User/Core/extmemloader_init.su ./Application/User/Core/gpio.cyclo ./Application/User/Core/gpio.d ./Application/User/Core/gpio.o ./Application/User/Core/gpio.su ./Application/User/Core/stm32n6xx_hal_msp.cyclo ./Application/User/Core/stm32n6xx_hal_msp.d ./Application/User/Core/stm32n6xx_hal_msp.o ./Application/User/Core/stm32n6xx_hal_msp.su ./Application/User/Core/xspi.cyclo ./Application/User/Core/xspi.d ./Application/User/Core/xspi.o ./Application/User/Core/xspi.su ./Application/User/Core/xspim.cyclo ./Application/User/Core/xspim.d ./Application/User/Core/xspim.o ./Application/User/Core/xspim.su

.PHONY: clean-Application-2f-User-2f-Core

