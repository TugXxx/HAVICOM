################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SynaptiX/utils/subdir.mk
-include SynaptiX/services/iic_master/subdir.mk
-include SynaptiX/services/cJSON/subdir.mk
-include SynaptiX/services/subdir.mk
-include SynaptiX/board/rtc_rx8130ce/subdir.mk
-include SynaptiX/board/modem/subdir.mk
-include SynaptiX/board/dp83848/subdir.mk
-include SynaptiX/board/ModbusSlave/port/subdir.mk
-include SynaptiX/board/ModbusSlave/modbus/tcp/subdir.mk
-include SynaptiX/board/ModbusSlave/modbus/rtu/subdir.mk
-include SynaptiX/board/ModbusSlave/modbus/functions/subdir.mk
-include SynaptiX/board/ModbusSlave/modbus/ascii/subdir.mk
-include SynaptiX/board/ModbusSlave/modbus/subdir.mk
-include SynaptiX/board/A76xx/subdir.mk
-include SynaptiX/board/subdir.mk
-include SynaptiX/apps/mqtt_server/subdir.mk
-include SynaptiX/apps/device_state/subdir.mk
-include SynaptiX/apps/config/subdir.mk
-include SynaptiX/apps/subdir.mk
-include Middlewares/Third_Party/LwIP/system/OS/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/ppp/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv6/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv4/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/subdir.mk
-include Middlewares/Third_Party/LwIP/src/apps/sntp/subdir.mk
-include Middlewares/Third_Party/LwIP/src/apps/mqtt/subdir.mk
-include Middlewares/Third_Party/LwIP/src/api/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include LWIP/Target/subdir.mk
-include LWIP/App/subdir.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Drivers/BSP/Components/lan8742/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := Datalogger
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Datalogger.elf \

MAP_FILES += \
Datalogger.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Datalogger.list \


# All Target
all: main-build

# Main-build Target
main-build: Datalogger.elf secondary-outputs

# Tool invocations
Datalogger.elf Datalogger.map: $(OBJS) $(USER_OBJS) D:\Local\Git\HAVICOM\Datalogger\TestDatalogger\Datalogger\STM32H743IITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Datalogger.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"D:\Local\Git\HAVICOM\Datalogger\TestDatalogger\Datalogger\STM32H743IITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Datalogger.map" -Wl,--gc-sections -static --data-sections --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Datalogger.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Datalogger.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Datalogger.elf Datalogger.list Datalogger.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
