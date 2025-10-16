#include "app.h"
#include "board.h"
#include <stdio.h>
#include "app_config.h"
#include "button.h"
#include "logger.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "device_state.h"
#include "config_manager.h"
//#include "rtc_app.h"
//#include "modbus_rtu_app.h"
#include "user_mb_app.h"
//static const char *TAG = "APP";

extern void shell_app_init();
extern void shell_app_poll();
extern void user_app();
extern void pppos_app_init();
extern void eth_init();

device_config_t sys_config;

void app_init()
{
   bsp_init();
   device_state_init();
   hmi_app_init();
   // rtc_app_init();
   // eth_init();
   // mbm_rtu_app_init();
   pppos_app_init();
#if BUTTON_ENABLE
   // button_init();
#endif
}

//static char taskListBuff[1024];
void app_poll()
{

#if BUTTON_ENABLE
   // button_poll(1);
#endif
//		size_t freeHeap = xPortGetFreeHeapSize();
//		vTaskList(taskListBuff);
//		printf("***************************************\r\n");
//		printf("Free Heap Size : %u \r\n", freeHeap);
//		printf("***************************************\r\n");
//		printf("Task\t\tState\tPrio\tStack\tNum\r\n");
//		printf("%s\n", taskListBuff);
//		vTaskDelay(10);

}
