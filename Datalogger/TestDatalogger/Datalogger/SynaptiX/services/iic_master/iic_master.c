
#include "iic_master.h"
#include <logger.h>

static const char *TAG = "iic_master";

int iic_master_bus_init(iic_bus_handle_t *bus, void *handle) {
    if (bus == NULL) {
        log_error(TAG, "iic_master_bus_init || Invalid argument");
        return -1;
    }

    bus->handle = handle;
#ifdef FREERTOS_EN
    bus->bus_lock_mux = xSemaphoreCreateMutex();
        if (bus->bus_lock_mux == NULL) {
            log_error(TAG, "iic_master_bus_init || Failed to create mutex for I2C bus");
            return -1;
        }
#endif
    return 0; 
}

int iic_master_add_dev_to_bus(iic_bus_handle_t *bus, iic_dev_handle_t *dev, uint8_t address, uint8_t *reg) {
    if (bus == NULL || dev == NULL) {
            log_error(TAG, "iic_master_add_dev_to_bus || Invalid argument");
        return -1;
    }
    dev->reg = reg;
    dev->bus_handle = bus;
    dev->address = address;
    return 0;
}

int iic_master_write(iic_dev_handle_t *dev, uint8_t *data, uint16_t size) {
    if (dev == NULL) {
        // log_error(TAG, "iic_master_write || Invalid argument");
        return -1;
    }

    if (dev->bus_handle == NULL)
    {
        // log_error(TAG, "iic_master_write || I2C bus hanhle not initialized ");
        return -1;
    }
#ifdef FREERTOS_EN
    if (dev->bus_handle->bus_lock_mux == NULL)
    {
        // log_error(TAG, "iic_master_write || Mutex created unsuccessfully");
        return -1; 
    }
#endif
    if (dev->bus_handle->handle == NULL)
    {
        return -1; 
    }
    // IIC_Handle_t iic_bsp;
    // iic_bsp.handle = dev->bus_handle->handle;
    // log_info(TAG, "%02X %02X %d", dev->address, *data, size);
#ifdef FREERTOS_EN
    xSemaphoreTake(dev->bus_handle->bus_lock_mux, portMAX_DELAY);
#endif
    int err = bsp_iic_write(&dev->bus_handle->handle, dev->address, data, size);
#ifdef FREERTOS_EN
    xSemaphoreGive(dev->bus_handle->bus_lock_mux);
#endif
    return err;
}

int iic_master_read(iic_dev_handle_t *dev, uint8_t *data, uint16_t size) {
    if (dev == NULL) {
        // log_error(TAG, "iic_master_read || Invalid argument");
        return -1;
    }

    if (dev->bus_handle == NULL)
    {
        // log_error(TAG, "iic_master_read || I2C bus hanhle not initialized ");
        return -1;
    }
#ifdef FREERTOS_EN
    if (dev->bus_handle->bus_lock_mux == NULL)
    {
        // log_error(TAG, "iic_master_read || Mutex created unsuccessfully");
        return -1; 
    }
#endif   
    if (dev->bus_handle->handle == NULL)
    {
        return -1; 
    }
    // IIC_Handle_t iic_bsp;
    // iic_bsp.handle = dev->bus_handle->handle;
#ifdef FREERTOS_EN
    xSemaphoreTake(dev->bus_handle->bus_lock_mux, portMAX_DELAY);
#endif
    int err = bsp_iic_read(&dev->bus_handle->handle, dev->address, data, size);
#ifdef FREERTOS_EN
    xSemaphoreGive(dev->bus_handle->bus_lock_mux);
#endif
    return err;
}