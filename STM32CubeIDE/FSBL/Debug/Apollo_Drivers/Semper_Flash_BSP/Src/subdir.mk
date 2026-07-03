################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.c 

OBJS += \
./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.o 

C_DEPS += \
./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.d 


# Each subdirectory must supply rules for building sources it contributes
Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.o: D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/FSBL/Application/User/Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.c Apollo_Drivers/Semper_Flash_BSP/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../../../FSBL/Core/Inc -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/ST/STM32_ExtMem_Manager -I../../../Middlewares/ST/STM32_ExtMem_Manager/boot -I../../../Middlewares/ST/STM32_ExtMem_Manager/sal -I../../../Middlewares/ST/STM32_ExtMem_Manager/nor_sfdp -I../../../Middlewares/ST/STM32_ExtMem_Manager/psram -I../../../Middlewares/ST/STM32_ExtMem_Manager/sdcard -I../../../Middlewares/ST/STM32_ExtMem_Manager/user -I../../../Drivers/CMSIS/Include -I../../../Drivers/Apollo_Drivers/Micron_Flash_BSP/inc -I../../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc -I../../../Test/inc -I../../../Third_Party/debug_print/inc -I../../../Third_Party/unity/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Apollo_Drivers-2f-Semper_Flash_BSP-2f-Src

clean-Apollo_Drivers-2f-Semper_Flash_BSP-2f-Src:
	-$(RM) ./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.cyclo ./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.d ./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.o ./Apollo_Drivers/Semper_Flash_BSP/Src/semperflash_drv.su

.PHONY: clean-Apollo_Drivers-2f-Semper_Flash_BSP-2f-Src

