#include "sps30.h"
#include "logger.h"
#include "stdint.h"

static const char *TAG = "SPS30";

#define SPS30_TIME_WAITING 20

static uint8_t s_sps30_generate_crc(uint8_t *data, uint8_t count);
static uint8_t s_sps30_uart_error(uint8_t err);

/**
 * @brief     generate the crc
 * @param[in] *this pointer to an sps30 handle structure
 * @param[in] *data pointer to a data buffer
 * @param[in] len data length
 * @return    crc
 * @note      none
 */
static uint8_t s_sps30_generate_crc(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint32_t crc = 0x00;

    for (i = 0; i < len; i++)
    {
        crc += data[i];
    }

    return (uint8_t)(~(crc & 0xFF));
}

/**
 * @brief     print error
 * @param[in] *this pointer to an sps30 handle structure
 * @param[in] err error code
 * @return    error code
 * @note      none
 */
static uint8_t s_sps30_uart_error(uint8_t err)
{
    switch (err)
    {
    case 0x00:
    {
        break;
    }
    case 0x01:
    {
        log_error(TAG, "wrong data length for this command error.");
        break;
    }
    case 0x02:
    {
        log_error(TAG, "unknown command.");
        break;
    }
    case 0x03:
    {
        log_error(TAG, "no access right for command.");
        break;
    }
    case 0x04:
    {
        log_error(TAG, "illegal command parameter or parameter "
                       "out of allowed range.");
        break;
    }
    case 0x28:
    {
        log_error(TAG, "internal function argument out of range.");
        break;
    }
    case 0x43:
    {
        log_error(TAG, "command not allowed in current state.");
        break;
    }
    default:
    {
        log_error(TAG, "unknown code.");
        break;
    }
    }

    return err;
}

/**
 * @brief     Send command
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[in]  *input pointer to an input buffer
 * @param[in]  in_len input length
 * @return    error
 * @note      none
 */
static int s_sps30_uart_set_tx_frame(SPS30_t *this, uint8_t *input, uint16_t in_len)
{
    uint16_t i;
    uint16_t len;
    memset(this->buff, 0, sizeof(uint8_t) * SPS30_UART_BUFF); /* clear buffer */
    this->buff[0] = input[0];                                 /* set start bit */
    len = 1;                                                  /* set output length */
    for (i = 1; i < (in_len - 1); i++)
    {
        if ((len) >= (SPS30_UART_BUFF - 1)) /* check output length */
        {
            return -1; /* return error */
        }
        if ((input[i] == 0x7E) || (input[i] == 0x7D) || /* check data */
            (input[i] == 0x11) || (input[i] == 0x13))
        {
            switch (input[i]) /* judge input */
            {
            case 0x7E: /* 0x7E */
            {
                this->buff[len] = 0x7D; /* 0x7D */
                (len)++;                /* output length++ */
                this->buff[len] = 0x5E; /* 0x5E */
                (len)++;                /* output length++ */

                break; /* break */
            }
            case 0x7D: /* 0x7D */
            {
                this->buff[len] = 0x7D; /* set 0x7D */
                (len)++;                /* output length++ */
                this->buff[len] = 0x5D; /* set 0x5D */
                (len)++;                /* output length++ */

                break; /* break */
            }
            case 0x11: /* 0x11 */
            {
                this->buff[len] = 0x7D; /* set 0x7D */
                (len)++;                /* output length++ */
                this->buff[len] = 0x31; /* set 0x31 */
                (len)++;                /* output length++ */

                break; /* break */
            }
            case 0x13: /* 0x13 */
            {
                this->buff[len] = 0x7D; /* set 0x7D */
                (len)++;                /* output length++ */
                this->buff[len] = 0x33; /* set 0x33 */
                (len)++;                /* output length++ */

                break; /* break */
            }
            default:
            {
                break; /* break */
            }
            }
        }
        else
        {
            this->buff[len] = input[i]; /* set the buffer */
            (len)++;                    /* output length++ */
        }
    }
    this->buff[len] = input[in_len - 1]; /* set stop bit*/
    (len)++;                             /* copy the end frame */

    return bsp_com_write(this->uart_port, this->buff, len);
}

/**
 * @brief     Receive response
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[out] *output pointer to an output buffer
 * @param[in]  out_len output length
 * @return    error
 * @note      none
 */
static int s_sps30_uart_get_rx_frame(SPS30_t *this, uint8_t *output, uint8_t *out_len)
{
    memset(this->buff, 0, sizeof(uint8_t) * SPS30_UART_BUFF);
    uint16_t len = bsp_com_read(this->uart_port, this->buff, SPS30_UART_BUFF);

    uint16_t i, point;

    output[0] = this->buff[0]; /* save start bit*/ /* set buf[0] */
    point = 1;                                     /* set point 1 */
    for (i = 1; i < (len - 1); i++)                /* run n -2 times */
    {
        if (point >= (out_len - 1)) /* check length */
        {
            return -1; /* return error */
        }
        if (((this->buff[i] == 0x7D) && (this->buff[i + 1]) == 0x5E) || /* check buffer */
            ((this->buff[i] == 0x7D) && (this->buff[i + 1]) == 0x5D) ||
            ((this->buff[i] == 0x7D) && (this->buff[i + 1]) == 0x31) ||
            ((this->buff[i] == 0x7D) && (this->buff[i + 1]) == 0x33))
        {
            switch (this->buff[i + 1]) /* judge */
            {
            case 0x5E: /* 0x5E */
            {
                output[point] = 0x7E; /* set output */
                point++;              /* point++ */
                i++;                  /* i++ */

                break; /* break */
            }
            case 0x5D: /* 0x5D */
            {
                output[point] = 0x7D; /* set output */
                point++;              /* point++ */
                i++;                  /* i++ */

                break; /* break */
            }
            case 0x31: /* 0x31 */
            {
                output[point] = 0x11; /* set output */
                point++;              /* point++ */
                i++;                  /* i++ */

                break; /* break */
            }
            case 0x33: /* 0x33 */
            {
                output[point] = 0x13; /* set output */
                point++;              /* point++ */
                i++;                  /* i++ */

                break; /* break */
            }
            default:
            {
                break; /* break */
            }
            }
        }
        else
        {
            output[point] = this->buff[i]; /* set output */
            point++;                       /* point++ */
        }
    }
    output[point] = this->buff[len - 1]; /* save stop bit*/ /* set the end part */
    point++;                                                /* point++ */
    if (point != out_len)                                   /* check point */
    {
        return -1; /* return error */
    }

    return 0;
}

/**
 * @brief      write read bytes
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[in]  *input pointer to an input buffer
 * @param[in]  in_len input length
 * @param[in]  delay_ms delay time in ms
 * @param[out] *output pointer to an output buffer
 * @param[in]  out_len output length
 * @return     error
 * @note       none
 */
static uint8_t s_sps30_uart_write_read(SPS30_t *this, uint8_t *input, uint16_t in_len,
                                       uint16_t delay_ms, uint8_t *output, uint16_t out_len)
{
    uint16_t len;

    if (s_sps30_uart_set_tx_frame(this, input, in_len) != 0) /* set tx frame */
    {
        return SPS30_ERR_COMM; /* return error */
    }

    bsp_delay(delay_ms); /* delay ms */

    if (s_sps30_uart_get_rx_frame(this, len, output) <= 0) /* get rx frame */
    {
        return SPS30_ERR_COMM; /* return error */
    }

    return 0; /* success return 0 */
}

static uint8_t s_sps30_send_command(SPS30_t *this, uint16_t delay_ms, SPS30CmdId cmd,
                                    uint8_t *in_pay, uint8_t in_pay_len,
                                    uint8_t *out_pay, uint8_t out_pay_len)
{
    int res = SPS30_OK;

    if (this == NULL)
    {
        return SPS30_ERR_NULL_HANDLE;
    }

    if ((in_pay_len > SPS30_UART_BUFF) || (out_pay_len > SPS30_UART_BUFF))
    {
        return SPS30_ERR_SIZE;
    }

    uint8_t input_buf[6 + in_pay_len];
    uint8_t out_buf[7 + out_pay_len];

    input_buf[0] = SPS30_FRAME_START; /* set start */
    input_buf[1] = SPS30_SHDLC_ADDR;  /* set addr */
    input_buf[2] = cmd;               /* set command */
    input_buf[3] = in_pay_len;
    uint8_t index = 4; /* set length */
    if (in_pay != NULL)
    {
        for (uint8_t i = 0; i < in_pay_len; i++)
        {
            input_buf[index] = in_pay[i];
            index++;
        }
    }
    else
    {
        return SPS30_ERR_NULL_HANDLE;
    }

    input_buf[index++] = s_sps30_generate_crc((uint8_t *)&input_buf[1], (3 + in_pay_len));                             /* set crc */
    input_buf[index++] = SPS30_FRAME_STOP;                                                                             /* set stop */
    memset(out_buf, 0, sizeof(uint8_t) * (7 + out_pay_len));                                                           /* clear the buffer */
    res = s_sps30_uart_write_read(this, (uint8_t *)input_buf, index, delay_ms, (uint8_t *)out_buf, (7 + out_pay_len)); /* write read frame */
    if (res != SPS30_OK)                                                                                               /* check result */
    {
        log_error(TAG, "write read failed.");

        return SPS30_ERR_COMM;
    }
    if (out_buf[5 + out_pay_len] != s_sps30_generate_crc((uint8_t *)&out_buf[1], 4 + out_pay_len)) /* check crc */
    {
        log_error(TAG, "crc check error.");

        return SPS30_ERR_CRC;
    }
    if (s_sps30_uart_error(out_buf[3]) != 0) /* check status */
    {
        return SPS30_ERR_STATUS;
    }
    if ((out_pay != NULL) && (out_pay_len > 0))
    {
        memcpy(out_pay, &out_buf[5], out_pay_len);
    }

    return SPS30_OK;
}
int sps30_init(SPS30_t *this, int port)
{
    if (this == NULL)
        return -1;
    memset(this, 0, sizeof(SPS30_t));
    this->uart_port = port;
    return 0;
}

int sps30_wakeup_sequence(SPS30_t *this)
{
    int localError = 0;
    localError = sps30_wakeup_communication(this);
    if (localError != SPS30_OK)
    {
        return localError;
    }
    localError = sps30_wakeup(this);
    return localError;
}

int sps30_start_measurement(SPS30_t *this, SPS30OutputFormat format)
{
    uint8_t payload[2] = {0x01, (uint8_t)format};
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_START_MEASUREMENT_CMD_ID, payload, 2, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_start_measurement failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      Stop measurement on SPS30 sensor
 * @param[in]  this  Pointer to SPS30 handle structure
 * @return     Status code:
 */
int sps30_stop_measurement(SPS30_t *this)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_STOP_MEASUREMENT_CMD_ID, NULL, 0, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_stop_measurement failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      Put SPS30 sensor into sleep mode
 * @param[in]  this  Pointer to SPS30 handle structure
 * @return     Status code:
 */
int sps30_sleep(SPS30_t *this)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_SLEEP_CMD_ID, NULL, 0, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_sleep failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

int sps30_wakeup_communication(SPS30_t *this)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_WAKE_UP_COMMUNICATION_CMD_ID, NULL, 0, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_wakeup_communication failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}
/**
 * @brief      Wake up SPS30 sensor from sleep mode
 * @param[in]  *this  Pointer to SPS30 handle structure
 * @return     Status code:
 */
int sps30_wake_up(SPS30_t *this)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_WAKE_UP_CMD_ID, NULL, 0, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_wake_up failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief     start the fan cleaning
 * @param[in] *this pointer to an sps30 handle structure
 * @return    status code
 * @note      none
 */
int sps30_start_fan_cleaning(SPS30_t *this)
{
    uint8_t payload = 0x00;
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_START_FAN_CLEANING_CMD_ID, &payload, 1, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_start_fan_cleaning failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      get the auto cleaning interval
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[out] *autoCleaningInterval pointer to an interval buffer
 * @return     status code
 * @note       none
 */
int sps30_read_auto_cleaning_interval(SPS30_t *this, uint32_t *autoCleaningInterval)
{
    uint8_t payload = 0x00;
    uint8_t read_pay[4];
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_READ_AUTO_CLEANING_INTERVAL_CMD_ID, &payload, 1, read_pay, 4) != SPS30_OK)
    {
        log_error(TAG, "sps30_read_auto_cleaning_interval failed!");
        return SPS30_NOK;
    }
    *autoCleaningInterval = ((uint32_t)read_pay[0] << 24) | ((uint32_t)read_pay[1] << 16) |
                            ((uint32_t)read_pay[2] << 8) | ((uint32_t)read_pay[3] << 0);
    return SPS30_OK;
}

/**
 * @brief     set the auto cleaning interval.
 * @param[in] *this pointer to an sps30 handle structure
 * @param[in] autoCleaningInterval interval
 * @return    status code
 * @note      10 <= second <= 604800, second = 0 to disable
 */
int sps30_write_auto_cleaning_interval(SPS30_t *this, uint32_t autoCleaningInterval)
{
    uint8_t payload[5];
    payload[0] = 0x00;
    payload[1] = (autoCleaningInterval >> 24) & 0xFF; /* set 32 - 24 bits */
    payload[2] = (autoCleaningInterval >> 16) & 0xFF; /* set 24 - 16 bits */
    payload[3] = (autoCleaningInterval >> 8) & 0xFF;  /* set 16 - 8 bits */
    payload[4] = (autoCleaningInterval >> 0) & 0xFF;
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_WRITE_AUTO_CLEANING_INTERVAL_CMD_ID, &payload, 5, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_write_auto_cleaning_interval failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      get the product type
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[out] *productType pointer to a serial number buffer,
 * @param[in]  *productSize of serial number buffer, it is 9.
 * @return     status code
 * @note       none
 */
int sps30_read_product_type(SPS30_t *this, char *productType, uint8_t productSize)
{
    uint8_t payload = 0x00; // 0x00: Product Type
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_READ_PRODUCT_TYPE_CMD_ID, &payload, 1, (uint8_t *)productType, productSize) != SPS30_OK)
    {
        log_error(TAG, "sps30_read_product_type failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      get the serial number
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[out] *serialNumber pointer to a serial number buffer,
 * @param[in]  *serialSize of serial number buffer, it is 17 or 21.
 * @return     status code
 * @note       none
 */
int sps30_read_serial_number(SPS30_t *this, char *serialNumber, uint8_t serialSize)
{
    uint8_t payload = 0x03; // 0x03: Serial Number
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_READ_SERIAL_NUMBER_CMD_ID, &payload, 1, (uint8_t *)serialNumber, serialSize) != SPS30_OK)
    {
        log_error(TAG, "sps30_read_serial_number failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      get the version
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[out] *version pointer to a major buffer
 * @return     status code
 * @note       none
 */
int sps30_read_version(SPS30_t *this, struct SPS30Version *version)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_READ_VERSION_CMD_ID, NULL, 0, (uint8_t *)version, 7) != SPS30_OK)
    {
        log_error(TAG, "sps30_read_version failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}

/**
 * @brief      get the device status
 * @param[in]  *this pointer to an sps30 handle structure
 * @param[in]  *clearStatusReg to clear any bit in the Device Status Register after reading.
 * @param[out] *deviceStatusRegister pointer to a deviceStatusRegister buffer
 * @return     status code
 * @note       none
 */
int sps30_read_device_status(SPS30_t *this, bool clearStatusReg, uint32_t *deviceStatusRegister, uint8_t *reserved)
{
    uint8_t payload = 0x00;
    if (clearStatusReg == true)
        payload = 0x01;

    uint8_t pay_read[5];
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_READ_DEVICE_STATUS_REGISTER_CMD_ID, &payload, 1, pay_read, 5) != SPS30_OK)
    {
        log_error(TAG, "sps30_read_device_status failed!");
        return SPS30_NOK;
    }
    *deviceStatusRegister = ((uint32_t)pay_read[0] << 24) | ((uint32_t)pay_read[1] << 16) |
                            ((uint32_t)pay_read[2] << 8) | ((uint32_t)pay_read[3] << 0);
    *reserved = pay_read[4];
    return SPS30_OK;
}

/**
 * @brief     reset the chip
 * @param[in] *this pointer to an sps30 handle structure
 * @return    status code
 * @note      none
 */
int sps30_reset(SPS30_t *this)
{
    if (s_sps30_send_command(this, SPS30_TIME_WAITING, SPS30_DEVICE_RESET_CMD_ID, NULL, 0, NULL, 0) != SPS30_OK)
    {
        log_error(TAG, "sps30_reset failed!");
        return SPS30_NOK;
    }
    return SPS30_OK;
}