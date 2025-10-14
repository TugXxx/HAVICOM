#include "rtc_rx8130ce.h"

#define RX8130CE_REG_SEC    0x10
#define RX8130CE_REG_MIN    0x11
#define RX8130CE_REG_HOUR   0x12
#define RX8130CE_REG_DAY    0x14
#define RX8130CE_REG_MONTH  0x15
#define RX8130CE_REG_YEAR   0x16

static uint32_t runtimes = 0;

#define CHECK_OK(OP) if (OP != 0) return -1;

static int rx8130ce_begin(iic_dev_handle_t *dev);

static uint8_t bcd_to_dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

static uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

int rx8130ce_init(iic_bus_handle_t *bus, iic_dev_handle_t *dev, uint8_t address,  RTCTime_t *time) {
    CHECK_OK(iic_master_add_dev_to_bus(bus, dev, address, (uint8_t *)time));
    CHECK_OK(rx8130ce_begin(dev));
    return 0;
}

static int rx8130ce_write_reg(iic_dev_handle_t *dev, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buf[len + 1];
    buf[0] = reg;
    for (size_t i = 0; i < len; i++) buf[i + 1] = data[i];
    return iic_master_write(dev, buf, len + 1);
}

static int rx8130ce_write_reg_byte(iic_dev_handle_t *dev, uint8_t reg, uint8_t value) {
    return rx8130ce_write_reg(dev, reg, &value, 1);
}

static int rx8130ce_read_reg(iic_dev_handle_t *dev, uint8_t reg, uint8_t *data, size_t len) {
    if (iic_master_write(dev, &reg, 1) != 0) return -1;
    return iic_master_read(dev, data, len);
}

int rx8130ce_stop(iic_dev_handle_t *dev, bool stop) {
    return rx8130ce_write_reg_byte(dev, 0x1E, stop ? 0x40 : 0x00);
}



static int rx8130ce_begin(iic_dev_handle_t *dev) { 
    /* 
     * Digital offset register:
     *   [7]   DET: 0 ->  disabled
     *   [6:0] L7-L1: 0 -> no offset
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x30, 0x00));
        /* 
     * Extension Register register:
     *   [7:6]   FSEL: 0 ->  0
     *   [5]     USEL: 0 -> 0
     *   [4]     TE: 0 ->
     *   [3]     WADA: 0 -> 0
     *   [2-0]   TSEL: 0 -> 0
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x1C, 0x00));
        /* 
     * Flag Register register:
     *   [7]     VBLF: 0 ->  0
     *   [6]     0: 0 -> 
     *   [5]     UF: 0 -> 
     *   [4]     TF: 0 ->
     *   [3]     AF: 0 -> 0
     *   [2]     RSF: 0 -> 0
     *   [1]     VLF: 0 -> 0
     *   [0]     VBFF: 0 -> 0
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x1D, 0x00));
        /* 
     * Control Register0 register:
     *   [7]     TEST: 0 ->  0
     *   [6]     STOP: 0 -> 
     *   [5]     UIE: 0 -> 
     *   [4]     TIE: 0 ->
     *   [3]     AIE: 0 -> 0
     *   [2]     TSTP: 0 -> 0
     *   [1]     TBKON: 0 -> 0
     *   [0]     TBKE: 0 -> 0
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x1E, 0x00));
        /* 
     * Control Register1 register:
     *   [7-6]   SMPTSEL: 0 ->  0
     *   [5]     CHGEN: 0 -> 
     *   [4]     INIEN: 0 -> 
     *   [3]     0: 0 ->
     *   [2]     RSVSEL: 0 -> 0
     *   [1-0]   BFVSEL: 0 -> 0
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x1F, 0x00));
    CHECK_OK(rx8130ce_stop(dev, false));
        /* 
     * Function register:
     *   [7]   100TH: 0 ->  disabled
     *   [6:5] Periodic interrupt: 0 -> no periodic interrupt
     *   [4]   RTCM: 0 -> real-time clock mode
     *   [3]   STOPM: 0 -> RTC stop is controlled by STOP bit only
     *   [2:0] Clock output frequency: 000 (Default value)
     */
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x28, 0x00));
    // Battery switch register
    CHECK_OK(rx8130ce_write_reg_byte(dev, 0x26, 0x00));
    return 0;
}

int rx8130ce_set_time(iic_dev_handle_t *dev, RTCTime_t t) {
    uint8_t buf[7];
    buf[0] = dec_to_bcd(t.tm_sec) & 0x7F;
    buf[1] = dec_to_bcd(t.tm_min) & 0x7F;
    buf[2] = dec_to_bcd(t.tm_hour) & 0x3F;
    buf[3] = dec_to_bcd(t.tm_wday) & 0x07;
    buf[4] = dec_to_bcd(t.tm_mday) & 0x3F;
    buf[5] = dec_to_bcd(t.tm_mon + 1) & 0x1F;
    buf[6] = dec_to_bcd((t.tm_year + 1900) % 100);

    CHECK_OK(rx8130ce_stop(dev, true));
    CHECK_OK(rx8130ce_write_reg(dev, RX8130CE_REG_SEC, buf, 7));
    CHECK_OK(rx8130ce_stop(dev, false));
    return 0;
}

int rx8130ce_get_time(iic_dev_handle_t *dev, RTCTime_t *t) {
    uint8_t buf[7];
    CHECK_OK(rx8130ce_read_reg(dev, RX8130CE_REG_SEC, buf, 7));

    t->tm_sec  = bcd_to_dec(buf[0] & 0x7F);
    t->tm_min  = bcd_to_dec(buf[1] & 0x7F);
    t->tm_hour = bcd_to_dec(buf[2] & 0x3F);
    t->tm_wday = bcd_to_dec(buf[3] & 0x07);
    t->tm_mday = bcd_to_dec(buf[4] & 0x3F);
    t->tm_mon  = bcd_to_dec(buf[5] & 0x1F) - 1;
    t->tm_year = bcd_to_dec(buf[6]) + 2000;

    return 0;
}

// int rx8130ce_sync_to_system(iic_dev_handle_t *dev) {
//     RTCTime_t rtc_time;
//     if (rx8130ce_get_time(dev, &rtc_time) != 0) return -1;

//     RTCTime_t timeinfo = {
//         .tm_sec  = rtc_time.tm_sec,
//         .tm_min  = rtc_time.tm_min,
//         .tm_hour = rtc_time.tm_hour,
//         .tm_mday = rtc_time.tm_mday,
//         .tm_mon  = rtc_time.tm_mon - 1,
//         .tm_year = rtc_time.tm_year - 1900,
//         .tm_wday = rtc_time.tm_wday,
//     };

//     time_t t = mktime(&timeinfo);
//     struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
//     return settimeofday(&tv, NULL);
// }

bool convert_timestamp_to_rtc(time_t timestamp, RTCTime_t *rtc_time) {
    struct tm timeinfo;
    if (gmtime_r(&timestamp, &timeinfo) == NULL) {
        return false;
    }

    rtc_time->tm_sec  = timeinfo.tm_sec;
    rtc_time->tm_min  = timeinfo.tm_min;
    rtc_time->tm_hour = timeinfo.tm_hour;
    rtc_time->tm_mday = timeinfo.tm_mday;
    rtc_time->tm_mon  = timeinfo.tm_mon + 1;       // tm_mon: 0–11 → RTC: 1–12
    rtc_time->tm_year = timeinfo.tm_year + 1900;   // tm_year: từ 1900
    rtc_time->tm_wday = timeinfo.tm_wday;

    return true;
}