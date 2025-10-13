#ifndef SHT3x_H_
#define SHT3x_H_

#include "iic_master.h"
#include <stdint.h>
#include <stdbool.h>

// Address I2C
#define SHT3x_I2C_ADDR_1 0x44
#define SHT3x_I2C_ADDR_2 0x45

// Command
#define SHT3x_CMD_SOFT_RESET 0x30A2
#define SHT3x_CMD_HEATER_ENABLE 0x306D
#define SHT3x_CMD_HEATER_DISABLE 0x3066
#define SHT3x_CMD_READ_STATUS 0xF32D
#define SHT3x_CMD_CLEAR_STATUS 0x3041
#define SHT3x_CMD_ART 0x2B32
#define SHT3x_CMD_READ_SN 0x3780

// Single shot
#define SHT3x_CMD_SINGLE_HIGHREP 0x2400
#define SHT3x_CMD_SINGLE_MEDREP 0x240B
#define SHT3x_CMD_SINGLE_LOWREP 0x2416

// Continuous mode
#define SHT3x_CMD_CONT_HIGHREP_05HZ 0x2032
#define SHT3x_CMD_CONT_HIGHREP_1HZ 0x2130
#define SHT3x_CMD_CONT_HIGHREP_2HZ 0x2236
#define SHT3x_CMD_CONT_HIGHREP_4HZ 0x2334
#define SHT3x_CMD_CONT_HIGHREP_10HZ 0x2737

typedef enum
{
    SHT3x_REPEATABILITY_HIGH,
    SHT3x_REPEATABILITY_MEDIUM,
    SHT3x_REPEATABILITY_LOW
} sht3x_repeatability_t;

typedef struct
{
    iic_dev_handle_t dev;
    sht3x_repeatability_t repeatability;
    bool heater_on;
    bool continuous;
} sht3x_handle_t;

int sht3x_init(sht3x_handle_t *sht, iic_bus_handle_t *bus, uint8_t addr);
int sht3x_soft_reset(sht3x_handle_t *sht);
int sht3x_clear_status(sht3x_handle_t *sht);
int sht3x_read_status(sht3x_handle_t *sht, uint16_t *status);
int sht3x_set_heater(sht3x_handle_t *sht, bool enable);
int sht3x_set_repeatability(sht3x_handle_t *sht, sht3x_repeatability_t rep);
int sht3x_start_continuous(sht3x_handle_t *sht, uint16_t cmd);
int sht3x_stop_continuous(sht3x_handle_t *sht);
int sht3x_read_single(sht3x_handle_t *sht, float *temperature, float *humidity);
int sht3x_read_continuous(sht3x_handle_t *sht, float *temperature, float *humidity);
int sht3x_trigger_art(sht3x_handle_t *sht);
int sht3x_read_serial_number(sht3x_handle_t *sht, uint32_t *sn);

#ifndef FREERTOS_EN
#define SHT3x_Delay bsp_delay
#elif
#define SHT3x_Delay vTaskDelay
#endif

#endif // SHT3x_H_
