#include "A76xx.h"
#include "logger.h"
#include "FreeRTOS.h"
#include "task.h"
static const char *TAG = "A76xx";
#define MODEM_AT_CMD_TIMOUT_mS 5000
#define NUM_RETRY 2

void a76xx_init(A76xx_t *a76xx, int uart, void (*reset)(), void (*power)(bool enable)){
   modem_init(&a76xx->modem, uart, reset, power);
   memset(a76xx->imei, 0, sizeof(a76xx->imei));
   memset(a76xx->ccid, 0, sizeof(a76xx->ccid));
   a76xx->signal_strength = 0;
   a76xx->rssi_level = 0;
   a76xx->ber = 0;
   a76xx->modem.rssi = 0;
   	// Initialize and power on the module
init_retry:
	// if(a76xx_echo(a76xx) != 0){
		a76xx_power_on(a76xx);
		vTaskDelay(1000); // Wait for the module to power on
	// }
   uint32_t timeout = 0;
   // 5. Send startup commands
   while (a76xx_echo(a76xx) != 0) {
       log_error(TAG, "Failed to echo AT command");
       timeout++;
       if (timeout > NUM_RETRY) {
           log_error(TAG, "Timeout waiting for echo");
           goto init_retry;
       }
   }
   timeout = 0;
	// Check IMEI
	while((a76xx_get_imei(a76xx)) == NULL) {
       log_error(TAG, "Failed to get IMEI");
       timeout++;
       if (timeout > NUM_RETRY) {
           log_error(TAG, "Timeout waiting for IMEI");
           goto init_retry;
       }
   }
   // log_info(TAG, "IMEI: %s", a76xx->imei);
   timeout = 0;
   // Check CCID
   while((a76xx_get_ccid(a76xx)) == NULL) {
       log_error(TAG, "Failed to get CCID");
       timeout++;
       if (timeout > NUM_RETRY) {
           log_error(TAG, "Timeout waiting for CCID");
           goto init_retry;
       }
   }
   // log_info(TAG, "CCID: %s", a76xx->ccid);
   timeout = 0;
   // Check RSSI
	check_rssi:
   if (a76xx_get_signal_strength(a76xx) == -1)
		goto init_retry;
	else {
		if (a76xx->rssi_level == 99) {
			// <-107 dBm or unknown
			vTaskDelay(1000);
			goto check_rssi;
		}
	}
   // log_info(TAG, "Signal Strength: -%d dBm, RSSI Level: %d", a76xx->signal_strength, a76xx->rssi_level);
	//AT+CPOS?
   while(NULL == a76xx_get_operator(a76xx)){
       log_error(TAG, "Failed to get operator");
       vTaskDelay(pdMS_TO_TICKS(1000));
       timeout++;
       if (timeout > 5) {
           log_error(TAG, "Timeout waiting for operator");
           goto init_retry;
       }
   }
   timeout = 0;

   if (a76xx->operator[0] == '\0') {
       if(0!=a76xx_set_operator(a76xx)) goto init_retry;
       while(NULL == a76xx_get_operator2(a76xx)){
           log_error(TAG, "Failed to get operator2");
           vTaskDelay(pdMS_TO_TICKS(1000));
           timeout++;
           if (timeout > 5) {
               log_error(TAG, "Timeout waiting for operator2");
               goto init_retry;
           }
       }
   }
   timeout = 0;

   // log_info(TAG, "Operator: %s", a76xx->operator);
   a76xx->apn_config = get_apn(a76xx->operator);
   if (a76xx->apn_config == NULL) {
       log_error(TAG, "APN configuration not found for operator: %s", a76xx->operator);
       goto init_retry;
   }
   log_info(TAG, "APN: %s", a76xx->apn_config->apn);
   // 6. Run modem commands
   	if(0!=a76xx_define_PDP_context(a76xx)) goto init_retry;

   	if(0!=a76xx_authen_PDP_context(a76xx)) goto init_retry;

   	if(0!=a76xx_set_active_PDP_context(a76xx, true)) goto init_retry;;

   	if(0!=a76xx_registration_network(a76xx)) goto init_retry;;

   	if(0!=a76xx_operation_selection(a76xx, 0)) goto init_retry;; // 0: LTE

   	if(0!= a76xx_operation_get_mode(a76xx)) goto init_retry;;

   	if(0!=a76xx_get_ip_context(a76xx)) goto init_retry;;
   a76xx_set_mode(a76xx, MD_Data_Mode);
}
void a76xx_reset(A76xx_t *a76xx){
   if (a76xx->modem.reset) {
       a76xx->modem.reset();
       log_info(TAG, "A76xx modem reset");
   } else {
       log_error(TAG, "Reset function not defined for A76xx modem");
   }
}
void a76xx_power_on(A76xx_t *a76xx){
   if (a76xx->modem.power) {
       a76xx->modem.power(true);
       log_info(TAG, "A76xx modem powered on");
   } else {
       log_error(TAG, "Power function not defined for A76xx modem");
   }
}
void a76xx_power_off(A76xx_t *a76xx){
   if (a76xx->modem.power) {
       a76xx->modem.power(false);
       log_info(TAG, "A76xx modem powered off");
   } else {
       log_error(TAG, "Power function not defined for A76xx modem");
   }
}
int a76xx_echo(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = 2000; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "AT command successful");
       return 0;
   } else {
       log_error(TAG, "AT command failed");
       return -1;
   }
}

void a76xx_set_mode(A76xx_t *a76xx, Modem_Mode_t mode){
   if (mode != a76xx->modem.mode) {
       if(mode == MD_Command_Mode) {
           Modem_AT_CMD_t cmd = modem_at_cmd_init("+++",
               "OK", "ERROR", NULL, NULL);
           cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
           int res = modem_send_cmd(&a76xx->modem, &cmd);
           if (res != Modem_CMD_Success) {
               log_error(TAG, "Failed to switch to Command Mode");
               return;
           }
       }
       else if(mode == MD_Data_Mode) {
           Modem_AT_CMD_t cmd = modem_at_cmd_init("ATD*99#\r\n",
               "CONNECT", "ERROR", NULL, NULL);
           cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
           int res = modem_send_cmd(&a76xx->modem, &cmd);
           if (res != Modem_CMD_Success) {
               log_error(TAG, "Failed to switch to Data Mode");
               return;
           }
       }
       modem_set_mode(&a76xx->modem, mode);
       a76xx->modem.mode = mode;
       log_info(TAG, "A76xx modem mode set to %d", mode);
   } else {
       log_info(TAG, "A76xx modem already in mode %d", mode);
   }
}
int a76xx_get_signal_strength(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CSQ\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       char *start = strstr((char *)a76xx->modem.buff, "+CSQ: ");
       if (start) {
           start += 6; // Skip "+CSQ: "
           int rssi, ber;
           sscanf(start, "%d,%d", &rssi, &ber);
           if(rssi == 0) {
               a76xx->signal_strength = -113; // 0 means -113 dBm
           } else if (rssi == 99) {
               a76xx->signal_strength = 99; // 99 means unknown
           } else if (rssi == 1) {
               a76xx->signal_strength = -111; // -111 dBm
           } else if( rssi == 31 ){
               a76xx->signal_strength = -51; // -51 dBm
           } else {
               a76xx->signal_strength = ((((-53) - (-109)) / 28 )* (rssi-2)) + (-109); // Convert to dBm
           }
           a76xx->ber = ber;
           a76xx->rssi_level = rssi; // Convert to level 0-5
           log_info(TAG, "Signal strength: -%d dBm, RSSI Level: %d", a76xx->signal_strength, a76xx->rssi_level);
           return rssi;
       }
   }
   log_error(TAG, "Failed to get signal strength");
   return -1;
}
char* a76xx_get_imei(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CGSN\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       char *start = strstr((char *)a76xx->modem.buff, "\r\n");
       if (start) {
           start += 2; // Skip "+CGSN: "
           sscanf(start, "%19s", a76xx->imei);
           log_info(TAG, "IMEI: %s", a76xx->imei);
           return a76xx->imei;
       }
   }
   log_error(TAG, "Failed to get IMEI");
   return NULL;
}
char* a76xx_get_ccid(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CICCID\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       char *start = strstr((char *)a76xx->modem.buff, "+ICCID: ");
       if (start) {
           start += 7; // Skip "+ICCID: "
           sscanf(start, "%19s", a76xx->ccid);
           log_info(TAG, "CCID: %s", a76xx->ccid);
           return a76xx->ccid;
       }
   }
   log_error(TAG, "Failed to get CCID");
   return NULL;
}
char* a76xx_get_operator(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CSPN?\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       char *start = strstr((char *)a76xx->modem.buff, "+CSPN: ");
       if (start) {
           start += 7; // Skip "+CSPN: "
           sscanf(start, "\"%s\",", a76xx->operator);
           char *end = strstr(a76xx->operator,"\"");
           end[0] = 0;
           log_info(TAG, "Operator: %s", a76xx->operator);
           return a76xx->operator;
       }
   }
   log_error(TAG, "Failed to get Operator");
   return NULL;
}
int a76xx_set_operator(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+COPS=3,0\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "Operator set successfully");
       return 0;
   } else {
       log_error(TAG, "Failed to set Operator");
       return -1;
   }
}

char* a76xx_get_operator2(A76xx_t *a76xx)
{
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+COPS?\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS;
   int res = modem_send_cmd(&a76xx->modem, &cmd);
       if (res == Modem_CMD_Success) {
           char *start = strstr((char *)a76xx->modem.buff, "+COPS: ");
           if (start) {
               start += 7; // Skip "+COPS: "
               int mode, format;
               char oper[32];
               sscanf(start, "%d,%d,\"%31[^\"]\"", &mode, &format, oper);
               strncpy(a76xx->operator, oper, sizeof(a76xx->operator)-1);
               a76xx->operator[sizeof(a76xx->operator)-1] = '\0'; // Ensure null-termination
               log_info(TAG, "Operator from COPS: %s", a76xx->operator);
               if (a76xx->operator[0] == '\0') {
                   log_error(TAG, "Operator name is empty");
                   return NULL;
               }
               return a76xx->operator;
           }
       }
   log_error(TAG, "Failed to get Operator");
   return NULL;
}

int a76xx_set_sim_host_swap_level(A76xx_t *a76xx, int level){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+AT+UIMHOTSWAPLEVEL=%d\r\n", "OK", "ERROR", NULL, NULL);
   char cmd_str[32];
   snprintf(cmd_str, sizeof(cmd_str), cmd.cmd, level);
   cmd.cmd = cmd_str;
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "SIM host swap level set to %d", level);
       return 0;
   } else {
       log_error(TAG, "Failed to set SIM host swap level");
       return -1;
   }
}
int a76xx_define_PDP_context(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CGDCONT=1,\"IP\",\"%s\"\r\n", "OK", "ERROR", NULL, NULL);
   if (a76xx->apn_config == NULL) {
       log_error(TAG, "APN configuration not set");
       return -1;
   }
   char cmd_str[64];
   snprintf(cmd_str, sizeof(cmd_str), cmd.cmd, a76xx->apn_config->apn);
   cmd.cmd = cmd_str;
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "PDP context defined successfully");
       return 0;
   } else {
       log_error(TAG, "Failed to define PDP context");
       return -1;
   }
}
int a76xx_authen_PDP_context(A76xx_t *a76xx){

   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CGAUTH=1,1,\"%s\",\"%s\"\r\n", "OK", "ERROR", NULL, NULL);
   char cmd_str[64];
   if(a76xx->apn_config->username != NULL && a76xx->apn_config->password != NULL){
       snprintf(cmd_str, sizeof(cmd_str), cmd.cmd, a76xx->apn_config->username, a76xx->apn_config->password);
   }else if(a76xx->apn_config->username != NULL && a76xx->apn_config->password == NULL){
       snprintf(cmd_str, sizeof(cmd_str),"AT+CGAUTH=1,1,\"%s\"\r\n", a76xx->apn_config->username);
   }else if(a76xx->apn_config->username == NULL && a76xx->apn_config->password == NULL){
       snprintf(cmd_str, sizeof(cmd_str),"AT+CGAUTH=1,0\r\n");
   }
   cmd.cmd = cmd_str;
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS*2; // Set a longer timeout for authentication
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "PDP context authenticated successfully");
       return 0;
   } else {
       log_error(TAG, "Failed to authenticate PDP context");
       return -1;
   }
}
int a76xx_set_active_PDP_context(A76xx_t *a76xx, bool active){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CGACT=%d,1\r\n", "OK", "ERROR", NULL, NULL);
   char cmd_str[32];
   snprintf(cmd_str, sizeof(cmd_str), cmd.cmd, active ? 1 : 0);
   cmd.cmd = cmd_str;
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "PDP context %s successfully", active ? "activated" : "deactivated");
       return 0;
   } else {
       log_error(TAG, "Failed to %s PDP context", active ? "activate" : "deactivate");
       return -1;
   }
}
int a76xx_registration_network(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CREG=1\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "Network registration command sent successfully");
       return 0;
   } else {
       log_error(TAG, "Failed to register network");
       return -1;
   }
}
int a76xx_operation_selection(A76xx_t *a76xx, uint8_t mode){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+COPS=%d\r\n", "OK", "ERROR", NULL, NULL);
   char cmd_str[32];
   snprintf(cmd_str, sizeof(cmd_str), cmd.cmd, mode);
   cmd.cmd = cmd_str;
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "Operation selection set to %d successfully", mode);
       return 0;
   } else {
       log_error(TAG, "Failed to set operation selection");
       return -1;
   }
}
int a76xx_operation_get_mode(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+COPS?\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       log_info(TAG, "Operation selection get successfully");
       return 0;
   } else {
       log_error(TAG, "Failed to get operation selection");
       return -1;
   }
}
int a76xx_get_ip_context(A76xx_t *a76xx){
   Modem_AT_CMD_t cmd = modem_at_cmd_init("AT+CGDCONT?\r\n", "OK", "ERROR", NULL, NULL);
   cmd.timeout = MODEM_AT_CMD_TIMOUT_mS; // Set a timeout for the command
   int res = modem_send_cmd(&a76xx->modem, &cmd);
   if (res == Modem_CMD_Success) {
       char *start = strstr((char *)a76xx->modem.buff, "+CGDCONT: ");
       if (start) {
//            start += 10; // Skip "+CGDCONT: "
//            sscanf(start, "+CGDCONT: 1,\"IP\",\"m3_world\",\"%s\",0,0,,,,\r\n\r\n", a76xx->modem.ip);
           log_info(TAG, "IP Context: %s", start);
           return 0;
       }
   }
   log_error(TAG, "Failed to get IP context");
   return -1;
}
