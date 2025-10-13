#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

/**
 * @def APPLICATION Version
 * @brief
 *
 * 
 */
#define APP_VERSION 0x01000001

#ifndef APPLICATION_TYPE
#define APPLICATION_TYPE
#endif

#define APPLICATION_MODE_GPIO BSP_IO_ENABLE

#define APPLICATION_MODE_ANALOG BSP_ANALOG_ENABLE

/**
 * @def BUTTON ENABLE
 * @brief Enables or disables Button functionality.
 *
 * Set to 1 to enable BUTTON features, or 0 to disable them.
 */
#define BUTTON_ENABLE BSP_BUTTON_EN

#if APPLICATION_MODE_ANALOG

#ifndef ANALOGS_DEBUG
#define ANALOGS_DEBUG 0
#endif

#endif

/**
 * @def ZIGBEE_ENABLE
 * @brief Enables or disables Zigbee functionality.
 * 
 * Set to 1 to enable Zigbee features in the application.
 * Set to 0 to disable Zigbee features.
 */
#define ZIGBEE_ENABLE 0

/**
 * @def LORA_ENABLE
 * @brief Enables or disables LoRa functionality.
 *
 * Set to 1 to enable LoRa features, or 0 to disable them.
 */
#define LORA_ENABLE 0

#define MODBUS_RTU_ENABLE 0

#define COM_ENABLE 1
#define RS485_ENABLE 0
#define RF_ENABLE 0

#ifdef __cplusplus
}
#endif

#endif