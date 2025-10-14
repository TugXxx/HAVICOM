################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SynaptiX/board/mbmaster/functions/mbmfunccoils.c \
../SynaptiX/board/mbmaster/functions/mbmfunccustom1.c \
../SynaptiX/board/mbmaster/functions/mbmfuncdisc.c \
../SynaptiX/board/mbmaster/functions/mbmfuncfiles.c \
../SynaptiX/board/mbmaster/functions/mbmfuncholding.c \
../SynaptiX/board/mbmaster/functions/mbmfuncinput.c \
../SynaptiX/board/mbmaster/functions/mbmfuncraw.c \
../SynaptiX/board/mbmaster/functions/mbmfuncslaveid.c 

OBJS += \
./SynaptiX/board/mbmaster/functions/mbmfunccoils.o \
./SynaptiX/board/mbmaster/functions/mbmfunccustom1.o \
./SynaptiX/board/mbmaster/functions/mbmfuncdisc.o \
./SynaptiX/board/mbmaster/functions/mbmfuncfiles.o \
./SynaptiX/board/mbmaster/functions/mbmfuncholding.o \
./SynaptiX/board/mbmaster/functions/mbmfuncinput.o \
./SynaptiX/board/mbmaster/functions/mbmfuncraw.o \
./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.o 

C_DEPS += \
./SynaptiX/board/mbmaster/functions/mbmfunccoils.d \
./SynaptiX/board/mbmaster/functions/mbmfunccustom1.d \
./SynaptiX/board/mbmaster/functions/mbmfuncdisc.d \
./SynaptiX/board/mbmaster/functions/mbmfuncfiles.d \
./SynaptiX/board/mbmaster/functions/mbmfuncholding.d \
./SynaptiX/board/mbmaster/functions/mbmfuncinput.d \
./SynaptiX/board/mbmaster/functions/mbmfuncraw.d \
./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.d 


# Each subdirectory must supply rules for building sources it contributes
SynaptiX/board/mbmaster/functions/%.o SynaptiX/board/mbmaster/functions/%.su SynaptiX/board/mbmaster/functions/%.cyclo: ../SynaptiX/board/mbmaster/functions/%.c SynaptiX/board/mbmaster/functions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/utils" -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../LWIP/App -I../LWIP/Target -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/modem" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/A76xx" -I../Drivers/BSP/Components/lan8742 -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/dp83848" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/config" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/mqtt_server" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/cJSON" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/apps/device_state" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/rtc_rx8130ce" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/services/iic_master" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include/internal" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include/common" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/include" -I"D:/Local/Git/HAVICOM/Datalogger/TestDatalogger/Datalogger/SynaptiX/board/mbmaster/port" -I"/Datalogger/SynaptiX/board/mbmaster/rtu" -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SynaptiX-2f-board-2f-mbmaster-2f-functions

clean-SynaptiX-2f-board-2f-mbmaster-2f-functions:
	-$(RM) ./SynaptiX/board/mbmaster/functions/mbmfunccoils.cyclo ./SynaptiX/board/mbmaster/functions/mbmfunccoils.d ./SynaptiX/board/mbmaster/functions/mbmfunccoils.o ./SynaptiX/board/mbmaster/functions/mbmfunccoils.su ./SynaptiX/board/mbmaster/functions/mbmfunccustom1.cyclo ./SynaptiX/board/mbmaster/functions/mbmfunccustom1.d ./SynaptiX/board/mbmaster/functions/mbmfunccustom1.o ./SynaptiX/board/mbmaster/functions/mbmfunccustom1.su ./SynaptiX/board/mbmaster/functions/mbmfuncdisc.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncdisc.d ./SynaptiX/board/mbmaster/functions/mbmfuncdisc.o ./SynaptiX/board/mbmaster/functions/mbmfuncdisc.su ./SynaptiX/board/mbmaster/functions/mbmfuncfiles.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncfiles.d ./SynaptiX/board/mbmaster/functions/mbmfuncfiles.o ./SynaptiX/board/mbmaster/functions/mbmfuncfiles.su ./SynaptiX/board/mbmaster/functions/mbmfuncholding.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncholding.d ./SynaptiX/board/mbmaster/functions/mbmfuncholding.o ./SynaptiX/board/mbmaster/functions/mbmfuncholding.su ./SynaptiX/board/mbmaster/functions/mbmfuncinput.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncinput.d ./SynaptiX/board/mbmaster/functions/mbmfuncinput.o ./SynaptiX/board/mbmaster/functions/mbmfuncinput.su ./SynaptiX/board/mbmaster/functions/mbmfuncraw.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncraw.d ./SynaptiX/board/mbmaster/functions/mbmfuncraw.o ./SynaptiX/board/mbmaster/functions/mbmfuncraw.su ./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.cyclo ./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.d ./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.o ./SynaptiX/board/mbmaster/functions/mbmfuncslaveid.su

.PHONY: clean-SynaptiX-2f-board-2f-mbmaster-2f-functions

