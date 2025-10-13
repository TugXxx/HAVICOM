/*
 * modem.h
 *
 *  Created on: May 23, 2025
 *      Author: phamminhnhan
 */

#ifndef MODEM_MODEM_H_
#define MODEM_MODEM_H_

#include "board.h"
#include <stdbool.h>

#define MODEM_BUFFER_SIZE 4096

typedef enum{
	Modem_CMD_Success = 1,
	Modem_CMD_False = 0,
	Modem_CMD_No_Response = -1,
	Modem_CMD_Error_Mode = -2
}Modem_Status_t;

typedef struct Modem_AT_CMD{
	const char *cmd;
	const char *res_ok;
	const char *res_false;
	void (*callback)(void *arg,int res);
	void *arg;
	int res;
	uint32_t timeout;
}Modem_AT_CMD_t;

typedef enum{
	MD_Command_Mode = 0,
	MD_Data_Mode,
	MD_ERROR
}Modem_Mode_t;

typedef struct Modem{
	int uart;
	void (*reset)();
	void (*power)(bool enable);
	uint8_t buff[MODEM_BUFFER_SIZE];
	int index;
	char ip[32];
	Modem_Mode_t mode;
	int rssi;
	uint8_t rssi_level;
	int ber;
	char IMEI[20];
	char CCID[24];
}Modem_t;

Modem_AT_CMD_t modem_at_cmd_init(const char *cmd, const char *res_ok, const char *res_false, void (*callback)(void *arg,int res), void *arg);
void modem_init(Modem_t *modem,int uart,void (*_reset)(),void (*_power)(bool enable));
int modem_send_cmd(Modem_t *modem,Modem_AT_CMD_t *cmd);
void modem_set_mode(Modem_t *modem,Modem_Mode_t mode);
#endif /* MODEM_MODEM_H_ */
