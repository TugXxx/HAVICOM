/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cJSON.h"

/* ----------------------- Platform includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "logger.h"
#include "semphr.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mbport.h"
#include "mbm.h"
#include "common/mbportlayer.h"

/* ----------------------------------------- DEFINES ------------------------------------------*/
#define MBM_SERIAL_PORT (BSP_RS485_1_COM_PORT)
#define MBM_SERIAL_BAUDRATE (115200)
#define MBM_PARITY (MB_PAR_NONE)

#define MAX_REQUESTS 18
#define MBM_TIME_REQUEST_MS 1

/* ------------------------------------- TYPE DEFINATIONS --------------------------------------*/
typedef struct
{
    uint8_t slave_id;
    uint8_t function_code;
    uint16_t start_addr;
    uint8_t quantity;
    void *buffer;

    // BOOL    in_use;
} xMBMRequest_t;

typedef struct
{
    uint8_t slave_id;
    uint8_t func_code;
    uint16_t start_addr;
    uint16_t quantity;
    void *w_data; // Pointer to data buffer for reading/writing
    bool w_flag;  // when user wants to write data, rise this flag and fall it when data is written
} xMBMWrite_t;

typedef enum
{
    FUNC_READ_COILS = 0x01,
    FUNC_READ_DISCRETE_INPUTS = 0x02,
    FUNC_READ_HOLDING_REGISTERS = 0x03,
    FUNC_READ_INPUT_REGISTERS = 0x04,
    FUNC_WRITE_SINGLE_COIL = 0x05,
    FUNC_WRITE_SINGLE_REGISTER = 0x06,
    FUNC_WRITE_MULTIPLE_COILS = 0x0F,
    FUNC_WRITE_MULTIPLE_REGISTERS = 0x10,
    // Add more function codes as needed
} msm_function_code_t;

/* ------------------------------------- EXTERN VARIABLES --------------------------------------*/


/* ------------------------------------- GLOBAL VARIABLES --------------------------------------*/
uint16_t holding_register[256] = {0};

xMBMRequest_t modbus_requests[3] = {
    // {slave_id, function_code, start_addr, quantity, buffer}
    {1, 0x03, 0, 5, &holding_register[0]},
    {1, 0x03, 5, 5, &holding_register[1]},
    {1, 0x03, 10, 5, &holding_register[2]},

};

xMBMWrite_t *write_req = NULL;
xMBMaster xMBMaster_no1;
xMBHandle xMBMHdl_no1 = NULL;

static SemaphoreHandle_t xRWMutex = NULL;
/* ------------------------------------- LOCAL VARIABLES ---------------------------------------*/
static const char *TAG = "MBM_RTU_APP";

/* ------------------------------------- STATIC FUNCTIONS --------------------------------------*/
static BOOL mbm_de_high();
static BOOL mbm_de_low();
extern void mbm_write_coil(bool state);
int tmrHdl = 0;
int tmrTimeoutHdl = 1;
/* ------------------------------------ START IMPLEMENTATION -----------------------------------*/
void modbus_master_rtu_task(void *vParameters)
{
    eMBErrorCode eStatus;

    /* Initialize the Modbus Master RTU handle. */
    xMBMaster_no1.rs485_de_deselect = mbm_de_low;
    xMBMaster_no1.rs485_de_select = mbm_de_high;
    xMBMaster_no1.xTmrHdl = &tmrHdl;
    xMBMaster_no1.xTmrTimeoutHdl = &tmrTimeoutHdl;
    log_info(TAG, "Modbus Master RTU Task started");

    if (MB_ENOERR ==
        (eStatus = eMBMSerialInit(&xMBMaster_no1, MB_RTU, MBM_SERIAL_PORT, MBM_SERIAL_BAUDRATE, MBM_PARITY)))
    {
        xMBMHdl_no1 = xMBMaster_no1.xMBMHdl;
        log_info(TAG, "eMBMSerialInit successful");
        // mosbus_dynamic_allocation_init();
        do
        {
            eStatus = MB_ENOERR;
            for (int i = 0; i < 2; i++)
            {
                xSemaphoreTake(xRWMutex, portMAX_DELAY);
                eStatus = eMBMReadHoldingRegisters(xMBMHdl_no1,
                                                   modbus_requests[i].slave_id,
                                                   modbus_requests[i].start_addr,
                                                   modbus_requests[i].quantity,
                                                   (uint16_t *)modbus_requests[i].buffer);
                xSemaphoreGive(xRWMutex);
                 log_info(TAG, "eMBMReadHoldingRegisters: %d",eStatus);
                if (modbus_requests[i].buffer == NULL) {
                    continue;
                }

                // switch (modbus_requests[i].function_code) {
                //     case 0x01: // Read Coils
                //         // if( MB_ENOERR !=  (eStatus = eMBMReadCoils( xMBMHdl_no1,
                //         //                                         modbus_requests[i].slave_id,
                //         //                                         modbus_requests[i].start_addr,
                //         //                                         modbus_requests[i].quantity,
                //         //                                         (uint8_t *) modbus_requests[i].buffer )))
                //         // {
                //         //     log_error(TAG, "eMBMReadCoils: %d",eStatus);
                //         // }
                //         // break;

                //     case 0x03: // Read holding register
                //         eStatus = eMBMReadHoldingRegisters( xMBMHdl_no1,
                //                                             modbus_requests[i].slave_id,
                //                                             modbus_requests[i].start_addr,
                //                                             modbus_requests[i].quantity,
                //                                             (uint16_t *) modbus_requests[i].buffer );

                //              log_info(TAG, "eMBMReadHoldingRegisters: %d",eStatus);
                //         break;

                //     case 0x04: // Input Register
                //         break;
                //     default:

                // } // End switch

                // Delay for next request
                vTaskDelay(3000);
            } // End for
        } while (TRUE);

        //        mosbus_dynamic_allocation_free();
    } // End if
    else
    {
        log_info(TAG, "eMBMSerialInit failed with status: %d", eStatus);
        MBP_ASSERT(0);
    }

    if (MB_ENOERR != (eStatus = eMBMClose(xMBMHdl_no1)))
    {
        MBP_ASSERT(0);
    }
}


void mbm_rtu_app_init(void)
{
    xRWMutex = xSemaphoreCreateMutex();
    xTaskCreate(modbus_master_rtu_task, "modbus_master_rtu_task", 1024 * 2, NULL, 4, NULL);
}

static BOOL mbm_de_low()
{
    bsp_485_de_on(BSP_RS485_1_COM_PORT);
    return 0;
}
static BOOL mbm_de_high()
{
    bsp_485_de_off(BSP_RS485_1_COM_PORT);
    return 0;
}

