APPS_SOURCE = ${SYNAPTIXDIR}/apps/app.c\
			  ${SYNAPTIXDIR}/apps/app_settings.c\
			  ${SYNAPTIXDIR}/apps/cli_shell_command.c\
			  ${SYNAPTIXDIR}/apps/user_app.c\
			  ${SYNAPTIXDIR}/apps/shell_app.c\

SERVICES_SOURCE = ${SYNAPTIXDIR}/services/logger.c\
				  ${SYNAPTIXDIR}/services/shell/cli_shell.c\


UTILS_SOURCE = ${SYNAPTIXDIR}/utils/cqueue.c 

BOARD_SOURCE = ${SYNAPTIXDIR}/board/board.c \
			   ${SYNAPTIXDIR}/board/button.c

RF_SOURCE	= 	${SYNAPTIXDIR}/services/zigbee/zigbee.c \
				${SYNAPTIXDIR}/services/lora/lora.c

MODBUS_SOURCE =	${SYNAPTIXDIR}/services/Modbus/modbus/ascii/mbascii.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/rtu/mbcrc.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/rtu/mbrtu.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfunccoils.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfuncdiag.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfuncdisc.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfuncholding.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfuncinput.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbfuncother.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/functions/mbutils.c \
				${SYNAPTIXDIR}/services/Modbus/modbus/mb.c \
				${SYNAPTIXDIR}/services/mbport/portevent.c \
				${SYNAPTIXDIR}/services/mbport/portserial.c \
				${SYNAPTIXDIR}/services/mbport/porttimer.c \
				${SYNAPTIXDIR}/apps/user_mb_app.c

DEVICE_INCLUDE = ${SYNAPTIXDIR}/apps \
				 ${SYNAPTIXDIR}/board \
				 ${SYNAPTIXDIR}/services \
				 ${SYNAPTIXDIR}/utils

DEVICE_SOURCE = ${APPS_SOURCE}\
				${SERVICES_SOURCE}\
				${UTILS_SOURCE}\
				${BOARD_SOURCE}