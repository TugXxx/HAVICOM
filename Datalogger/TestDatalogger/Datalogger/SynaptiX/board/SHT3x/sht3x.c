#include "sht3x.h"
#include <string.h>
#include <math.h>

static uint8_t sht3x_crc8(const uint8_t *data, int len);
static int sht3x_write_cmd(sht3x_handle_t *sht, uint16_t cmd);

static uint8_t sht3x_crc8(const uint8_t *data, int len)
{
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;
    for (int i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
        {
            crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
        }
    }
    return crc;
}

static int sht3x_write_cmd(sht3x_handle_t *sht, uint16_t cmd)
{
    uint8_t buf[2] = {(cmd >> 8) & 0xFF, cmd & 0xFF};
    return iic_master_write(&sht->dev, buf, 2);
}

int sht3x_init(sht3x_handle_t *sht, iic_bus_handle_t *bus, uint8_t addr)
{
    if (!sht || !bus)
        return -1;
    memset(sht, 0, sizeof(*sht));
    if (iic_master_add_dev_to_bus(bus, &sht->dev, addr, NULL) != 0)
        return -1;
    sht->repeatability = SHT3x_REPEATABILITY_HIGH;
    return sht3x_soft_reset(sht);
}

int sht3x_soft_reset(sht3x_handle_t *sht)
{
    return sht3x_write_cmd(sht, SHT3x_CMD_SOFT_RESET);
}

int sht3x_clear_status(sht3x_handle_t *sht)
{
    return sht3x_write_cmd(sht, SHT3x_CMD_CLEAR_STATUS);
}

int sht3x_read_status(sht3x_handle_t *sht, uint16_t *status)
{
    uint8_t cmd[2] = {(SHT3x_CMD_READ_STATUS >> 8), (SHT3x_CMD_READ_STATUS & 0xFF)};
    if (iic_master_write(&sht->dev, cmd, 2) != 0)
        return -1;
    uint8_t data[3];
    if (iic_master_read(&sht->dev, data, 3) != 0)
        return -1;
    if (sht3x_crc8(data, 2) != data[2])
        return -2;
    *status = (data[0] << 8) | data[1];
    return 0;
}

int sht3x_set_heater(sht3x_handle_t *sht, bool enable)
{
    sht->heater_on = enable;
    return sht3x_write_cmd(sht, enable ? SHT3x_CMD_HEATER_ENABLE : SHT3x_CMD_HEATER_DISABLE);
}

int sht3x_set_repeatability(sht3x_handle_t *sht, sht3x_repeatability_t rep)
{
    sht->repeatability = rep;
    return 0;
}

int sht3x_start_continuous(sht3x_handle_t *sht, uint16_t cmd)
{
    sht->continuous = true;
    return sht3x_write_cmd(sht, cmd);
}

int sht3x_stop_continuous(sht3x_handle_t *sht)
{
    sht->continuous = false;
    return sht3x_soft_reset(sht);
}

static int sht3x_convert(uint8_t *data, float *t, float *h)
{
    if (sht3x_crc8(data, 2) != data[2])
        return -1;
    if (sht3x_crc8(data + 3, 2) != data[5])
        return -1;
    uint16_t rawT = (data[0] << 8) | data[1];
    uint16_t rawRH = (data[3] << 8) | data[4];
    if (t)
        *t = -45 + 175 * ((float)rawT / 65535.0f);
    if (h)
        *h = 100 * ((float)rawRH / 65535.0f);
    return 0;
}

int sht3x_read_single(sht3x_handle_t *sht, float *temperature, float *humidity)
{
    uint16_t cmd = SHT3x_CMD_SINGLE_HIGHREP;
    if (sht->repeatability == SHT3x_REPEATABILITY_MEDIUM)
        cmd = SHT3x_CMD_SINGLE_MEDREP;
    else if (sht->repeatability == SHT3x_REPEATABILITY_LOW)
        cmd = SHT3x_CMD_SINGLE_LOWREP;

    if (sht3x_write_cmd(sht, cmd) != 0)
        return -1;
    SHT3x_Delay(20);
    uint8_t data[6];
    if (iic_master_read(&sht->dev, data, 6) != 0)
        return -1;
    return sht3x_convert(data, temperature, humidity);
}

int sht3x_read_continuous(sht3x_handle_t *sht, float *temperature, float *humidity)
{
    uint8_t data[6];
    if (iic_master_read(&sht->dev, data, 6) != 0)
        return -1;
    return sht3x_convert(data, temperature, humidity);
}

int sht3x_trigger_art(sht3x_handle_t *sht)
{
    return sht3x_write_cmd(sht, SHT3x_CMD_ART);
}

int sht3x_read_serial_number(sht3x_handle_t *sht, uint32_t *sn)
{
    if (sht3x_write_cmd(sht, SHT3x_CMD_READ_SN) != 0)
        return -1;

    SHT3x_Delay(10);

    uint8_t data[6];
    if (iic_master_read(&sht->dev, data, 6) != 0)
        return -1;

    if (sht3x_crc8(data, 2) != data[2])
        return -2;
    if (sht3x_crc8(data + 3, 2) != data[5])
        return -2;

    if (sn)
    {
        *sn = ((uint32_t)data[0] << 24) |
              ((uint32_t)data[1] << 16) |
              ((uint32_t)data[3] << 8) |
              ((uint32_t)data[4]);
    }
    return 0;
}
