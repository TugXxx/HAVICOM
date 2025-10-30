################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.c \
../SynaptiX/board/ModbusSlave/modbus/functions/mbutils.c 

OBJS += \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.o \
./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.o 

C_DEPS += \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.d \
./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
SynaptiX/board/ModbusSlave/modbus/functions/%.o SynaptiX/board/ModbusSlave/modbus/functions/%.su SynaptiX/board/ModbusSlave/modbus/functions/%.cyclo: ../SynaptiX/board/ModbusSlave/modbus/functions/%.c SynaptiX/board/ModbusSlave/modbus/functions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/utils" -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../LWIP/App -I../LWIP/Target -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/modem" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/A76xx" -I../Drivers/BSP/Components/lan8742 -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/dp83848" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/config" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/mqtt_server" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/cJSON" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/device_state" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/rtc_rx8130ce" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/iic_master" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/include" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/modbus/rtu" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/ModbusSlave/port" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/nanomodbus/port" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/nanomodbus" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SynaptiX-2f-board-2f-ModbusSlave-2f-modbus-2f-functions

clean-SynaptiX-2f-board-2f-ModbusSlave-2f-modbus-2f-functions:
	-$(RM) ./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfunccoils.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdiag.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncdisc.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncholding.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncinput.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbfuncother.su ./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.cyclo ./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.d ./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.o ./SynaptiX/board/ModbusSlave/modbus/functions/mbutils.su

.PHONY: clean-SynaptiX-2f-board-2f-ModbusSlave-2f-modbus-2f-functions

