
#include "ppp.h"
#include "pppos.h"

/* ---------------------------- Include system ----------------------------- */
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "main.h"
#include "logger.h"

/* --------------------------- Include flatform ---------------------------- */
#include "modem.h"
#include "board.h"
#include "A76xx.h"

/* ----------------------------- Include LwIP ----------------------------- */
#include "pppapi.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/sio.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"

#include "ppp_impl.h"
//#include "event_app.h"

#include "mqtt_server_app.h"
#include "device_state.h"
/* ----------------------------- Define ----------------------------- */

#define MODEM_SEND_RETRY_TIME        3


#define MAX_TIMEOUT_CHECK_CONNECT_PPPOS_mS (90 * 1000) // Note: This parameter must be greater than THINGSBOARD_KEEP_ALIVE_S
#define UART_MD_BUFF_SIZE 1024
/* ----------------------------- Extern Variables ----------------------------- */

// extern UART_HandleTypeDef huart2;

// int modem_transmit(void *handle,uint8_t *data, uint16_t len){
// 	// HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);
// 	bsp_com_write(a76xx.modem.uart, data, len);
// 	return len;
// }
// UARTHandle_t at_cmd_uart = {.handle = &huart2,.transmit = modem_transmit};
ppp_pcb *ppp;


/* ----------------------------- Static Variables ------------------------------*/
static const char *TAG = "pppos_app";
static TaskHandle_t pppos_task_handle = NULL;
static uint8_t modem_buff[MODEM_BUFFER_SIZE];
static A76xx_t a76xx;
static struct netif pppos_netif;

extern device_config_t sys_config;
/* ----------------------------- Static Functions ----------------------------- */
static void pwr(bool enb);
static void rst();

static void dns_initialize(void);
///* ---------------------------- Callback Functions ---------------------------- */
static void pppos_status_callback(struct netif *netif);
static u32_t ppp_output_cb(ppp_pcb *pcb, const void *data, u32_t len, void *ctx);
static void ppp_link_status_cb(ppp_pcb *pcb, int err_code, void *ctx);
static void ppp_phase_cb(ppp_pcb *pcb, u8_t phase, void *ctx);

/* ----------------------------- Public Functions ----------------------------- */
extern void dns_get_mqtt_host();
extern void mqtt_app_destroy();
extern bool mqtt_app_is_running();
extern void mqtt_app_init();
void pppos_stop();
extern char *modbus_create_json(void);
/* -----------------------------  Start programming ----------------------------- */
void pppos_start() {
	// 1. Initialize TCP/IP stack
	ppp = pppos_create(&pppos_netif, ppp_output_cb, ppp_link_status_cb, NULL);
	if (ppp == NULL) {
		log_error(TAG, "Create ppp false");
		NVIC_SystemReset();
	}
	ppp_set_default(ppp);
//	ppp_set_netif_statuscallback(ppp,pppos_status_callback);
	netif_set_default(&pppos_netif);
	netif_set_up(&pppos_netif);
	ppp_set_notify_phase_callback(ppp, ppp_phase_cb);
	netif_set_link_callback(&pppos_netif, pppos_status_callback);

	ppp_connect(ppp, 0);
}
void pppos_stop() {
	netif_set_link_down(&pppos_netif);
	netif_remove(&pppos_netif);
	ppp_free(ppp);
	ppp = NULL;
}

// #include "thingsboard.h"
// #include "partner_cloud.h"

// char* mqtt_get_last_telemetry_json(void){
// 	static char mqtt_payload[1024];
// 	memset(mqtt_payload,0,1024);
// 	sprintf(mqtt_payload,"{\"humidity\" : 62.6,\"temperature\" : 30.3}");
// 	return mqtt_payload;
// }

void pppos_app_task(void *arg) {
	// Wait for the VBAT input to be powered
	// event_init();
	vTaskDelay(pdMS_TO_TICKS(100));
	// Initialize the UART
	memset(modem_buff, 0, UART_MD_BUFF_SIZE);
	// bsp_uart_init(&at_cmd_uart, &modem_buff[0], UART_MD_BUFF_SIZE);
   // HAL_UART_Receive_IT(at_cmd_uart.handle, &at_cmd_uart.data, 1);
	// tcpip_init(NULL, NULL);
	// Initialize DNS
	dns_initialize();
	// MQTT_APP_INIT


	// strcpy(ServerMQTT_cfg.clientID,"nh49c0wlw6t05gqssmtz" );
	// strcpy(ServerMQTT_cfg.host,"thingsboard.cloud");
	// strcpy(ServerMQTT_cfg.username, "pmzjxj3fntm1sg31gixi");
	// strcpy(ServerMQTT_cfg.password, "28t8ydtkkdiirq1sg1sp");
	// strcpy(ServerMQTT_cfg.clientID,"PMS" );

	// strcpy(ServerMQTT_cfg.host,"27.71.27.73");
	// strcpy(ServerMQTT_cfg.username, "root");
	// strcpy(ServerMQTT_cfg.password, "Iottte135@");



	// strcpy(ServerMQTT_cfg.clientID,"SynaptiX" );
	// strcpy(ServerMQTT_cfg.host,"demo.thingsboard.io");
	// ServerMQTT_cfg.port = 1883;
	// strcpy(ServerMQTT_cfg.username, "TungNX1");
	// strcpy(ServerMQTT_cfg.password, "123456");
	// ServerMQTT_Init(&partner_client, &partner_cfg, NULL);
	// pcloud_init();
	// thingsboard_init();
	
modem_init_retry:
   // Initialize Modem
	device_set_state(REQUEST_NETWORK);
   a76xx_init(&a76xx, BSP_SIM_COM_PORT, rst, pwr);
//	strcpy(sys_config.dev.imei, a76xx.imei);
//	strcpy(sys_config.dev.ccid, a76xx.ccid);
//	sys_config.dev.signalLV = a76xx.rssi_level;

	// Start PPP
	pppos_start();
	uint32_t timer_check_connect = 0;
	mqtt_server_init(&sys_config);
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(10));
		switch (a76xx.modem.mode) {
		case MD_Data_Mode:
			 uint32_t len = bsp_com_read(a76xx.modem.uart, a76xx.modem.buff, MODEM_BUFFER_SIZE);
			 if (len > 0)
			 {
			 	timer_check_connect = 0;
			 	pppos_input(ppp, a76xx.modem.buff, len);
			 }
			//  if(timer_check_connect < MAX_TIMEOUT_CHECK_CONNECT_PPPOS_mS)
			//  	timer_check_connect += 10;
			//  if (timer_check_connect == MAX_TIMEOUT_CHECK_CONNECT_PPPOS_mS){
			//  	log_warn(TAG, "Timeout check connect pppos");
			//  	 netif_set_link_down(&pppos_netif);
			//  	 pppos_input(ppp, "DISCONNECT", strlen("NO CARRIER"));
			//  	ppp_close(ppp,0);
			//  	a76xx.modem.mode = MD_ERROR;
			//  }
			break;
		case MD_Command_Mode:
			 log_info(TAG, "Restart Modem");
			 pppos_stop();
			 goto modem_init_retry;
//			NVIC_SystemReset();
			break;
		case MD_ERROR:
			break;
		default:
			break;
		}
	}
}

void pppos_app_init(void)
{
   log_info(TAG, "Initializing PPPoS application...");
   xTaskCreate(pppos_app_task, "pppos_app_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, &pppos_task_handle);
}

static u32_t ppp_output_cb(ppp_pcb *pcb, const void *data, u32_t len, void *ctx) {
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(ctx);
	// bsp_uart_write(a76xx.modem.uart, (u8_t *)data, len);
	bsp_com_write(a76xx.modem.uart, (u8_t *)data, len);
	// NOTE: Return the number of bytes sent to pppos check if all data was sent
	return len;
}
static void pppos_status_callback(struct netif *netif) {
	log_info(TAG, "pppos status callback");
	if (netif_is_up(netif)) {
		log_info(TAG, "Netif is up");
	} else {
		log_info(TAG, "Netif is down");
	}
}

static void ppp_link_status_cb(ppp_pcb *pcb, int err_code, void *ctx) {
	struct netif *pppif = ppp_netif(pcb);
	LWIP_UNUSED_ARG(ctx);

	switch (err_code) {
	case PPPERR_NONE: /* No error. */
	{
#if LWIP_DNS
		const ip_addr_t *ns;
#endif /* LWIP_DNS */
		log_info(TAG, "ppp_link_status_cb: PPPERR_NONE");
#if LWIP_IPV4
		log_info(TAG, "our_ip4addr = %s", ip4addr_ntoa(netif_ip4_addr(pppif)));
		log_info(TAG, "his_ipaddr  = %s", ip4addr_ntoa(netif_ip4_gw(pppif)));
		log_info(TAG, "netmask     = %s",
				ip4addr_ntoa(netif_ip4_netmask(pppif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
           log_info(TAG, "our_ip6addr = %s", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* LWIP_IPV6 */

#if LWIP_DNS
//        dns_initialize();
		ns = dns_getserver(0);
		log_info(TAG, "dns1        = %s", ipaddr_ntoa(ns));
		ns = dns_getserver(1);
		log_info(TAG, "dns2        = %s", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
           log_info(TAG, "our6_ipaddr = %s", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
//		xEventGroupSetBits(net_event_group, NET_CONNECTED_BIT);
		netif_set_link_up(&pppos_netif);
		device_set_state(CONNECTED_NETWORK);
		// event_set(PPPOS_NET_CONNECTED_BIT);
	}
		break;

	case PPPERR_PARAM: /* Invalid parameter. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_PARAM");
		break;

	case PPPERR_OPEN: /* Unable to open PPP session. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_OPEN");
		break;

	case PPPERR_DEVICE: /* Invalid I/O device for PPP. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_DEVICE");
		//////a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_ALLOC: /* Unable to allocate resources. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_ALLOC");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_USER: /* User interrupt. */
		// event_set(PPPOS_NET_DISCONNECTED_BIT);
		log_info(TAG, "ppp_link_status_cb: PPPERR_USER");
		// pppos_stop();
		a76xx.modem.mode = MD_Command_Mode;
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_CONNECT: /* Connection lost. */
		// event_set(PPPOS_NET_DISCONNECTED_BIT);
		log_info(TAG, "ppp_link_status_cb: PPPERR_CONNECT");
		// pppos_stop();
		a76xx.modem.mode = MD_Command_Mode;
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_AUTHFAIL: /* Failed authentication challenge. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_AUTHFAIL");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_PROTOCOL: /* Failed to meet protocol. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_PROTOCOL");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_PEERDEAD: /* Connection timeout. */
		// event_set(PPPOS_NET_DISCONNECTED_BIT);
		log_info(TAG, "ppp_link_status_cb: PPPERR_PEERDEAD");
		// pppos_stop();
		a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_IDLETIMEOUT: /* Idle Timeout. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_IDLETIMEOUT");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_CONNECTTIME: /* PPPERR_CONNECTTIME. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_CONNECTTIME");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	case PPPERR_LOOPBACK: /* Connection timeout. */
		log_info(TAG, "ppp_link_status_cb: PPPERR_LOOPBACK");
		//a76xx.modem.mode = MD_Command_Mode;
		break;

	default:
		log_info(TAG, "ppp_link_status_cb: unknown errCode %d", err_code);
		//a76xx.modem.mode = MD_Command_Mode;
		break;
	}
}
static void ppp_phase_cb(ppp_pcb *pcb, u8_t phase, void *ctx) {
	static uint8_t repeat = 0;
	printf("[PPP PHASE] Phase changed to %d\r\n", phase);
	switch (phase) {
	case PPP_PHASE_DEAD:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE DEAD");
		break;
	case PPP_PHASE_MASTER:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE MASTER");
		break;
	case PPP_PHASE_HOLDOFF:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE HOLDOFF");
		break;
	case PPP_PHASE_INITIALIZE:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE INITIALIZE");
		break;
	case PPP_PHASE_SERIALCONN:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE SERIALCONN");
		break;
	case PPP_PHASE_DORMANT:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE DORMANT");
		break;
	case PPP_PHASE_ESTABLISH:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE ESTABLISH");
		repeat++;
		if (repeat > 2) {
			repeat = 0;
			a76xx.modem.mode = MD_Command_Mode;
		}
		break;
	case PPP_PHASE_AUTHENTICATE:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE AUTHENTICATE");
		break;
	case PPP_PHASE_CALLBACK:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE CALLBACK");
		break;
	case PPP_PHASE_NETWORK:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE NETWORK");
		break;
	case PPP_PHASE_RUNNING:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE RUNNING");
		repeat = 0;
		break;
	case PPP_PHASE_TERMINATE:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE TERMINATE");
		break;
	case PPP_PHASE_DISCONNECT:
		log_info(TAG, "[PPP PHASE] Phase changed to %s", "PHASE DISCONNECT");
		break;
	}
}


/**
* @brief Controls the power state of the LTE modem module.
*
* This function manages the power on/off sequence for the LTE modem by manipulating
* the PWRKEY_N GPIO pin. When enabling (true), it asserts the pin low for 3 seconds
* to power on the module. When disabling (false), it asserts the pin low for more
* than 2 seconds to power off the module.
*
* @param enb Boolean flag to enable (true) or disable (false) the modem power
*/
static void pwr(bool enb) {
	// After the VBAT input is powered, to switch the module on,
	// the PWRKEY_N interface must be asserted low for 3s and then released

	// When the module is in power-on mode, to turn off the module,
	// the PWRKEY_N pin must be asserted low for greater than 2s and then released.
	if (enb) {
		bsp_sim_power_on();
		vTaskDelay(pdMS_TO_TICKS(3000));
		bsp_sim_power_off();
		return;
	}
}
/**
* @brief Resets the LTE modem module using the RESET_N pin.
*
* This function performs a hardware reset of the N58 modem module by
* generating a negative pulse on the RESET_N pin. When the module is
* powered on, a low pulse lasting more than 50 ms will trigger a module reset.
*/
static void rst() {
	// N58 can be reset with the RESET_N pin. When the module is in power-on mode,
	// inputting a negative pulse for more than 50 ms to RESET_N can reset the module
	bsp_sim_reset_on();
	vTaskDelay(pdMS_TO_TICKS(3000));
	bsp_sim_reset_off();
}

/**
* @brief Initializes the DNS client with predefined DNS server addresses.
*
* This function sets up the DNS resolver by configuring two DNS servers:
* - Primary DNS: 8.8.8.8 (Google Public DNS)
* - Secondary DNS: 8.8.4.4 (Google Public DNS)
*
* The function uses `inet_pton` to convert the string representations of the IP addresses
* to binary form and assigns them to the DNS server slots using `dns_setserver`.
* Finally, it calls `dns_init` to initialize the DNS subsystem.
*/
static void dns_initialize(void) {
	// 8.8.8.8 và 1.1.1.1
	ip_addr_t dns_server;
	inet_pton(AF_INET, "8.8.8.8", &dns_server);
	dns_setserver(0, &dns_server);
	inet_pton(AF_INET, "8.8.4.4", &dns_server);
	dns_setserver(1, &dns_server);
	dns_init();
}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart == at_cmd_uart.handle){
//		bsp_uart_rx_callback(&at_cmd_uart, &at_cmd_uart.data, 1);
//		HAL_UART_Receive_IT(at_cmd_uart.handle, &at_cmd_uart.data, 1);
//	}
//}
