################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/STM32_ExtMem_Manager/subdir.mk
-include Middlewares/STM32_ExtMem_Loader/subdir.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Application/User/Core/Src/subdir.mk
-include Application/User/Core/subdir.mk
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


BUILD_ARTIFACT_NAME := Apollo_ExtMemLoader
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Apollo_ExtMemLoader.elf \

MAP_FILES += \
Apollo_ExtMemLoader.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Apollo_ExtMemLoader.list \

OBJCOPY_BIN += \
Apollo_ExtMemLoader.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Apollo_ExtMemLoader.elf secondary-outputs

# Tool invocations
Apollo_ExtMemLoader.elf Apollo_ExtMemLoader.map: $(OBJS) $(USER_OBJS) D:\STM32\STM32CubeIDE\Apollo\STM32CubeIDE\ExtMemLoader\STM32N657B0HXQ_extmemloader_default.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Apollo_ExtMemLoader.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"D:\STM32\STM32CubeIDE\Apollo\STM32CubeIDE\ExtMemLoader\STM32N657B0HXQ_extmemloader_default.ld" --specs=nosys.specs -Wl,-Map="Apollo_ExtMemLoader.map" -Wl,--gc-sections -static -Wl,--cmse-implib -Wl,--out-implib=./secure_nsclib.o --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Apollo_ExtMemLoader.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Apollo_ExtMemLoader.list"
	@echo 'Finished building: $@'
	@echo ' '

Apollo_ExtMemLoader.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Apollo_ExtMemLoader.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Apollo_ExtMemLoader.bin Apollo_ExtMemLoader.elf Apollo_ExtMemLoader.list Apollo_ExtMemLoader.map default.size.stdout
	-@echo ' '

post-build:
	bash "../postbuild.sh" "D:\ST\STM32CubeIDE_2.1.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.400.202601091506\tools\bin/ExternalLoader"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
