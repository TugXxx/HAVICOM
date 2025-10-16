#include "rtc_app.h"
#include "iic_master.h"
#include "rtc_rx8130ce.h"
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include <time.h>

static const char *TAG = "rtc_app";
static iic_bus_handle_t bus;
static iic_dev_handle_t rtc_dev;
RTCTime_t current_time;

static void rtc_task(void *arg);

void rtc_app_init(void)
{
    log_info(TAG, "Initializing RTC application...");
    xTaskCreate(rtc_task, "rtc_task", 256, NULL, 1, NULL);
}

static void rtc_task(void *arg)
{
    (void)arg;

    iic_master_bus_init(&bus, 0);

    while (rx8130ce_init(&bus, &rtc_dev, 0x32 << 1, NULL) != 0)
    {
        log_info(TAG, "RTC init failed\n"); 
        vTaskDelay(1000);
    }

    RTCTime_t set_time = {
        .tm_sec  = 0,
        .tm_min  = 15,
        .tm_hour = 17,
        .tm_mday = 16,
        .tm_mon  = 10,    // January (0-11)
        .tm_year = 2025,  // Year since 1900 (2024-1900)
    };

    rx8130ce_set_time(&rtc_dev, set_time);
    // rx8130ce_sync_to_system(&rtc_dev);
    
    while (1)
    {
        if (rx8130ce_get_time(&rtc_dev, &current_time) == 0)
        {
            log_info(TAG, "Current RTC Time: %04d-%02d-%02d %02d:%02d:%02d",
                     current_time.tm_year,
                     current_time.tm_mon,
                     current_time.tm_mday,
                     current_time.tm_hour,
                     current_time.tm_min,
                     current_time.tm_sec);
        }
        else
        {
            log_error(TAG, "Failed to get RTC time");
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds
    }
}
