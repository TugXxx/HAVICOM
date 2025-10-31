#include "user_mb_app.h"

#if (MB_ASCII_ENABLED > 0 || MB_RTU_ENABLED > 0 || MB_TCP_ENABLED > 0)

/*------------------------Slave mode use these variables----------------------*/

//Slave mode:DiscreteInputs variables
#if S_DISCRETE_INPUT_NDISCRETES > 0
USHORT   usSDiscInStart                               = S_DISCRETE_INPUT_START;
#if S_DISCRETE_INPUT_NDISCRETES%8
UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8]  ;
#endif

UCHAR *input_coils = ucSDiscInBuf;

#endif



//Slave mode:Coils variables
#if S_COIL_NCOILS > 0
USHORT   usSCoilStart                                 = S_COIL_START;
#if S_COIL_NCOILS%8
UCHAR    ucSCoilBuf[S_COIL_NCOILS/8+1]                ;
#else
UCHAR    ucSCoilBuf[S_COIL_NCOILS/8]                  ;
#endif

UCHAR *output_coils = ucSCoilBuf;

#endif

//Slave mode:InputRegister variables
#if S_REG_INPUT_NREGS > 0
USHORT   usSRegInStart                                = S_REG_INPUT_START;
USHORT   usSRegInBuf[S_REG_INPUT_NREGS]               ;

USHORT *input_reg = usSRegInBuf;

#endif

//Slave mode:HoldingRegister variables
#if S_REG_HOLDING_NREGS > 0
USHORT   usSRegHoldStart                              = S_REG_HOLDING_START;
USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS]           ;

USHORT *hoding_reg = usSRegHoldBuf;
#endif
/*------------------------Slave user code----------------------*/

/*------------------------Slave registers callback function----------------------*/

/**
 * Modbus slave input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
#if S_REG_INPUT_NREGS > 0
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegInputBuf;
    USHORT          REG_INPUT_START;
    USHORT          REG_INPUT_NREGS;
    USHORT          usRegInStart;

    pusRegInputBuf = usSRegInBuf;
    REG_INPUT_START = S_REG_INPUT_START;
    REG_INPUT_NREGS = S_REG_INPUT_NREGS;
    usRegInStart = usSRegInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
        iRegIndex = usAddress - usRegInStart;
        while (usNRegs > 0)
        {
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR) (pusRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
#else
	return MB_ENOREG;
#endif
}

/**
 * Modbus slave holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
#if S_REG_HOLDING_NREGS > 0
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegHoldingBuf;
    USHORT          REG_HOLDING_START;
    USHORT          REG_HOLDING_NREGS;
    USHORT          usRegHoldStart;

    pusRegHoldingBuf = usSRegHoldBuf;
    REG_HOLDING_START = S_REG_HOLDING_START;
    REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
    usRegHoldStart = usSRegHoldStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (UCHAR) (pusRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;

        /* write current register values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (usNRegs > 0)
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
#else
	return MB_ENOREG;
#endif
}

/**
 * Modbus slave coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
#if S_COIL_NCOILS > 0
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucCoilBuf;
    USHORT          COIL_START;
    USHORT          COIL_NCOILS;
    USHORT          usCoilStart;
    iNReg =  usNCoils / 8 + 1;

    pucCoilBuf = ucSCoilBuf;
    COIL_START = S_COIL_START;
    COIL_NCOILS = S_COIL_NCOILS;
    usCoilStart = usSCoilStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if( ( usAddress >= COIL_START ) && ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
        iRegIndex = (USHORT) (usAddress - usCoilStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usCoilStart) % 8;
        switch ( eMode )
        {
        /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],
                        iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            /* last coils */
            usNCoils = usNCoils % 8;
            /* filling zero to high bit */
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

            /* write current coil values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                        *pucRegBuffer++);
                iNReg--;
            }
            /* last coils */
            usNCoils = usNCoils % 8;
            /* xMBUtilSetBits has bug when ucNBits is zero */
            if (usNCoils != 0)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                        *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
#else
	return MB_ENOREG;
#endif
}

/**
 * Modbus slave discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
#if S_DISCRETE_INPUT_NDISCRETES > 0
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucDiscreteInputBuf;
    USHORT          DISCRETE_INPUT_START;
    USHORT          DISCRETE_INPUT_NDISCRETES;
    USHORT          usDiscreteInputStart;
    iNReg =  usNDiscrete / 8 + 1;

    pucDiscreteInputBuf = ucSDiscInBuf;
    DISCRETE_INPUT_START = S_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = S_DISCRETE_INPUT_NDISCRETES;
    usDiscreteInputStart = usSDiscInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= DISCRETE_INPUT_START) && (usAddress + usNDiscrete    <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        iRegIndex = (USHORT) (usAddress - usDiscreteInputStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usDiscreteInputStart) % 8;

        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++],
                    iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* filling zero to high bit */
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
#else
	return MB_ENOREG;
#endif
}
#include "board.h"
#include "mbutils.h"
#include "app_config.h"
#include "FreeRTOS.h"
#include "task.h"


// Baseline cho nhà thi đấu/hội chợ triển lãm
uint16_t baseline[6] = {
    18,   // PM2.5 (µg/m³)
    30,   // PM10  (µg/m³)
    450,  // CO    (ppb)
    10,   // SO₂   (ppb)
    20,   // NO₂   (ppb)
    20    // O₃    (ppb)
};

// Biên dao động ± cho từng chỉ số
uint16_t delta[6] = {
    1,    // PM2.5
    1,    // PM10
    2, //30,   // CO
    1,    // SO₂
    1,    // NO₂
    1     // O₃
};

static uint16_t rand_int(uint16_t min, uint16_t max) {
    return min + rand() % (max - min + 1);
}

static void generate_air_data(uint16_t *air_data, uint16_t i) {
        uint16_t min = baseline[i] - delta[i];
        uint16_t max = baseline[i] + delta[i];
        *air_data = rand_int(min, max);
}

typedef struct {
    uint16_t concentration;   // Giá trị đo được
    const char* name;    // Tên chất
    uint16_t breakpoints[7];  // Ngưỡng nồng độ
    uint16_t aqi_levels[7];   // Ngưỡng AQI tương ứng
} Pollutant;

// Tính AQI theo công thức tuyến tính giữa các ngưỡng
uint16_t calculate_aqi(uint16_t C, const uint16_t* bp, const uint16_t* aqi) {
    for (uint8_t i = 0; i < 6; i++) {
        if (C <= bp[i + 1]) {
            uint16_t Clow = bp[i];
            uint16_t Chigh = bp[i + 1];
            uint16_t Ilow = aqi[i];
            uint16_t Ihigh = aqi[i + 1];
            return ((Ihigh - Ilow) * (C - Clow)) / (Chigh - Clow) + Ilow;
        }
    }
    return aqi[6]; // Nếu vượt ngưỡng cao nhất
}

Pollutant pollutants[] = {
    {0, "PM2.5", {0, 12, 35, 55, 150, 250, 500}, {0, 50, 100, 150, 200, 300, 500}},
    {0, "PM10",  {0, 54, 154, 254, 354, 424, 604}, {0, 50, 100, 150, 200, 300, 500}},
    {0, "CO",    {0, 4, 9, 12, 15, 30, 50},        {0, 50, 100, 150, 200, 300, 500}}, // ppm
    {0, "SO2",   {0, 35, 75, 185, 304, 604, 1004}, {0, 50, 100, 150, 200, 300, 500}}, // ppb
    {0, "NO2",   {0, 53, 100, 360, 649, 1249, 2049}, {0, 50, 100, 150, 200, 300, 500}}, // ppb
    {0, "O3",    {0, 54, 70, 85, 105, 200, 400},   {0, 50, 100, 150, 200, 300, 500}} // ppb (8h)
};

typedef struct {
	uint16_t aqi;
    const char* pollutant;
} AQI_Result;

AQI_Result compute_aqi_us(uint16_t pm25, uint16_t pm10, uint16_t co_ppb, uint16_t so2, uint16_t no2, uint16_t o3) {
    uint16_t max_aqi = 0;
    const char* main_pollutant = "";

    // Gán giá trị đo vào cấu trúc
    pollutants[0].concentration = pm25;
    pollutants[1].concentration = pm10;
    pollutants[2].concentration = co_ppb / 1000; // CO: ppb → ppm
    pollutants[3].concentration = so2;
    pollutants[4].concentration = no2;
    pollutants[5].concentration = o3;

    for (uint8_t i = 0; i < 6; i++) {
        uint16_t aqi = calculate_aqi(pollutants[i].concentration, pollutants[i].breakpoints, pollutants[i].aqi_levels);
        if (aqi > max_aqi) {
            max_aqi = aqi;
            main_pollutant = pollutants[i].name;
        }
    }

    AQI_Result result = {max_aqi, main_pollutant};
    return result;
}


static eModbus modbus_no1;
static char const *TAG = "HMI";
// extern eModbus modbus[N_MODBUS];
static void hmi_task(void *arg){
    modbus_no1.config.ucPort = BSP_MBS_HMI_PORT;
    modbus_no1.timer = BSP_TIMER_MBS_HMI;
    modbus_no1.config.ulBaudRate = 115200;

    // eMBErrorCode err = eMBInit(&modbus_no1, MB_RTU, 1, BSP_MBS_HMI_PORT, 115200, MB_PAR_NONE);
    eMBErrorCode err = eMBInit(&modbus_no1, MB_RTU, 1);
    if (err != MB_ENOERR) {
        log_error(TAG, "Modbus init failed: %d", err);
    }
    err = eMBEnable(&modbus_no1);
    if (err != MB_ENOERR) {
        log_error(TAG, "Modbus enable failed: %d", err);
    }
    log_info(TAG, "Power on HMI");

    bsp_power_on_hmi();
	usSRegHoldBuf[0] = 0x02;
	usSRegHoldBuf[1] = 0x02;
	usSRegHoldBuf[2] = 0x012;
	usSRegHoldBuf[3] = 0x04;
	usSRegHoldBuf[130] = 32;
	usSRegHoldBuf[131] = 80;
	usSRegHoldBuf[101] = 1110;
	usSRegHoldBuf[103] = 1130;
	usSRegHoldBuf[105] = 1150;
	usSRegHoldBuf[107] = 1170;
    usSRegHoldBuf[109] = 1190;
	usSRegHoldBuf[111] = 1210;


    while(1) {
        eMBPoll(&modbus_no1);
        // usSRegHoldBuf[101]++;
        vTaskDelay(1);
    }
}

static void sensor_task(void *arg){
    while(1)
    {
        generate_air_data(&usSRegHoldBuf[101], 0);
        generate_air_data(&usSRegHoldBuf[103], 1);
        generate_air_data(&usSRegHoldBuf[105], 2);
        generate_air_data(&usSRegHoldBuf[107], 3);
        generate_air_data(&usSRegHoldBuf[109], 4);
        generate_air_data(&usSRegHoldBuf[111], 5);
        AQI_Result result = compute_aqi_us(usSRegHoldBuf[101], usSRegHoldBuf[103], usSRegHoldBuf[105], usSRegHoldBuf[107], usSRegHoldBuf[109], usSRegHoldBuf[111]);
        usSRegHoldBuf[113] = result.aqi;
        vTaskDelay(2000);
    }
}

void hmi_app_init(){

    xTaskCreate(hmi_task, "hmi_task", 512, NULL, 4, NULL);
    xTaskCreate(sensor_task, "sensor_task", 128, NULL, 4, NULL);
}


#endif
