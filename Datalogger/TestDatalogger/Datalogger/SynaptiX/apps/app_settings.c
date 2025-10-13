#include "app_settings.h"
#include "logger.h"

static const char *TAG = "Settings";

void app_setting_load(APP_Settings_t *setting)
{
    bsp_flash_read(APP_SETTING_ADDRESS, setting, sizeof(APP_Settings_t));
    if (setting->mg_number != MG_NUMBER)
    {
        // log_error(TAG, "Setting area error, overwrite setting default");

        setting->com.baudrate = 115200;
        setting->com.databits = DATA8;
        setting->com.parity = PARITY_NONE;
        setting->com.stopbits = STOPBITS_1;

        setting->rf.baudrate = 115200;
        setting->rf.databits = DATA8;
        setting->rf.parity = PARITY_NONE;
        setting->rf.stopbits = STOPBITS_1;

        setting->rs485.baudrate = 115200;
        setting->rs485.databits = DATA8;
        setting->rs485.parity = PARITY_NONE;
        setting->rs485.stopbits = STOPBITS_1;
        setting->log_level = LOGGER_DEBUG;
        setting->app_mode = Parency_Transfer;
        setting->mg_number = MG_NUMBER;

        app_setting_save(setting);
    }
}

void app_setting_save(APP_Settings_t *setting)
{
    bsp_flash_unlock();
    bsp_flash_erase(APP_SETTING_ADDRESS, 1);
    bsp_flash_program(APP_SETTING_ADDRESS, setting, sizeof(APP_Settings_t));
    bsp_flash_lock();
}

void app_setting_printf(APP_Settings_t *setting){
    log_info(TAG, "RS485 :");
    log_info(TAG, "Baudrate : %lu", setting->rs485.baudrate);
    log_info(TAG, "Databits : %d", setting->rs485.databits == DATA8 ? 8 : 9);
    log_info(TAG, "Stopbits : %d", setting->rs485.stopbits == STOPBITS_1 ? 1 : 2);
    switch (setting->rs485.parity)
    {
    case PARITY_NONE:
        /* code */
        log_info(TAG, "Parity   : None");
        break;
    case PARITY_EVEN:
        log_info(TAG, "Parity   : Even");
        break;
    case PARITY_ODD:
        log_info(TAG, "Parity   : Odd");
        break;
    default:
        break;
    }

    log_info(TAG, "RF :");
    log_info(TAG, "Baudrate : %lu", setting->rf.baudrate);
    log_info(TAG, "Databits : %d", setting->rf.databits == DATA8 ? 8 : 9);
    log_info(TAG, "Stopbits : %d", setting->rf.stopbits == STOPBITS_1 ? 1 : 2);
    switch (setting->rf.parity)
    {
    case PARITY_NONE:
        /* code */
        log_info(TAG, "Parity   : None");
        break;
    case PARITY_EVEN:
        log_info(TAG, "Parity   : Even");
        break;
    case PARITY_ODD:
        log_info(TAG, "Parity   : Odd");
        break;
    default:
        break;
    }

    log_info(TAG, "USB :");
    log_info(TAG, "Baudrate : %lu", setting->com.baudrate);
    log_info(TAG, "Databits : %d", setting->com.databits == DATA8 ? 8 : 9);
    log_info(TAG, "Stopbits : %d", setting->com.stopbits == STOPBITS_1 ? 1 : 2);
    switch (setting->com.parity)
    {
    case PARITY_NONE:
        /* code */
        log_info(TAG, "Parity   : None");
        break;
    case PARITY_EVEN:
        log_info(TAG, "Parity   : Even");
        break;
    case PARITY_ODD:
        log_info(TAG, "Parity   : Odd");
        break;
    default:
        break;
    }
}

void app_setup_serial(int serial, Serial_t *config)
{
    switch (serial)
    {
    case BSP_DEBUG_COM_PORT:
        /* code */
        huart1.Instance = USART1;
        huart1.Init.BaudRate = config->baudrate;
        huart1.Init.WordLength = config->databits;
        huart1.Init.StopBits = config->stopbits;
        huart1.Init.Parity = config->parity;
        huart1.Init.Mode = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart1) != HAL_OK)
        {
            Error_Handler();
        }
        break;
    case BSP_RS485_COM_PORT:
        huart2.Instance = USART2;
        huart2.Init.BaudRate = config->baudrate;
        huart2.Init.WordLength = config->databits;
        huart2.Init.StopBits = config->stopbits;
        huart2.Init.Parity = config->parity;
        huart2.Init.Mode = UART_MODE_TX_RX;
        huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart2.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart2) != HAL_OK)
        {
            Error_Handler();
        }
        break;
    case BSP_RF_COM_PORT:
        huart3.Instance = USART3;
        huart3.Init.BaudRate = config->baudrate;
        huart3.Init.WordLength = config->databits;
        huart3.Init.StopBits = config->stopbits;
        huart3.Init.Parity = config->parity;
        huart3.Init.Mode = UART_MODE_TX_RX;
        huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart3.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart3) != HAL_OK)
        {
            Error_Handler();
        }
        break;
    default:
        break;
    }
}

APP_Settings_t app_setting;