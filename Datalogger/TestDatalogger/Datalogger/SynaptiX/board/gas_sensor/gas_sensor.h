#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "board.h"

#define GAS_START_FRAME 0xFF
#define GAS_SENSOR_ADDR 0x01

#define GAS_CMD_READ_VALUE 0x86
#define GAS_CMD_ZERO_CALIBRATION 0x87
#define GAS_CMD_SPAN_CALIBRATION 0x88
#define GAS_CMD_SWITCH_MODE 0x78

// Frame sizes
#define GAS_TX_FRAME_SIZE 9
#define GAS_RX_FRAME_SIZE 9

    typedef enum
    {
        GAS_QA_MODE = 0x40,
        GAS_UPLOAD_MODE = 0x41,
    } GasCode_t;

    typedef enum
    {
        GAS_CODE_CO = 0x04,
        GAS_CODE_H2S = 0x03,
        GAS_CODE_SO2 = 0x2B,
        GAS_CODE_NO2 = 0x2C,
        GAS_CODE_O3 = 0x2A,
    } GasMode_t;

    /**
     * @brief TX frame structure (command sent to sensor)
     */
    typedef struct
    {
        uint8_t start;      // 0xFF
        uint8_t addr;       // 0x01
        uint8_t command;    // e.g. 0x86
        uint8_t payload[5]; // usually 0x00-filled
        uint8_t checksum;   // calculated from byte[1] to byte[7]
    } GasTxFrame_t;

    /**
     * @brief RX frame structure (response from sensor)
     */
    typedef struct
    {
        uint8_t start;      // 0xFF
        uint8_t command;    // e.g. 0x86
        uint8_t payload[6]; // includes concentration data
        uint8_t checksum;   // calculated from byte[1] to byte[7]
    } GasRxFrame_t;

    /**
     * @brief Main sensor handle
     */
    typedef struct
    {
        GasCode_t type;
        int uart_port;
        GasTxFrame_t tx;
        GasRxFrame_t rx;
        uint16_t concentration_ppb;  // parsed from payload[5] and [6]
        uint16_t concentration_ugm3; // optional: calculated from ppb
    } GasSensor_t;

    int gas_sensor_init(GasSensor_t *sensor, int port, GasCode_t type);
    int gas_sensor_mode(GasSensor_t *sensor, GasMode_t mode);
    int gas_sensor_read(GasSensor_t *sensor);
    int gas_sensor_read_it(uint8_t *rx_frame, GasSensor_t *temp_sensor);
#ifdef __cplusplus
}
#endif

#endif // GAS_SENSOR_H
