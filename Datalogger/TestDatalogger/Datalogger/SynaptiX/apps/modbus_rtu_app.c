/* -------------------------------------- SYSTEM INCLUDES --------------------------------------*/
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cJSON.h"
/* ------------------------------------- FLATFORM INCLUDES -------------------------------------*/
#include "nanomodbus.h"
#include "logger.h"
#include <port/port.h>

static const char *TAG = "ModbusRTU";
// #include "../SynaptiXThingsBoardSDK/Widgets/include/modbus_widgets.h"
// #include "../SynaptiXThingsBoardSDK/Widgets/include/thingsboard_widgets.h"

#define MAX_COMMANDS 50 // Maximum number of commands
#define MUTEX_LOCK(mutex)                                             \
   do                                                                 \
   {                                                                  \
      if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE)             \
      {                                                               \
         log_error(TAG, "Error: Failed to take mutex!\r\n");                  \
         /* Handle error appropriately, e.g., return an error code */ \
      }                                                               \
   } while (0)

#define MUTEX_UNLOCK(mutex)                                    \
   do                                                          \
   {                                                           \
      if (xSemaphoreGive(mutex) != pdTRUE)                     \
      {                                                        \
         log_error(TAG, "Error: Failed to give mutex!\r\n");           \
         /* Handle error appropriately, e.g., log the error */ \
      }                                                        \
   } while (0)

/* ------------------------------------- EXTERN VARIABLES --------------------------------------*/
/* Global manager handle */
typedef struct
{
    uint8_t slave_id;
    uint8_t reg_type;
    uint16_t start_addr;
    uint8_t quantity;
    void *buffer;
} xMBMRequest_t;

static nmbs_t nmbs;
uint8_t write_data[1000];
static SemaphoreHandle_t xRWMutex = NULL;
/* ------------------------------------- EXTERN FUNCTION ---------------------------------------*/
uint16_t holding_register[256] = {0};
xMBMRequest_t modbus_requests[2] = {
    // {slave_id, reg_type, start_addr, quantity, buffer}
    {1, 0x03, 1, 10, &holding_register[0]},
    {1, 0x03, 2, 20, &holding_register[1]},
};
/* ------------------------------------- STATIC FUNCTION ----------------------------------------*/

/* ----------------------------------- CODE IMPLEMENTATION --------------------------------------*/
void mbm_rtu_read_task(void *vParameters)
{
   // restart_modbus:
   log_error(TAG, "mbm_rtu_read_task started\r\n");


   // Initialize NMBS client
   nmbs_error err = nmbs_client_init(&nmbs);
   if (err != NMBS_ERROR_NONE)
   {
      log_error(TAG, "nmbs_client_init failed with status: %d\r\n", err);
      return;
   }

   nmbs_error status = NMBS_ERROR_NONE;
   xMBMRequest_t *current = &modbus_requests[0];
   while (1)
   {


         status = NMBS_ERROR_NONE;

         if (current->buffer != NULL)
         {
            nmbs_set_destination_rtu_address(&nmbs, current->slave_id);

            switch (current->reg_type)
            {
            case 0x01:
               // Implement reading coils if needed
               MUTEX_LOCK(xRWMutex);
               status = nmbs_read_coils(&nmbs,
                                        current->start_addr,
                                        current->quantity,
                                        current->buffer);
               MUTEX_UNLOCK(xRWMutex);
               // log_error(TAG, "Read: slave id %d, coils [%s] with status: %d\r\n", current->slave_id, current->name, status);
               // if (status != NMBS_ERROR_NONE)
               // {
               //    log_error(TAG, "Read coils [%s] failed with status: %d\r\n", current->name, status);
               // }
               // else
               // {
               //    uint8_t *data = (uint8_t *)current->buffer;
               //    uint8_t buffer_size = (current->quantity + 7) / 8;
               //    log_error(TAG, "Read coils [%s] SUCCESS - Data received:\r\n", current->name);
               //    for (uint16_t i = 0; i < buffer_size; i++)
               //    {
               //       log_error(TAG, "0x%02X ", data[i]);
               //       // uint16_t byte_index = i / 8;
               //       // uint8_t bit_index = i % 8;
               //       // bool bit_value = (coil_buffer[byte_index] >> bit_index) & 0x01;

               //       // log_error(TAG, "%d", bit_value ? 1 : 0);
               //       // if (i < current->quantity - 1)
               //       // {
               //       //    log_error(TAG, ", ");
               //       // }
               //    }
               //    log_error(TAG, "\r\n");
               // }
               break;
            case 0x02:
               // Implement reading discrete inputs if needed
               MUTEX_LOCK(xRWMutex);
               status = nmbs_read_discrete_inputs(&nmbs,
                                                  current->start_addr,
                                                  current->quantity,
                                                  current->buffer);
               MUTEX_UNLOCK(xRWMutex);
               // log_error(TAG, "Read: slave id %d, discrete inputs [%s] with status: %d\r\n", current->slave_id, current->name, status);
               break;
            case 0x03:
               MUTEX_LOCK(xRWMutex);
               status = nmbs_read_holding_registers(&nmbs,
                                                    current->start_addr,
                                                    current->quantity,
                                                    (uint16_t *)current->buffer);
               MUTEX_UNLOCK(xRWMutex);
               // log_error(TAG, "Read: slave id %d, holding registers [%s] with status: %d\r\n", current->slave_id, current->name, status);
               if (status != NMBS_ERROR_NONE)
               {
                  log_error(TAG, "Read holding failed with status: %d", status);
               }
               else
               {
                  log_info(TAG, "Read holding SUCCESS - Data received:");
                  uint16_t *data = (uint16_t *)current->buffer;
                  for (uint16_t i = 0; i < current->quantity; i++)
                  {
                     log_info(TAG, "Reg[%d] = 0x%04X (%d)", current->start_addr + i, data[i], data[i]);
                  }
               }
               break;
            case 0x04:
               // Implement reading input registers if needed
               MUTEX_LOCK(xRWMutex);
               status = nmbs_read_input_registers(&nmbs,
                                                  current->start_addr,
                                                  current->quantity,
                                                  (uint16_t *)current->buffer);
               MUTEX_UNLOCK(xRWMutex);
               // log_error(TAG, "Read: slave id %d, input registers [%s] with status: %d\r\n", current->slave_id, current->name, status);
               break;

            default:
               // log_error(TAG, "Please implement reading for this register type\r\n");
               break;
            }
         }
         vTaskDelay(2000);

   }
}

void mbm_rtu_write_task(void *vParameters)
{
//    // Initialize write request structure
//    if (write_req == NULL) // To avoid reallocation, when restarting task
//    {
//       write_req = MDM_MALLOC(sizeof(mdm_write_t));
//       if (write_req == NULL)
//       {
//          log_error(TAG, "Failed to allocate memory for write request\r\n");
//          return;
//       }
//       write_req->w_data = &write_data;
//       write_req->w_flag = false;
//    }
//    else
//    {
//       write_req->w_flag = false;
//    }

//    while (1)
//    {
// //      if (config_changed) {
// //         log_warn(TAG, "Config changed, stopping write task to re-initialize Modbus Master RTU");
// //         // Wait until the configuration is applied
// //         while(config_changed) {
// //             vTaskDelay(1000);
// //         }
// //      }
//       // To prioritize write task than read task
//       if ((nmbs.isInitialized == true) && (write_req != NULL) && write_req->w_flag)
//       {
//          mdm_write_registers(&nmbs, write_req);
//          vTaskDelay(500);
//       }
//       vTaskDelay(1);
//    }
}

void mbm_rtu_app_init(void)
{
   // To protect modbus buffers and shared resources during reads and writes
   xRWMutex = xSemaphoreCreateMutex();

   // To prioritize write task than read task
   xTaskCreate(mbm_rtu_read_task, "mbm_rtu_read_task", 512, NULL, 4, NULL);
   // xTaskCreate(mbm_rtu_write_task, "mbm_rtu_write_task", 512, NULL, 5, NULL);
}

/**
 * @brief Main write function - dispatches to appropriate write function based on function code
 */
// static mdm_error_t mdm_write_registers(nmbs_t *nmbs, mdm_write_t *write_req)
// {
//    if (write_req == NULL)
//    {
//       log_error(TAG, "Write request is NULL\r\n");
//       return MDM_ERROR_NULL_POINTER;
//    }

//    if (write_req->w_data == NULL)
//    {
//       log_error(TAG, "Write data buffer is NULL\r\n");
//       return MDM_ERROR_NULL_POINTER;
//    }

//    mdm_error_t result = MDM_ERROR_INVALID_PARAMETER;

//    switch (write_req->func_code)
//    {
//    case MDM_FUNC_WRITE_SINGLE_COIL:
//       xSemaphoreTake(xRWMutex, portMAX_DELAY);
//       result = nmbs_write_single_coil(nmbs, write_req->start_addr, *(bool *)write_req->w_data);
//       xSemaphoreGive(xRWMutex);
//       log_error(TAG, "Write single coil with status: %d\r\n", result);
//       break;

//    case MDM_FUNC_WRITE_SINGLE_REGISTER:
//       xSemaphoreTake(xRWMutex, portMAX_DELAY);
//       result = nmbs_write_single_register(nmbs, write_req->start_addr, *(uint16_t *)write_req->w_data);
//       xSemaphoreGive(xRWMutex);
//       log_error(TAG, "Write single register with status: %d\r\n", result);
//       break;

//    case MDM_FUNC_WRITE_MULTIPLE_COILS:
//       xSemaphoreTake(xRWMutex, portMAX_DELAY);
//       result = nmbs_write_multiple_coils(nmbs, write_req->start_addr, write_req->quantity,
//                                          (uint8_t *)write_req->w_data);
//       xSemaphoreGive(xRWMutex);
//       log_error(TAG, "Write multiple coil with status: %d\r\n", result);
//       break;

//    case MDM_FUNC_WRITE_MULTIPLE_REGISTERS:

//       MUTEX_LOCK(xRWMutex);
//       result = nmbs_write_multiple_registers(nmbs, write_req->start_addr, write_req->quantity,
//                                              (uint16_t *)write_req->w_data);
//       MUTEX_UNLOCK(xRWMutex);
//       log_error(TAG, "Write multiple register with status: %d\r\n", result);
//       break;

//    default:
//       log_error(TAG, "Unsupported write function code: 0x%02X\r\n", write_req->func_code);
//       return MDM_ERROR_INVALID_PARAMETER;
//    }

//    // Clear write flag after operation
//    if (result == MDM_OK)
//    {
//       write_req->w_flag = false;
//       log_error(TAG, "Write operation completed successfully\r\n");
//    }
//    return result;
// }


