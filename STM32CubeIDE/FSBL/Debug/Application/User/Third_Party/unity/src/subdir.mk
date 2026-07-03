################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/Third_Party/unity/src/unity.c \
../Application/User/Third_Party/unity/src/unity_fixture.c \
../Application/User/Third_Party/unity/src/unity_memory.c 

OBJS += \
./Application/User/Third_Party/unity/src/unity.o \
./Application/User/Third_Party/unity/src/unity_fixture.o \
./Application/User/Third_Party/unity/src/unity_memory.o 

C_DEPS += \
./Application/User/Third_Party/unity/src/unity.d \
./Application/User/Third_Party/unity/src/unity_fixture.d \
./Application/User/Third_Party/unity/src/unity_memory.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Third_Party/unity/src/%.o Application/User/Third_Party/unity/src/%.su Application/User/Third_Party/unity/src/%.cyclo: ../Application/User/Third_Party/unity/src/%.c Application/User/Third_Party/unity/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../FSBL/Core/Inc -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/boot -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Test/inc -I../../../Third_Party/debug_print/inc -I../../../Third_Party/unity/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Third_Party-2f-unity-2f-src

clean-Application-2f-User-2f-Third_Party-2f-unity-2f-src:
	-$(RM) ./Application/User/Third_Party/unity/src/unity.cyclo ./Application/User/Third_Party/unity/src/unity.d ./Application/User/Third_Party/unity/src/unity.o ./Application/User/Third_Party/unity/src/unity.su ./Application/User/Third_Party/unity/src/unity_fixture.cyclo ./Application/User/Third_Party/unity/src/unity_fixture.d ./Application/User/Third_Party/unity/src/unity_fixture.o ./Application/User/Third_Party/unity/src/unity_fixture.su ./Application/User/Third_Party/unity/src/unity_memory.cyclo ./Application/User/Third_Party/unity/src/unity_memory.d ./Application/User/Third_Party/unity/src/unity_memory.o ./Application/User/Third_Party/unity/src/unity_memory.su

.PHONY: clean-Application-2f-User-2f-Third_Party-2f-unity-2f-src

