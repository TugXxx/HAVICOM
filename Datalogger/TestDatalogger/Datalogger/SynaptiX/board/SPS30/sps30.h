#ifndef SPS30_H
#define SPS30_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "board.h"

#define SPS30_FRAME_START 0x7E
#define SPS30_FRAME_STOP 0x7E

#define SPS30_SHDLC_ADDR 0x00

#define SPS30_UART_BUFF 256

    typedef enum
    {
        SPS30_OK = 0,
        SPS30_NOK = -1,
        SPS30_ERR_COMM = -2,
        SPS30_ERR_CRC = -3,
        SPS30_ERR_STATUS = -4,
        SPS30_ERR_SIZE = -5,
        SPS30_ERR_NULL_HANDLE = -6,
    } SPS30Status;

    typedef enum
    {
        SPS30_START_MEASUREMENT_CMD_ID = 0x0,
        SPS30_STOP_MEASUREMENT_CMD_ID = 0x1,
        SPS30_READ_MEASUREMENT_VALUES_UINT16_CMD_ID = 0x3,
        SPS30_READ_MEASUREMENT_VALUES_FLOAT_CMD_ID = 0x3,
        SPS30_SLEEP_CMD_ID = 0x10,
        SPS30_WAKE_UP_COMMUNICATION_CMD_ID = 0xff,
        SPS30_WAKE_UP_CMD_ID = 0x11,
        SPS30_START_FAN_CLEANING_CMD_ID = 0x56,
        SPS30_READ_AUTO_CLEANING_INTERVAL_CMD_ID = 0x80,
        SPS30_WRITE_AUTO_CLEANING_INTERVAL_CMD_ID = 0x80,
        SPS30_READ_PRODUCT_TYPE_CMD_ID = 0xd0,
        SPS30_READ_SERIAL_NUMBER_CMD_ID = 0xd0,
        SPS30_READ_VERSION_CMD_ID = 0xd1,
        SPS30_READ_DEVICE_STATUS_REGISTER_CMD_ID = 0xd2,
        SPS30_DEVICE_RESET_CMD_ID = 0xd3,
    } SPS30CmdId;

    typedef enum
    {
        SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT = 0x03,  // 0x01 03
        SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_UINT16 = 0x05, // 0x01 05
    } SPS30OutputFormat;

    typedef struct SPS30FrameTx
    {
        uint8_t start;
        uint8_t address;
        uint8_t cmd;
        uint8_t length;
        uint8_t *payload;
        uint8_t CHK;
        uint8_t stop;
    } SPS30FrameTx_t;

    typedef struct SPS30FrameRx
    {
        uint8_t start;
        uint8_t address;
        uint8_t cmd;
        uint8_t state;
        uint8_t length;
        uint8_t *payload;
        uint8_t CHK;
        uint8_t stop;
    } SPS30FrameRx_t;

    typedef struct SPS30
    {
        SPS30FrameTx_t tx;
        SPS30FrameRx_t rx;
        uint8_t buff[SPS30_UART_BUFF];
        int uart_port;
        struct SPS30Version
        {
            uint8_t firmwareMajorVersion;
            uint8_t firmwareMinorVersion;
            uint8_t reserved1;
            uint8_t hardwareRevision;
            uint8_t reserved2;
            uint8_t shdlcMajorVersion;
            uint8_t shdlcMinorVersion;
        } version;
    } SPS30_t;

    int sps30_init(SPS30_t *this, int port);
    int sps30_wakeup_sequence(SPS30_t *this);
    int sps30_start_measurement(SPS30_t *this, SPS30OutputFormat format);
    int sps30_stop_measurement(SPS30_t *this);
    int sps30_sleep(SPS30_t *this);
    int sps30_wakeup_communication(SPS30_t *this);
    int sps30_wakeup(SPS30_t *this);
    int sps30_start_fan_cleaning(SPS30_t *this);
    int sps30_read_auto_cleaning_interval(SPS30_t *this, uint32_t *autoCleaningInterval);
    int sps30_write_auto_cleaning_interval(SPS30_t *this, uint32_t autoCleaningInterval);
    int sps30_read_product_type(SPS30_t *this, char *productType, uint8_t productSize);
    int sps30_read_serial_number(SPS30_t *this, char *serialNumber, uint8_t serialSize);
    int sps30_read_version(SPS30_t *this, struct SPS30Version *version);
    int sps30_read_device_status(SPS30_t *this, bool clearStatusReg, uint32_t *deviceStatusRegister, uint8_t *reserved);
    int sps30_reset(SPS30_t *this);
#ifdef __cplusplus
}
#endif
#endif // SPS30_H