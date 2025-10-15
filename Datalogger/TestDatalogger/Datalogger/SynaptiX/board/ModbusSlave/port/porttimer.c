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

/* ----------------------- Platform includes --------------------------------*/
#include "board.h"
#include "logger.h"

/* ------------------------ Static functions --------------------------------*/
static void vMBPortTimerISR(void * arg){	
    eModbus_t modbus = (eModbus_t )arg;
    modbus->pxMBPortCBTimerExpired(modbus);
}

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit(eModbus_t modbus, USHORT usTim1Timerout50us )
{
    bsp_timer_set_handle(modbus->timer, vMBPortTimerISR, modbus);
    return TRUE;
}

inline void
vMBPortTimersEnable( eModbus_t modbus )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	// bsp_timer_start((bsp_timer_handle_t *)modbus->timer);
    bsp_timer_start(modbus->timer);
}

inline void
vMBPortTimersDisable( eModbus_t modbus )
{
    /* Disable any pending timers. */
	// bsp_timer_stop((bsp_timer_handle_t *)modbus->timer);
    bsp_timer_stop(modbus->timer);
}

void            
vMBPortTimersDelay(eModbus_t modbus, USHORT usTimeOutMS )
{
    /* Delay for the number of milliseconds passed to this function. */
    bsp_delay(usTimeOutMS);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */