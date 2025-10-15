#include "board.h"
#include "cqueue.h"
#include "stdbool.h"
#include "logger.h"
#include <string.h>
#include "button.h"

static const char *TAG = "BSP";

#define MAX_UART_BUFF_SIZE 256

#if BSP_UART_NUM > 0

static CQueue_t uart_queue[BSP_UART_NUM];
static uint8_t uart_buff[BSP_UART_NUM][MAX_UART_BUFF_SIZE];
static bool uart_tranfer_done[BSP_UART_NUM] = {false};
static uint8_t uart_data[BSP_UART_NUM];
static uint8_t uart_data_dma[BSP_UART_NUM][MAX_UART_BUFF_SIZE];

UART_HandleTypeDef *puart[BSP_UART_NUM] = {&hlpuart1, &huart8, &huart4, &huart5, &huart6};// &huart4,&huart5};

uint32_t bsp_com_write(int com_num, uint8_t *buff, uint32_t len)
{
    if (com_num < 0 || com_num >= BSP_UART_NUM)
        return -1;
    return HAL_UART_Transmit(puart[com_num], buff, len, HAL_MAX_DELAY);
}

uint32_t bsp_com_write_it(int com, uint8_t *buff, uint32_t len)
{
    if (com < 0 || com >= BSP_UART_NUM)
        return 0;
    uart_tranfer_done[com] = false;
    HAL_UART_Transmit_IT(puart[com], buff, len);
    return len;
}

typedef void (*com_tx_cb)(void *arg);
typedef struct BSP_COM_Tx_Callback
{
    /* data */
    com_tx_cb cb;
    void *arg;
} BSP_COM_Tx_Callback_t;

BSP_COM_Tx_Callback_t com_tx_callback[BSP_UART_NUM];

void bsp_com_set_tx_callback(int com, void (*callback)(void *arg), void *arg)
{
    if (com < 0 || com >= BSP_UART_NUM)
        return;
    com_tx_callback[com].cb = callback;
    com_tx_callback[com].arg = arg;
}

typedef void (*com_rx_cb)(void *arg);
typedef struct BSP_COM_Rx_Callback
{
    /* data */
    com_rx_cb cb;
    void *arg;
} BSP_COM_Rx_Callback_t;

BSP_COM_Rx_Callback_t com_rx_callback[BSP_UART_NUM];

void bsp_com_set_rx_callback(int com, void (*callback)(void *arg), void *arg)
{
    if (com < 0 || com >= BSP_UART_NUM)
        return;
    com_rx_callback[com].cb = callback;
    com_rx_callback[com].arg = arg;
}

uint32_t  bsp_com_read(int com_num, uint8_t *buff, uint32_t len)
{
    if (com_num < 0 || com_num >= BSP_UART_NUM)
        return 0;
    uint32_t length = 0;
    while (length < len)
    {
        if (false == cqueue_receive(&uart_queue[com_num], &buff[length]))
            break;
        length++;
    }
    return length;
}

uint32_t bsp_com_available(int com_num)
{
    if (com_num < 0 || com_num >= BSP_UART_NUM)
        return 0;

    return uart_queue[com_num].count;
}

uint32_t bsp_com_init()
{
    for (int i = 0; i < BSP_UART_NUM; i++)
    {
        HAL_UART_Receive_IT(puart[i], &uart_data[i], 1);
        cqueue_init_static(&uart_queue[i], uart_buff[i], MAX_UART_BUFF_SIZE, sizeof(uint8_t));
        bsp_com_set_rx_callback(i, NULL, NULL);
        bsp_com_set_tx_callback(i, NULL, NULL);
    }
    return 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *husart)
{
    for (int i = 0; i < BSP_UART_NUM; i++)
    {
        if (husart == puart[i])
        {
            uart_tranfer_done[i] = true;
            if (com_tx_callback[i].cb != NULL)
                com_tx_callback[i].cb(com_tx_callback[i].arg);
            return;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (int i = 0; i < BSP_UART_NUM; i++)
    {
        if (huart == puart[i])
        {
            cqueue_send(&uart_queue[i], &uart_data[i]);
            HAL_UART_Receive_IT(puart[i], &uart_data[i], 1);
            if (com_rx_callback[i].cb != NULL)
                com_rx_callback[i].cb(com_rx_callback[i].arg);
            return;
            return;
        }
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    for (int i = 0; i < BSP_UART_NUM; i++)
    {
        if (huart == puart[i])
        {
            for (uint16_t j = 0; j < Size; j++)
                cqueue_send(&uart_queue[0], &uart_data_dma[i][j]);
            HAL_UARTEx_ReceiveToIdle_DMA(puart[i], uart_data_dma[i], MAX_UART_BUFF_SIZE);
            return;
        }
    }
}

int __io_putchar(int ch){
   return bsp_com_write(BSP_COM_PORT,(uint8_t*)&ch,1);
}
int __io_getchar(void){
    uint8_t ch[1];
    if(1== bsp_com_read(BSP_COM_PORT,&ch,1))
        return ch;
    return -1;
}

#endif

#if BSP_TIMER_NUM > 0

static timer_handle tim_handle[BSP_TIMER_NUM] = {NULL};
static TIM_HandleTypeDef *ptimer[BSP_TIMER_NUM] = {&htim1,&htim2,&htim3};

void bsp_timer_set_handle(int timer, timer_handle handle)
{
    if (0 < timer || timer >= 2)
        return;
    tim_handle[timer] = handle;
}
void bsp_timer_start(int timer)
{
    if (0 < timer || timer >= 2)
        return;
    HAL_TIM_Base_Start_IT(ptimer[timer]);
}
void bsp_timer_stop(int timer)
{
    if (0 < timer || timer >= 2)
        return;
    HAL_TIM_Base_Stop_IT(ptimer[timer]);
}

void BSP_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 0; i < BSP_TIMER_NUM; i++)
        if (htim == ptimer[i])
        {
            if (tim_handle[i] != NULL)
                tim_handle[i]();
            return;
        }
}

#endif

#if BSP_IO_ENABLE && BSP_OUTPUT_NUM > 0 && BSP_INPUT_NUM > 0

uint8_t bsp_get_input(int input_num)
{
    if (input_num == 0)
        return (uint8_t)HAL_GPIO_ReadPin(BSP_INPUT0_PORT, BSP_INPUT0_Pin);
    else if (input_num == 1)
        return (uint8_t)HAL_GPIO_ReadPin(BSP_INPUT1_PORT, BSP_INPUT1_Pin);
    else if (input_num == 2)
        return (uint8_t)HAL_GPIO_ReadPin(BSP_INPUT2_PORT, BSP_INPUT2_Pin);
    else if (input_num == 3)
        return (uint8_t)HAL_GPIO_ReadPin(BSP_INPUT3_PORT, BSP_INPUT3_Pin);
    return 0;
}

uint8_t bsp_output_on(int output_num)
{
    if (output_num == 0)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT0_Pin, GPIO_PIN_SET);
    else if (output_num == 1)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT1_Pin, GPIO_PIN_SET);
    else if (output_num == 2)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT2_Pin, GPIO_PIN_SET);
    else if (output_num == 3)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT3_Pin, GPIO_PIN_SET);
    return 0;
}

uint8_t bsp_output_off(int output_num)
{
    if (output_num == 0)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT0_Pin, GPIO_PIN_RESET);
    else if (output_num == 1)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT1_Pin, GPIO_PIN_RESET);
    else if (output_num == 2)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT2_Pin, GPIO_PIN_RESET);
    else if (output_num == 3)
        HAL_GPIO_WritePin(BSP_OUTPUT_PORT, BSP_OUTPUT3_Pin, GPIO_PIN_RESET);
    return 0;
}

uint8_t bsp_output_toggle(int output_num)
{
    if (output_num == 0)
        HAL_GPIO_TogglePin(BSP_OUTPUT_PORT, BSP_OUTPUT0_Pin);
    else if (output_num == 1)
        HAL_GPIO_TogglePin(BSP_OUTPUT_PORT, BSP_OUTPUT1_Pin);
    else if (output_num == 2)
        HAL_GPIO_TogglePin(BSP_OUTPUT_PORT, BSP_OUTPUT2_Pin);
    else if (output_num == 3)
        HAL_GPIO_TogglePin(BSP_OUTPUT_PORT, BSP_OUTPUT3_Pin);
    return 0;
}
#endif

#if BSP_ADDRESS_NUM > 0
uint8_t bsp_get_address()
{
    uint8_t bit4 = (HAL_GPIO_ReadPin(BSP_ADDRESS_PORT, BSP_ADDRESS_BIT0_Pin) == GPIO_PIN_SET) ? 0 : 1;
    uint8_t bit3 = (HAL_GPIO_ReadPin(BSP_ADDRESS_PORT, BSP_ADDRESS_BIT1_Pin) == GPIO_PIN_SET) ? 0 : 1;
    uint8_t bit2 = (HAL_GPIO_ReadPin(BSP_ADDRESS_PORT, BSP_ADDRESS_BIT2_Pin) == GPIO_PIN_SET) ? 0 : 1;
    uint8_t bit1 = (HAL_GPIO_ReadPin(BSP_ADDRESS_PORT, BSP_ADDRESS_BIT3_Pin) == GPIO_PIN_SET) ? 0 : 1;
    uint8_t bit0 = (HAL_GPIO_ReadPin(BSP_ADDRESS_PORT, BSP_ADDRESS_BIT4_Pin) == GPIO_PIN_SET) ? 0 : 1;
    return ((bit0 << 0) | (bit1 << 1) | (bit2 << 2) | (bit3 << 3) | (bit4 << 4));
}
#endif

#if BSP_UART_NUM > 0
#if BSP_COM_ENABLE
static void log_puts(const char *s)
{
    bsp_com_write(BSP_COM_PORT, (uint8_t *)s, strlen(s));
}
#else
static void log_puts(const char *s)
{
    (void) s;
}
#endif
#endif

I2C_HandleTypeDef *bus_iic[BSP_BUS_NUM] = {&hi2c1};

#if BSP_IIC_ENABLE
uint32_t bsp_iic_bus_init()
{
    for (int i = 0; i < BSP_BUS_NUM; i++)
    {
        if (HAL_I2C_GetState(bus_iic[i]) == HAL_I2C_STATE_RESET)
        {
            if (HAL_I2C_Init(bus_iic[i]) != HAL_OK)
            {
                log_error(TAG, "bsp_iic_bus_init || I2C bus %d init failed", i);
                return 1;
            }
        }
    }
    return 0;
}

uint32_t bsp_iic_write(int bus_num, uint8_t address, uint8_t *data, uint16_t size)
{
    if (bus_num < 0 || bus_num >= BSP_BUS_NUM)
        return 1;
    return HAL_I2C_Master_Transmit(bus_iic[bus_num], address, data, size, HAL_MAX_DELAY);
}

uint32_t bsp_iic_read(int bus_num, uint8_t address, uint8_t *data, uint16_t size)
{
    if (bus_num < 0 || bus_num >= BSP_BUS_NUM)
        return 1;
    return HAL_I2C_Master_Receive(bus_iic[bus_num], address, data, size, HAL_MAX_DELAY);
}
#endif
void bsp_init()
{
    bsp_eth_reset_on();
#if BSP_UART_NUM > 0
    bsp_com_init();
#endif
#if BSP_IIC_ENABLE
    bsp_iic_bus_init();
#endif
    logger_init(BSP_LOG_LEVEL, log_puts);
#if BSP_BUTTON_EN
    button_init();
#endif
}

