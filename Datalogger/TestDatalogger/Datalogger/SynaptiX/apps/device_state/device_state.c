#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "logger.h"
#include <stdbool.h>
#include "device_state.h"

static Device_State_t device_state;

SemaphoreHandle_t device_mutex = NULL;
void device_state_init(){
    device_mutex = xSemaphoreCreateMutex();
    device_state = POWER_ON;
}
void device_set_state(Device_State_t state){
    if(device_mutex == NULL) return;
    xSemaphoreTake(device_mutex,portMAX_DELAY);
    device_state = state;
    xSemaphoreGive(device_mutex);
}
Device_State_t device_get_state(){
    return device_state;
}
