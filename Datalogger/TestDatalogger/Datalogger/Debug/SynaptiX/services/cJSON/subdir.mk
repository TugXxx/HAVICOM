################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SynaptiX/services/cJSON/cJSON.c \
../SynaptiX/services/cJSON/cJSON_Utils.c 

OBJS += \
./SynaptiX/services/cJSON/cJSON.o \
./SynaptiX/services/cJSON/cJSON_Utils.o 

C_DEPS += \
./SynaptiX/services/cJSON/cJSON.d \
./SynaptiX/services/cJSON/cJSON_Utils.d 


# Each subdirectory must supply rules for building sources it contributes
SynaptiX/services/cJSON/%.o SynaptiX/services/cJSON/%.su SynaptiX/services/cJSON/%.cyclo: ../SynaptiX/services/cJSON/%.c SynaptiX/services/cJSON/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/utils" -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../LWIP/App -I../LWIP/Target -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/modem" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/A76xx" -I../Drivers/BSP/Components/lan8742 -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/dp83848" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/config" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/mqtt_server" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/cJSON" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/device_state" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/rtc_rx8130ce" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/iic_master" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/include" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/rtu" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/port" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/nanomodbus/port" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/nanomodbus" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SynaptiX-2f-services-2f-cJSON

clean-SynaptiX-2f-services-2f-cJSON:
	-$(RM) ./SynaptiX/services/cJSON/cJSON.cyclo ./SynaptiX/services/cJSON/cJSON.d ./SynaptiX/services/cJSON/cJSON.o ./SynaptiX/services/cJSON/cJSON.su ./SynaptiX/services/cJSON/cJSON_Utils.cyclo ./SynaptiX/services/cJSON/cJSON_Utils.d ./SynaptiX/services/cJSON/cJSON_Utils.o ./SynaptiX/services/cJSON/cJSON_Utils.su

.PHONY: clean-SynaptiX-2f-services-2f-cJSON

