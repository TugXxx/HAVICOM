#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
// #include "iwdg.h"
#include <stdbool.h>
#include "tim.h"
#include <string.h>
#include "logger.h"

#define BSP_LOG_LEVEL LOGGER_DEBUG
#define BSP_BUTTON_EN 1

#define BSP_ANALOG_ENABLE 0
#define BSP_IO_ENABLE 0
#define BSP_IWDG_ENABLE 0
#define BSP_FLASH_ENABLE 0
#define BSP_IIC_ENABLE 1
#define BSP_SIM_ENABLE 1

#if BSP_IIC_ENABLE 
#define BSP_BUS_NUM 1
#endif

#if BSP_ANALOG_ENABLE
#define BSP_NUM_ADC_CHANNEL 5
#endif
 
#if BSP_IO_ENABLE
/**
 * @def BSP_OUTPUT_NUM
 * @brief Defines the number of output channels available on the board.
 *
 * This macro specifies the total number of output channels (e.g., relays, digital outputs)
 * that are supported by the board hardware. Update this value if the hardware configuration changes.
 */
#define BSP_OUTPUT_NUM 4
/**
 * @def BSP_INPUT_NUM
 * @brief Defines the number of input channels available on the board.
 *
 * This macro specifies the total number of input lines (e.g., digital or analog inputs)
 * that are supported by the board hardware. It is used throughout the codebase to
 * reference the number of available input channels, ensuring consistency and ease of maintenance.
 */
#define BSP_INPUT_NUM 4

#endif

/**
 * @def BSP_GAS_NUM
 * @brief Defines the number of gas sensor available on the board.
 * 
 * This macro specifies the total count of gas sensor that are present and can be controlled
 * via the board support package (BSP). Adjust this value according to the hardware configuration.
 */
#define BSP_GAS_NUM 0
/**
 * @def BSP_LED_NUM
 * @brief Defines the number of LEDs available on the board.
 * 
 * This macro specifies the total count of LEDs that are present and can be controlled
 * via the board support package (BSP). Adjust this value according to the hardware configuration.
 */
#define BSP_LED_NUM 0
/**
 * @def BSP_ADDRESS_NUM
 * @brief Defines the number of board support package (BSP) addresses.
 *
 * This macro specifies the total number of addresses used by the BSP.
 * Adjust this value according to the number of supported addresses required by your hardware configuration.
 */
#define BSP_ADDRESS_NUM 0
/**
 * @def BSP_UART_NUM
 * @brief Defines the number of UART peripherals available on the board.
 *
 * This macro specifies the total count of UART (Universal Asynchronous Receiver/Transmitter)
 * interfaces supported by the board. It is used for configuring and managing UART resources
 * in the application.
 */
#define BSP_UART_NUM 3

#if BSP_UART_NUM > 0
#ifndef BSP_COM_ENABLE
#define BSP_COM_ENABLE 1
#define BSP_COM_PORT 0
#endif
#endif
#define BSP_TIMER_NUM 3
/**
 * @def BSP_DEBUG_COM_PORT
 * @brief Defines the communication port number used for debugging purposes.
 * 
 * This macro specifies which COM port is designated as the debug port for the board support package (BSP).
 * The value should correspond to the port index used in the hardware or software configuration.
 */
#define BSP_DEBUG_COM_PORT BSP_COM_PORT
/**
 * @def BSP_RS485_COM_PORT
 * @brief Defines the communication port number used for RS485 interface.
 * 
 * This macro specifies which COM port is assigned for RS485 communication
 * on the board. Update this value if the RS485 interface is connected to a
 * different port.
 */
#define BSP_RS485_1_COM_PORT 2
#define BSP_RS485_2_COM_PORT 3
#define BSP_SIM_COM_PORT 1
// #define BSP_GPS_COM_PORT 2
// #define BSP_SPS30_COM_PORT 3
// #define BSP_GAS_COM_PORT 4
/**
 * @def TIMER0
 * @brief Macro definition for timer 0 identifier.
 *
 * This macro defines the identifier for Timer 0, typically used to reference
 * the first hardware timer in the system. The value assigned is 0.
 */
#define TIMER0 0
/**
 * @def TIMER1
 * @brief Macro definition for timer identifier 1.
 *
 * This macro defines a constant value representing Timer 1.
 * It can be used throughout the codebase to refer to Timer 1 in a clear and maintainable way.
 */
#define TIMER1 1

// MODBUS RS485 ADRESS GPIO
#if BSP_ADDRESS_NUM > 0
#define BSP_ADDRESS_PORT GPIOB
#define BSP_ADDRESS_BIT0_Pin GPIO_PIN_8
#define BSP_ADDRESS_BIT1_Pin GPIO_PIN_7
#define BSP_ADDRESS_BIT2_Pin GPIO_PIN_6
#define BSP_ADDRESS_BIT3_Pin GPIO_PIN_5
#define BSP_ADDRESS_BIT4_Pin GPIO_PIN_4
#endif
#if BSP_LED_NUM > 0
// LED INDICATOR GPIO
#define BSP_LED0_PORT GPIOB
#define BSP_LED0_Pin GPIO_PIN_1

#define BSP_LED1_PORT GPIOB
#define BSP_LED1_Pin GPIO_PIN_2
#endif

#if BSP_IO_ENABLE
// INPUT GPIO
#define BSP_INPUT0_PORT GPIOA 
#define BSP_INPUT1_PORT GPIOA 
#define BSP_INPUT2_PORT GPIOA 
#define BSP_INPUT3_PORT GPIOB 
#define BSP_INPUT0_Pin GPIO_PIN_5
#define BSP_INPUT1_Pin GPIO_PIN_6
#define BSP_INPUT2_Pin GPIO_PIN_7
#define BSP_INPUT3_Pin GPIO_PIN_0

// OUTPUT GPIO
#define BSP_OUTPUT_PORT GPIOB 
#define BSP_OUTPUT0_Pin GPIO_PIN_12
#define BSP_OUTPUT1_Pin GPIO_PIN_13
#define BSP_OUTPUT2_Pin GPIO_PIN_14
#define BSP_OUTPUT3_Pin GPIO_PIN_15
#endif

#if BSP_GAS_NUM > 0
#define UART4S0_PORT GPIOA
#define UART4S1_PORT GPIOA
#define UART4S0_PIN GPIO_PIN_4
#define UART4S1_PIN GPIO_PIN_5
#endif

// BUTTTON GPIO
#if BSP_UART_NUM > 0
#define BSP_BUTTON_PORT GPIOA
#define BSP_BUTTON_Pin  GPIO_PIN_12

#define SERIAL_WORDLENGTH_8B UART_WORDLENGTH_8B
#define SERIAL_WORDLENGTH_9B UART_WORDLENGTH_9B

#define SERIAL_STOPBITS_1     UART_STOPBITS_1
#define SERIAL_STOPBITS_2     UART_STOPBITS_2

#define SERIAL_PARITY_NONE    UART_PARITY_NONE
#define SERIAL_PARITY_EVEN    UART_PARITY_EVEN
#define SERIAL_PARITY_ODD     UART_PARITY_ODD
#endif

#if BSP_GAS_NUM > 0
#define bsp_gas_select_channel(channel) do { \
                                        if ((channel) < BSP_GAS_NUM) { \
                                            HAL_GPIO_WritePin(UART4S0_PORT, UART4S0_PIN, ((channel) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
                                            HAL_GPIO_WritePin(UART4S1_PORT, UART4S1_PIN, ((channel) & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
                                        } \
                                    } while(0)

#endif

#if BSP_SIM_ENABLE > 0
#endif

#if BSP_BUTTON_EN
#define bsp_button_status() HAL_GPIO_ReadPin(BSP_BUTTON_PORT,BSP_BUTTON_Pin)
#endif

#if BSP_LED_NUM > 0
/**
 * @brief Turns the status LED on.
 *
 * This macro sets the specified GPIO pin (BSP_LED0_Pin on BSP_LED0_PORT) to a low logic level (RESET),
 * which turns the status LED on. The macro wraps the operation in a do-while(0) construct for safe usage
 * in conditional statements and code blocks.
 */
#define bsp_led_status_on()     do{HAL_GPIO_WritePin(BSP_LED0_PORT,BSP_LED0_Pin,GPIO_PIN_RESET);}while(0)
/**
 * @brief Turns the status LED off.
 *
 * This macro sets the specified GPIO pin (BSP_LED0_Pin on BSP_LED0_PORT) to a high logic level,
 * which turns off the status LED according to the board's wiring.
 * It uses the HAL_GPIO_WritePin function to perform the operation.
 */
#define bsp_led_status_off()    do{HAL_GPIO_WritePin(BSP_LED0_PORT,BSP_LED0_Pin,GPIO_PIN_SET);}while(0)
/**
 * @brief Toggles the status LED.
 *
 * This macro toggles the state of the status LED connected to BSP_LED0_PORT and BSP_LED0_Pin.
 * It uses the HAL_GPIO_TogglePin function to change the LED state.
 */
#define bsp_led_status_toggle() do{HAL_GPIO_TogglePin(BSP_LED0_PORT,BSP_LED0_Pin);}while(0)
/**
 * @brief Turns on the network LED by setting the corresponding GPIO pin to RESET (active low).
 *
 * This macro controls the network status LED (BSP_LED1) by writing a RESET value to its GPIO pin,
 * effectively turning the LED on if it is configured as active low.
 *
 * Usage:
 *     bsp_led_net_on();
 *
 * Note:
 *     - Ensure BSP_LED1_PORT and BSP_LED1_Pin are properly defined.
 *     - The macro uses a do-while(0) construct for safe expansion in all contexts.
 */
#define bsp_led_net_on()        do{HAL_GPIO_WritePin(BSP_LED1_PORT,BSP_LED1_Pin,GPIO_PIN_RESET);}while(0)
/**
 * @brief Turns off the network LED by setting the corresponding GPIO pin.
 *
 * This macro sets the BSP_LED1_Pin on BSP_LED1_PORT to a high logic level (GPIO_PIN_SET),
 * which turns off the network LED indicator. It uses the HAL_GPIO_WritePin function
 * provided by the hardware abstraction layer.
 */
#define bsp_led_net_off()       do{HAL_GPIO_WritePin(BSP_LED1_PORT,BSP_LED1_Pin,GPIO_PIN_SET);}while(0)
/**
 * @brief Toggles the state of the network LED.
 *
 * This macro toggles the GPIO pin associated with the network LED (BSP_LED1).
 * It uses the HAL_GPIO_TogglePin function to invert the current state of the LED.
 * 
 * Usage:
 *     bsp_led_net_toggle();
 *
 * Note:
 *     Ensure that BSP_LED1_PORT and BSP_LED1_Pin are properly defined before using this macro.
 */
#define bsp_led_net_toggle()    do{HAL_GPIO_TogglePin(BSP_LED1_PORT,BSP_LED1_Pin) ;}while(0)
#endif
/**
 * @brief Macro to retrieve the current system tick value.
 *
 * This macro maps bsp_get_tick to the HAL_GetTick function, which returns
 * the number of milliseconds since the system started. It is typically used
 * for timing operations and delays.
 *
 * @see HAL_GetTick()
 */
#define bsp_get_tick HAL_GetTick

/**
 * @brief Refreshes (reloads) the Independent Watchdog (IWDG) timer.
 *
 * This macro calls HAL_IWDG_Refresh() with the handle to the IWDG peripheral,
 * preventing the watchdog from resetting the system. It should be called
 * periodically within the allowed time window to ensure normal operation.
 */
#if BSP_IWDG_ENABLE > 0 
#define bsp_iwdg_refresh() HAL_IWDG_Refresh(&hiwdg)
#else
#define bsp_iwdg_refresh()
#endif
/**
 * @brief Triggers a system reset using the NVIC (Nested Vectored Interrupt Controller).
 *
 * This macro provides a convenient way to restart the microcontroller by invoking
 * the NVIC_SystemReset() function. It can be used to perform a software reset of the system.
 */
#define bsp_restart()   NVIC_SystemReset()

/**
 * @brief Delays execution for a specified number of milliseconds and refreshes the independent watchdog timer.
 *
 * This macro wraps the HAL_Delay function to pause execution for 'x' milliseconds,
 * and then calls bsp_iwdg_refresh() to reset the watchdog timer, preventing a system reset.
 *
 * @param x Number of milliseconds to delay.
 */
#define bsp_delay(x) do{HAL_Delay(x);bsp_iwdg_refresh();}while (0)

/**
 * @brief  Enables the DE (Driver Enable) pin for UART2.
 *         This macro sets the USART2_DE pin high, typically used to control the direction
 *         of data flow in RS485 communication (enabling transmission mode).
 * @note   Make sure USART2_DE_GPIO_Port and USART2_DE_Pin are properly defined.
 */
#define bsp_485_de_on(com) do { \
    if (com == BSP_RS485_1_COM_PORT) HAL_GPIO_WritePin(UART4_DE_GPIO_Port, UART4_DE_Pin, GPIO_PIN_SET);\
    else if (com == BSP_RS485_2_COM_PORT) HAL_GPIO_WritePin(UART5_DE_GPIO_Port, UART5_DE_Pin, GPIO_PIN_SET);\
} while(0)
/**
 * @brief Disables the UART2 Driver Enable (DE) pin by setting it to a low logic level.
 *
 * This macro sets the USART2_DE_Pin on the USART2_DE_GPIO_Port to GPIO_PIN_RESET,
 * effectively turning off the driver enable signal for RS485 communication.
 */
#define bsp_485_de_off(com) do { \
    if (com == BSP_RS485_1_COM_PORT) HAL_GPIO_WritePin(UART4_DE_GPIO_Port, UART4_DE_Pin, GPIO_PIN_RESET);\
    else if (com == BSP_RS485_2_COM_PORT) HAL_GPIO_WritePin(UART5_DE_GPIO_Port, UART5_DE_Pin, GPIO_PIN_RESET);\
} while(0)

#if BSP_TIMER_NUM > 0
/**
 * @brief Starts Timer in interrupt mode.
 *
 * This macro initializes and starts the base timer (TIM2) with interrupt enabled
 * using the HAL library function HAL_TIM_Base_Start_IT. It is typically used to
 * begin periodic operations or timing events that require interrupt handling.
 *
 * @note Ensure that the timer handle 'htim2' is properly initialized before calling this macro.
 */
void bsp_timer_start(int timer);
/**
 * @brief Stops Timer using interrupt mode.
 *
 * This macro calls HAL_TIM_Base_Stop_IT() with the handle to Timer 2 (htim2),
 * effectively stopping the timer's base generation in interrupt mode.
 *
 * @note Ensure that htim2 is properly initialized before calling this macro.
 */
void bsp_timer_stop(int timer);

typedef void (*timer_handle)();
/**
 * @brief Sets the handler function for a specified timer.
 *
 * Associates a handler function with a timer, allowing the timer to invoke
 * the specified handler when triggered.
 *
 * @param timer The identifier of the timer to set the handler for.
 * @param handle The handler function to be called when the timer event occurs.
 */
void bsp_timer_set_handle(int timer,timer_handle handle);
#endif

#if BSP_FLASH_ENABLE > 0

#define BSP_FLASH_SECTOR_SIZE (uint32_t)(8*1024) // bytes
#define BSP_FLASH_BASE FLASH_BASE
#define BSP_FLASH_SIZE        (224 * 1024)
#define BSP_FLASH_SECTOR_NUM  (BSP_FLASH_SIZE / BSP_FLASH_SECTOR_SIZE)

#define bsp_flash_unlock() HAL_FLASH_Unlock()
/**
 * @brief   Locks the FLASH control register access.
 *
 * This macro calls the HAL_FLASH_Lock() function to disable write access
 * to the FLASH control registers, preventing accidental modification of
 * FLASH memory. It is typically used after FLASH operations are complete
 * to enhance system safety.
 */
#define bsp_flash_lock() HAL_FLASH_Lock()

/**
 * @brief Erases one or more pages in flash memory starting from the specified address.
 *
 * This function erases a specified number of flash memory pages, beginning at the given page address.
 *
 * @param sector_address The starting address of the flash page to erase.
 * @param num_sectors The number of consecutive pages to erase.
 * @return uint32_t Status code indicating success or failure of the erase operation.
 */
static inline uint32_t bsp_flash_erase(uint32_t sector_address,uint32_t num_sectors){
    FLASH_EraseInitTypeDef erase = {.Banks = FLASH_BANK_1,.Sector = (sector_address - BSP_FLASH_BASE) / BSP_FLASH_SECTOR_SIZE,.NbSectors = num_sectors,.TypeErase = FLASH_TYPEERASE_SECTORS};
    uint32_t page_err = 0;
    if(HAL_FLASHEx_Erase(&erase,&page_err) != HAL_OK){
        return page_err;
    }
    return 0;
}

/**
 * @brief Programs data to the flash memory at the specified address.
 *
 * This function writes a block of data to the flash memory starting at the given address.
 *
 * @param address The starting address in flash memory where data will be programmed.
 * @param data Pointer to the data buffer to be written to flash.
 * @param len The number of bytes to program from the data buffer.
 * @return uint32_t Status code indicating success or failure of the operation.
 */
static inline uint32_t bsp_flash_program(uint32_t address, void *data, uint32_t len) {
    uint32_t length = len / 16;
    uint8_t *p = (uint8_t *)data;

    for (uint32_t i = 0; i < length; i++) {
        uint32_t data_write[4];
        memcpy(data_write, p + i * 16,16);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address + i * 32, (uint32_t)data_write);
    }

    if (len % 4 != 0) {
        uint32_t data_write[4] = {0xFFFFFFFF};
        uint32_t div = len % 16;
        memcpy(data_write, p + length * 16, div);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address + length * 32, (uint32_t)data_write);
    }
    return 0;
}

/**
 * @brief Reads data from flash memory at the specified address.
 *
 * @param address The starting address in flash memory to read from.
 * @param data Pointer to the buffer where the read data will be stored.
 * @param len Number of bytes to read from flash memory.
 * @return uint32_t Status code indicating success or failure of the read operation.
 */
static inline uint32_t bsp_flash_read(uint32_t address,void *data,uint32_t len){
    memcpy(data,(void*)address,len);
    return 0;
}

#endif

// /**
//  * @brief Enables the DE (Driver Enable) signal for the BSP (Board Support Package).
//  *
//  * This function is typically used to control the direction of data flow
//  * in RS485 communication by enabling the driver output.
//  *
//  * @return true if the DE signal was successfully enabled, false otherwise.
//  */
// static inline bool bsp_de_on(){
//     bsp_uart2_de_on();
//     return true;
// }
// /**
//  * @brief Turns off the DE (Driver Enable) signal for the BSP (Board Support Package).
//  *
//  * This function disables the driver enable line, typically used in RS485 communication
//  * to control the direction of data flow. When DE is off, the transceiver is set to receive mode.
//  *
//  * @return true if the operation was successful, false otherwise.
//  */
// static inline bool bsp_de_off(){
//     bsp_uart2_de_off();
//     return true;
// }
    #define bsp_eth_reset_on() HAL_GPIO_WritePin(ETH_RST_GPIO_Port,ETH_RST_Pin,GPIO_PIN_SET)
    #define bsp_eth_reset_off() HAL_GPIO_WritePin(ETH_RST_GPIO_Port,ETH_RST_Pin,GPIO_PIN_RESET)
#if BSP_SIM_ENABLE > 0
    #define bsp_sim_power_on() HAL_GPIO_WritePin(LTE_PWR_GPIO_Port,LTE_PWR_Pin,GPIO_PIN_SET)
    #define bsp_sim_power_off() HAL_GPIO_WritePin(LTE_PWR_GPIO_Port,LTE_PWR_Pin,GPIO_PIN_RESET)
    #define bsp_sim_reset_on() HAL_GPIO_WritePin(ETH_RST_GPIO_Port,LTE_RST_Pin,GPIO_PIN_SET)
    #define bsp_sim_reset_off() HAL_GPIO_WritePin(ETH_RST_GPIO_Port,LTE_RST_Pin,GPIO_PIN_RESET)
#endif

#if BSP_OUTPUT_NUM > 0 && BSP_INPUT_NUM > 0
/**
 * @brief Retrieves the state of a specified input.
 *
 * This function returns the current state (typically HIGH or LOW) of the input
 * specified by the input_num parameter.
 *
 * @param input_num The index or identifier of the input to read.
 * @return uint8_t The state of the specified input (e.g., 0 for LOW, 1 for HIGH).
 */
uint8_t bsp_get_input(int input_num);
uint8_t bsp_output_on(int output_num);
uint8_t bsp_output_off(int output_num);
uint8_t bsp_output_toggle(int output_num);
#endif

#if BSP_UART_NUM > 0

uint32_t bsp_com_write(int com_num,uint8_t *buff,uint32_t len);
uint32_t bsp_com_write_it(int com,uint8_t *buff,uint32_t len);
void bsp_com_set_tx_callback(int com,void (*callback)(void *arg),void *arg);
uint32_t bsp_com_read(int com_num,uint8_t *buff,uint32_t len);
uint32_t bsp_com_available(int com_num);
uint32_t bsp_com_init();
#endif

#if BSP_ADDRESS_NUM > 0
uint8_t bsp_get_address();
#endif

#if BSP_IIC_ENABLE 
uint32_t bsp_iic_write(int bus_num, uint8_t address, uint8_t *data, uint16_t size);
uint32_t bsp_iic_read(int bus_num, uint8_t address, uint8_t *data, uint16_t size);
#endif

void bsp_init();
void BSP_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#ifdef __cplusplus
}
#endif

#endif //BOARD_H
