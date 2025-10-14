#ifndef RX8130CE_H
#define RX8130CE_H

#include <stdint.h>
#include <stdbool.h>
#include "iic_master.h"
#include <time.h>  
#include <sys/time.h>

typedef struct tm RTCTime_t;
int rx8130ce_init(iic_bus_handle_t *bus, iic_dev_handle_t *dev, uint8_t address,  RTCTime_t *time);
int rx8130ce_get_time(iic_dev_handle_t *dev, RTCTime_t *time);
int rx8130ce_set_time(iic_dev_handle_t *dev, RTCTime_t time);
// int rx8130ce_sync_to_system(iic_dev_handle_t *dev);
bool convert_timestamp_to_rtc(time_t timestamp, RTCTime_t *rtc_time);

#endif
