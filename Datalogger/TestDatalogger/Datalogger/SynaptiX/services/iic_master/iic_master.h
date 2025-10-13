#ifndef IIC_MASTER_H_
#define IIC_MASTER_H_

// #include "bsp_iic.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef FREERTOS_EN
// #include "FreeRTOS.h"
// #include "semphr.h"
#endif

typedef struct {
#ifdef FREERTOS_EN
    SemaphoreHandle_t bus_lock_mux;
#endif
    void *handle;
    size_t device_count;
} iic_bus_handle_t;

typedef struct {
    iic_bus_handle_t *bus_handle;
    uint8_t address;
    uint8_t *reg;
} iic_dev_handle_t;

int iic_master_bus_init(iic_bus_handle_t *bus, void *handle);
int iic_master_add_dev_to_bus(iic_bus_handle_t *bus, iic_dev_handle_t *dev, uint8_t address, uint8_t *reg);
int iic_master_write(iic_dev_handle_t *dev, uint8_t *data, uint16_t size);
int iic_master_read(iic_dev_handle_t *dev, uint8_t *data, uint16_t size);
#endif // IIC_MASTER_H_