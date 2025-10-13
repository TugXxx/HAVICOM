#include "gas_sensor.h"
#include <string.h>

/**
 * @brief Calculate checksum for ZE12 frame
 *        Checksum = ~(sum of byte[1] to byte[7]) + 1
 * @param[in] data Pointer to frame buffer
 * @param[in] len Length of frame (usually 9)
 * @return checksum byte
 */
static uint8_t s_gas_sensor_calculate_checksum(const uint8_t *data, uint8_t len)
{
    uint8_t sum = 0;
    for (uint8_t i = 1; i < len - 1; i++)
    {
        sum += data[i];
    }
    return (~sum) + 1;
}

/**
 * @brief Build a TX frame with specified command
 *        Payload is filled with zeros by default
 * @param[in] sensor Pointer to sensor handle
 * @param[in] command Command byte to send
 */
static void s_gas_sensor_build_tx_frame(GasSensor_t *sensor, uint8_t command)
{
    sensor->tx.start = GAS_START_FRAME;
    sensor->tx.addr = GAS_SENSOR_ADDR;
    sensor->tx.command = command;
    memset(sensor->tx.payload, 0x00, sizeof(sensor->tx.payload));

    uint8_t temp[8];
    temp[0] = sensor->tx.start;
    temp[1] = sensor->tx.addr;
    temp[2] = sensor->tx.command;
    memcpy(&temp[3], sensor->tx.payload, 5);

    sensor->tx.checksum = s_gas_sensor_calculate_checksum(temp, 8);
}

int gas_sensor_init(GasSensor_t *sensor, int port, GasCode_t type)
{
    if (sensor == NULL)
        return -1;
    memset(sensor, 0, sizeof(GasSensor_t));
    sensor->uart_port = port;
    sensor->type = type;
    return 0;
}

/**
 * @brief Send read command and parse gas concentration from response
 * @param[in] sensor Pointer to sensor handle
 * @param[in] bsp_com_write Function to send UART data
 * @param[in] uart_read Function to receive UART data
 * @return 0 on success, -1 on failure
 */
int gas_sensor_read(GasSensor_t *sensor)
{
    s_gas_sensor_build_tx_frame(sensor, GAS_CMD_READ_VALUE);

    uint8_t tx_buf[9] = {
        sensor->tx.start,
        sensor->tx.addr,
        sensor->tx.command,
        sensor->tx.payload[0],
        sensor->tx.payload[1],
        sensor->tx.payload[2],
        sensor->tx.payload[3],
        sensor->tx.payload[4],
        sensor->tx.checksum};

    if (bsp_com_write(sensor->uart_port, tx_buf, 9) != 0)
    {
        return -1; // UART write failed
    }

    uint8_t rx_buf[9] = {0};
    if (bsp_com_read(sensor->uart_port, rx_buf, 9) <= 0)
    {
        return -1; // UART read failed
    }

    // Validate response frame
    if (rx_buf[0] != GAS_START_FRAME || rx_buf[1] != GAS_CMD_READ_VALUE)
    {
        return -1; // Invalid start or command byte
    }

    uint8_t calc_crc = s_gas_sensor_calculate_checksum(rx_buf, 8);
    if (calc_crc != rx_buf[8])
    {
        return -1; // CRC mismatch
    }

    // Extract gas concentration (ppb) from payload
    uint16_t high = rx_buf[6];
    uint16_t low = rx_buf[7];
    sensor->concentration_ppb = (high << 8) | low;
    // Extract gas concentration (ugm3) from payload
    high = rx_buf[2];
    low = rx_buf[3];
    sensor->concentration_ugm3 = (high << 8) | low;

    return 0;
}

/**
 * @brief Switch sensor to question-answer mode
 * @param[in] sensor Pointer to sensor handle
 * @param[in] mode to switch mode
 * @return 0 on success, -1 on failure
 */
int gas_sensor_mode(GasSensor_t *sensor, GasMode_t mode)
{
    s_gas_sensor_build_tx_frame(sensor, GAS_CMD_SWITCH_MODE);
    sensor->tx.payload[0] = (uint8_t)mode;

    uint8_t tx_buf[9] = {
        sensor->tx.start,
        sensor->tx.addr,
        sensor->tx.command,
        sensor->tx.payload[0],
        sensor->tx.payload[1],
        sensor->tx.payload[2],
        sensor->tx.payload[3],
        sensor->tx.payload[4],
        sensor->tx.checksum};

    return bsp_com_write(sensor->uart_port, tx_buf, 9);
}

int gas_sensor_read_it(uint8_t *rx_frame, GasSensor_t *temp_sensor)
{
    if (rx_frame[0] != 0xFF || rx_frame[2] != 0x04)
        return -1; // Check start & unit

    uint8_t calc_crc = gas_sensor_calculate_checksum(rx_frame, 9);
    if (calc_crc != rx_frame[8])
        return -2; // CRC mismatch

    uint16_t high = rx_frame[4];
    uint16_t low = rx_frame[5];
    temp_sensor->concentration_ppb = (high << 8) | low;
    temp_sensor->type = (GasCode_t)rx_frame[1];
    return 0;
}
