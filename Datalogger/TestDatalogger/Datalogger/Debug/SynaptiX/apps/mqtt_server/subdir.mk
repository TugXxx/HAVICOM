################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SynaptiX/apps/mqtt_server/environment.c \
../SynaptiX/apps/mqtt_server/mqtt_interface.c \
../SynaptiX/apps/mqtt_server/mqtt_server_app.c 

OBJS += \
./SynaptiX/apps/mqtt_server/environment.o \
./SynaptiX/apps/mqtt_server/mqtt_interface.o \
./SynaptiX/apps/mqtt_server/mqtt_server_app.o 

C_DEPS += \
./SynaptiX/apps/mqtt_server/environment.d \
./SynaptiX/apps/mqtt_server/mqtt_interface.d \
./SynaptiX/apps/mqtt_server/mqtt_server_app.d 


# Each subdirectory must supply rules for building sources it contributes
SynaptiX/apps/mqtt_server/%.o SynaptiX/apps/mqtt_server/%.su SynaptiX/apps/mqtt_server/%.cyclo: ../SynaptiX/apps/mqtt_server/%.c SynaptiX/apps/mqtt_server/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/utils" -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../LWIP/App -I../LWIP/Target -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/modem" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/A76xx" -I../Drivers/BSP/Components/lan8742 -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/dp83848" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/config" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/mqtt_server" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/cJSON" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/device_state" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/rtc_rx8130ce" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/iic_master" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/include" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/rtu" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/port" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SynaptiX-2f-apps-2f-mqtt_server

clean-SynaptiX-2f-apps-2f-mqtt_server:
	-$(RM) ./SynaptiX/apps/mqtt_server/environment.cyclo ./SynaptiX/apps/mqtt_server/environment.d ./SynaptiX/apps/mqtt_server/environment.o ./SynaptiX/apps/mqtt_server/environment.su ./SynaptiX/apps/mqtt_server/mqtt_interface.cyclo ./SynaptiX/apps/mqtt_server/mqtt_interface.d ./SynaptiX/apps/mqtt_server/mqtt_interface.o ./SynaptiX/apps/mqtt_server/mqtt_interface.su ./SynaptiX/apps/mqtt_server/mqtt_server_app.cyclo ./SynaptiX/apps/mqtt_server/mqtt_server_app.d ./SynaptiX/apps/mqtt_server/mqtt_server_app.o ./SynaptiX/apps/mqtt_server/mqtt_server_app.su

.PHONY: clean-SynaptiX-2f-apps-2f-mqtt_server

