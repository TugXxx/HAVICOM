/*
 * FreeModbus Libary: Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

 /**********************************************************
 *	Based on Walter's project. 
 *	Modified by TungNX for Synaptix Technology JSC company.
 ***********************************************************/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "string.h"
#if MB_PLATFORM == STM32_FLATFORM
#include "main.h"
#endif

/* ----------------------- Platform includes --------------------------------*/
#include "board.h"
#include "logger.h"

/* -------------------------- Static Variables ------------------------------*/
static uint32_t     lock_count  = 0;
static const char *TAG = "MBSPort";

static void mb_uart_rx_cb(void *arg)
{
	eModbus_t modbus = (eModbus_t )arg;
	modbus->pxMBFrameCBByteReceived(modbus);
}

static void mb_uart_tx_cb(void *arg){
	eModbus_t modbus = (eModbus_t )arg;
	modbus->pxMBFrameCBTransmitterEmpty(modbus);
}

/* ----------------------- MB Port Initialization -------------------------- */
BOOL 
xMBPortSerialInit(eModbus_t modbus)
{
    bsp_com_set_rx_callback(modbus->config.ucPort, mb_uart_rx_cb, modbus);
	bsp_com_set_tx_callback(modbus->config.ucPort, mb_uart_tx_cb, modbus);

	log_info(TAG,"Init port %d success", modbus->config.ucPort);
    return TRUE;
}

/* ----------------------- Enable UART RX/TX ----------------------------- */
void 
vMBPortSerialEnable(eModbus_t modbus, BOOL xRxEnable, BOOL xTxEnable)
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if (xRxEnable)
    {
        if (modbus->rs485_de_deselect != NULL) modbus->rs485_de_deselect();       
    }
    else
    {
		// User code begin
        // bsp_uart_rx_about(modbus->uart);
		// User code end
    }

    if (xTxEnable)
    {
        if (modbus->rs485_de_select != NULL)  modbus->rs485_de_select();
        modbus->pxMBFrameCBTransmitterEmpty(modbus);
    }
    else
    {
		// User code begin
        // bsp_uart_rx_about(modbus->uart);
		// User code end
	}
}

/* ----------------------- Close UART ----------------------------- */
void 
vMBPortClose(eModbus_t modbus) {
    // bsp_uart_deinit((bsp_uart_handle_t *)modbus->uart);
}  

/* ----------------------- Send a Byte ----------------------------- */
BOOL 
xMBPortSerialPutByte(eModbus_t modbus, CHAR ucByte)
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    modbus->tx_done = FALSE;
    bsp_com_write_it(modbus->config.ucPort,(uint8_t*) ucByte, 1);
    return TRUE;

}

/* ----------------------- Send Multiple Bytes ----------------------------- */
BOOL 
xMBPortSerialPutBytes(eModbus_t modbus, volatile UCHAR *ucByte, USHORT usSize)
{
	bsp_com_write_it(modbus->config.ucPort,(uint8_t*) ucByte, usSize);
    return TRUE;

}


/* ----------------------- Receive a Byte ----------------------------- */
BOOL 
xMBPortSerialGetByte(eModbus_t modbus, CHAR *pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	uint8_t res = bsp_com_read(modbus->config.ucPort,(uint8_t*)pucByte,1);
    return (1 == res ) ? TRUE : FALSE;
}

BOOL 
xMBPortSerialGetBaurate(eModbus_t modbus, ULONG *ulBaudRate)
{
    /* Return the baudrate of the UART. This function is called by the
     * protocol stack to get the baudrate of the UART. */
    *ulBaudRate = modbus->config.ulBaudRate;
    return TRUE;
}

void __critical_enter(void)
{
#if MB_PLATFORM == STM32_FLATFORM
	 __disable_irq();
#endif
	lock_count++;
}

void __critical_exit(void)
{
	lock_count--;
	if (lock_count == 0){
#if MB_PLATFORM == STM32_FLATFORM
		 __enable_irq();
#endif
	}	
}
