################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Application/User/vl53l9/subdir.mk
-include Application/User/sensors/vd6g/subdir.mk
-include Application/User/sensors/vd55g1/subdir.mk
-include Application/User/sensors/vd55g0/subdir.mk
-include Application/User/sensors/imx335/subdir.mk
-include Application/User/sensors/subdir.mk
-include Application/User/lib/vl53l9-utils/src/subdir.mk
-include Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/algo/subdir.mk
-include Application/User/lib/transform/vl53l9-transform-c/vl53l9-transform-c-lib/src/subdir.mk
-include Application/User/interface/vl53l9/subdir.mk
-include Application/User/interface/platform/subdir.mk
-include Application/User/app/simple_ranging_csi/subdir.mk
-include Application/User/Third_Party/unity/src/subdir.mk
-include Application/User/Third_Party/debug_print/src/subdir.mk
-include Application/User/Test/src/subdir.mk
-include Application/User/Startup/subdir.mk
-include Application/User/STM32_ISP_Library/isp/Src/subdir.mk
-include Application/User/Core/subdir.mk
-include Application/User/BSP/src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Apollo_Appli
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Apollo_Appli.elf \

MAP_FILES += \
Apollo_Appli.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Apollo_Appli.list \

OBJCOPY_BIN += \
Apollo_Appli.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Apollo_Appli.elf secondary-outputs

# Tool invocations
Apollo_Appli.elf Apollo_Appli.map: $(OBJS) $(USER_OBJS) D:\STM32\STM32CubeIDE\Apollo\STM32CubeIDE\Appli\STM32N657B0HXQ_LRUN.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Apollo_Appli.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"D:\STM32\STM32CubeIDE\Apollo\STM32CubeIDE\Appli\STM32N657B0HXQ_LRUN.ld" --specs=nosys.specs -Wl,-Map="Apollo_Appli.map" -Wl,--gc-sections -static -Wl,--cmse-implib -Wl,--out-implib=./secure_nsclib.o -L"D:\STM32\STM32CubeIDE\Apollo\STM32CubeIDE\Appli\Application\User\STM32_ISP_Library\evision\Lib" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Apollo_Appli.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Apollo_Appli.list"
	@echo 'Finished building: $@'
	@echo ' '

Apollo_Appli.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Apollo_Appli.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Apollo_Appli.bin Apollo_Appli.elf Apollo_Appli.list Apollo_Appli.map default.size.stdout
	-@echo ' '

post-build:
	cd "D:/STM32/STM32CubeIDE/Apollo/STM32CubeIDE/Appli/Debug"
	echo y | "E:\STM32CUBEPROGRAMMER\bin\STM32_SigningTool_CLI.exe" -bin "Apollo_Appli.bin" -nk -of 0x80000000 -t fsbl -o "Apollo_Appli-Trusted.bin" -hv 2.3 -dump "Apollo_Appli-Trusted.bin" -align
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
