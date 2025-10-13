#include "logger.h"
#include "lwip.h"
#include "stdint.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
static char const *TAG = "ETH_APP";
extern struct netif gnetif;

static char taskListBuff[1024];

static void eth_app_task(void *arg);
// #include "mqtt-client-app.h"
#include "dns.h"

void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
	log_info(TAG, "Host %s have ipv4 : %s\r\n", name, ip_ntoa(ipaddr));
	ip_addr_t *ip = (ip_addr_t*) callback_arg;

	ip->addr = ipaddr->addr;

	// mqtt_app_init(ip->addr);

}

void eth_init(void *arg)
{
	bsp_eth_reset_on();
	while(gnetif.ip_addr.addr == 0){
		vTaskDelay(1);
	}
	log_info(TAG, "IP : %s \r\n",ipaddr_ntoa(&gnetif.ip_addr));
	ip_addr_t ipAddress;
	dns_gethostbyname("demo.thingsboard.io", &ipAddress, dns_found_cb, &ipAddress);
	uint32_t timeout = 0;
	while (ipAddress.addr == 0 && timeout++ < 10000) {
		vTaskDelay(1);
	}
   xTaskCreate(eth_app_task, "eth_app_task", 1024, NULL, configMAX_PRIORITIES - 1, NULL);
}

static void eth_app_task(void *arg){
	/* Infinite loop */
	while(1) {
		sys_check_timeouts();
		 size_t freeHeap = xPortGetFreeHeapSize();
		 vTaskList(taskListBuff);
		 printf("***************************************\r\n");
		 printf("Free Heap Size : %u *** IP : %s \r\n", freeHeap,
		 		ipaddr_ntoa(&gnetif.ip_addr));
		 log_info(TAG, "IP : %s \r\n", ipaddr_ntoa(&gnetif.ip_addr));
		 printf("***************************************\r\n");
		 printf("Task\t\tState\tPrio\tStack\tNum\r\n");
		 printf("%s\n", taskListBuff);
		 vTaskDelay(10000);
	}
 /* USER CODE END 5 */
}
