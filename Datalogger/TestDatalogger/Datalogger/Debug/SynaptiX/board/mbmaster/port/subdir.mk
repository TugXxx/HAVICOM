################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SynaptiX/board/mbmaster/port/mbportevent.c \
../SynaptiX/board/mbmaster/port/mbportother.c \
../SynaptiX/board/mbmaster/port/mbportserial.c \
../SynaptiX/board/mbmaster/port/mbporttimer.c 

OBJS += \
./SynaptiX/board/mbmaster/port/mbportevent.o \
./SynaptiX/board/mbmaster/port/mbportother.o \
./SynaptiX/board/mbmaster/port/mbportserial.o \
./SynaptiX/board/mbmaster/port/mbporttimer.o 

C_DEPS += \
./SynaptiX/board/mbmaster/port/mbportevent.d \
./SynaptiX/board/mbmaster/port/mbportother.d \
./SynaptiX/board/mbmaster/port/mbportserial.d \
./SynaptiX/board/mbmaster/port/mbporttimer.d 


# Each subdirectory must supply rules for building sources it contributes
SynaptiX/board/mbmaster/port/%.o SynaptiX/board/mbmaster/port/%.su SynaptiX/board/mbmaster/port/%.cyclo: ../SynaptiX/board/mbmaster/port/%.c SynaptiX/board/mbmaster/port/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/utils" -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../LWIP/App -I../LWIP/Target -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/modem" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/A76xx" -I../Drivers/BSP/Components/lan8742 -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/dp83848" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/config" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/mqtt_server" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/cJSON" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/device_state" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/rtc_rx8130ce" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/iic_master" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include/internal" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include/common" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/port" -I"/Datalogger/SynaptiX/board/mbmaster/rtu" -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SynaptiX-2f-board-2f-mbmaster-2f-port

clean-SynaptiX-2f-board-2f-mbmaster-2f-port:
	-$(RM) ./SynaptiX/board/mbmaster/port/mbportevent.cyclo ./SynaptiX/board/mbmaster/port/mbportevent.d ./SynaptiX/board/mbmaster/port/mbportevent.o ./SynaptiX/board/mbmaster/port/mbportevent.su ./SynaptiX/board/mbmaster/port/mbportother.cyclo ./SynaptiX/board/mbmaster/port/mbportother.d ./SynaptiX/board/mbmaster/port/mbportother.o ./SynaptiX/board/mbmaster/port/mbportother.su ./SynaptiX/board/mbmaster/port/mbportserial.cyclo ./SynaptiX/board/mbmaster/port/mbportserial.d ./SynaptiX/board/mbmaster/port/mbportserial.o ./SynaptiX/board/mbmaster/port/mbportserial.su ./SynaptiX/board/mbmaster/port/mbporttimer.cyclo ./SynaptiX/board/mbmaster/port/mbporttimer.d ./SynaptiX/board/mbmaster/port/mbporttimer.o ./SynaptiX/board/mbmaster/port/mbporttimer.su

.PHONY: clean-SynaptiX-2f-board-2f-mbmaster-2f-port

