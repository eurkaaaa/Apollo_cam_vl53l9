################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/Core/Src/syscalls.c \
../Application/User/Core/Src/sysmem.c 

OBJS += \
./Application/User/Core/Src/syscalls.o \
./Application/User/Core/Src/sysmem.o 

C_DEPS += \
./Application/User/Core/Src/syscalls.d \
./Application/User/Core/Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Core/Src/%.o Application/User/Core/Src/%.su Application/User/Core/Src/%.cyclo: ../Application/User/Core/Src/%.c Application/User/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DSTM32_EXTMEMLOADER_STM32CUBETARGET -c -I../../../ExtMemLoader/Core/Inc -I../../../Secure_nsclib -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Loader/core -I../../../Middlewares/ST/STM32_ExtMem_Loader/MDK-ARM -I../../../Middlewares/ST/STM32_ExtMem_Loader/STM32Cube -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Core-2f-Src

clean-Application-2f-User-2f-Core-2f-Src:
	-$(RM) ./Application/User/Core/Src/syscalls.cyclo ./Application/User/Core/Src/syscalls.d ./Application/User/Core/Src/syscalls.o ./Application/User/Core/Src/syscalls.su ./Application/User/Core/Src/sysmem.cyclo ./Application/User/Core/Src/sysmem.d ./Application/User/Core/Src/sysmem.o ./Application/User/Core/Src/sysmem.su

.PHONY: clean-Application-2f-User-2f-Core-2f-Src

