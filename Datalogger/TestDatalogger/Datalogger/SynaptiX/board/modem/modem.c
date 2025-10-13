#include "modem.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"

static const char *TAG = "modem";
void modem_init(Modem_t *modem,int uart,void (*reset)(),void (*power)(bool enable)){
	modem->uart = uart;
	modem->reset = reset;
	modem->power = power;
	modem->index = 0;
	modem->mode = MD_Command_Mode;
	log_info(TAG, "Modem initialized with UART: %p", modem->uart);
}
int modem_send_cmd(Modem_t *modem,Modem_AT_CMD_t *cmd){
	Modem_Status_t res = Modem_CMD_False;
	memset(modem->buff, 0, sizeof(modem->buff));
   modem->index = 0;

	if(modem->mode != MD_Command_Mode) return Modem_CMD_Error_Mode;
	bsp_com_write(modem->uart,(uint8_t*)cmd->cmd, strlen(cmd->cmd));
	bsp_delay(50);
	uint32_t time_tick = 0;
	while(time_tick < cmd->timeout){
		vTaskDelay(pdMS_TO_TICKS(10));
		time_tick+=10;
		int len = bsp_com_read(modem->uart, modem->buff + modem->index,MODEM_BUFFER_SIZE - modem->index);
		modem->index += len;
		if(strstr((char*)modem->buff,cmd->res_ok) != NULL){
			log_info(TAG, "Command response: %s", cmd->res_ok);
			if(cmd->callback != NULL) cmd->callback(cmd->arg,Modem_CMD_Success);
			res = Modem_CMD_Success;
			break;
		}
		else if(strstr((char*)modem->buff,cmd->res_false) != NULL){
			log_error(TAG, "Command response: %s", cmd->res_false);
			if(cmd->callback != NULL) cmd->callback(cmd->arg,Modem_CMD_False);
			res = Modem_CMD_False;
			break;
		}
	}
	if(time_tick >= cmd->timeout){
		log_error(TAG, "Command response timeout: %s", cmd->cmd);
		if(cmd->callback != NULL) cmd->callback(cmd->arg,Modem_CMD_No_Response);
		res = Modem_CMD_No_Response;
	}
	return (int)res;
}
void modem_set_mode(Modem_t *modem,Modem_Mode_t mode){
	modem->mode = mode;
}

Modem_AT_CMD_t modem_at_cmd_init(const char *cmd, const char *res_ok, const char *res_false, void (*callback)(void *arg,int res), void *arg){
	Modem_AT_CMD_t cmd_at = {
		.cmd = cmd,
		.res_ok = res_ok,
		.res_false = res_false,
		.callback = callback,
		.arg = arg,
		.res = 0
	};
	return cmd_at;
}
