/*
 * FreeModbus Libary: BARE Port
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

/* ----------------------- Platform includes --------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "board.h"

extern eModbus modbus[N_MODBUS];

/* ----------------------- static functions ---------------------------------*/
static void mb_timer0_callback(){
	modbus[0].pxMBPortCBTimerExpired(&modbus[0]);
	return;
}
static void mb_timer1_callback(){
	modbus[1].pxMBPortCBTimerExpired(&modbus[1]);
	return;
}
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit(eModbus_t modbus, USHORT usTim1Timerout50us )
{
	// modbus->timer_handle = (void*) &mb_timer[modbus->config.ucPort];
	bsp_timer_set_handle(0,mb_timer0_callback);
	bsp_timer_set_handle(1,mb_timer1_callback);
    return TRUE;
}


inline void
vMBPortTimersEnable( eModbus_t modbus )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
//	modbus->timer_start(modbus->timer_handle);
	if(modbus->config.ucPort == BSP_RS485_COM_PORT){
		bsp_timer0_start();
	}else if(modbus->config.ucPort == BSP_RF_COM_PORT){
		bsp_timer1_start();
	}
}

inline void
vMBPortTimersDisable( eModbus_t modbus )
{
    /* Disable any pending timers. */
//	modbus->timer_stop(modbus->timer_handle);
	if(modbus->config.ucPort == BSP_RS485_COM_PORT){
		bsp_timer0_stop();
	}else if(modbus->config.ucPort == BSP_RF_COM_PORT){
		bsp_timer1_stop();
	}
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */

