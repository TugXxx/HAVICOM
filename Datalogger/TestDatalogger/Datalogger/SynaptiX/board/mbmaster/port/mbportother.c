/* 
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * $Id: mbportother.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "board.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "common/mbtypes.h"
#include "common/mbportlayer.h"
#include "common/mbframe.h"
#include "common/mbutils.h"
/* ----------------------- Defines ------------------------------------------*/
#define MBM_LOGFACILITIES                       ( 0xFFFFU )
#define MBM_LOGLEVELS                           ( MB_LOG_DEBUG )
/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

static UBYTE    ubNesting = 0;
#if defined( MBP_ENABLE_DEBUG_FACILITY ) && ( MBP_ENABLE_DEBUG_FACILITY == 1 )
STATIC CHAR     arubDebugBuffer[256];

xSemaphoreHandle xDebugBufferLock;
#endif
/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
#if defined( MBP_ENABLE_DEBUG_FACILITY ) && ( MBP_ENABLE_DEBUG_FACILITY == 1 )

BOOL
bMBPPortLogIsEnabled( eMBPortLogLevel eLevel, eMBPortLogFacility eModule )
{
    BOOL            bLogMessage = FALSE;

    if( ( ( MBM_LOGFACILITIES & ( ULONG ) eModule ) > 0 ) && ( ( ULONG ) eLevel <= MBM_LOGLEVELS ) )
    {
        bLogMessage = TRUE;
    }
    return bLogMessage;
}

const CHAR     *
pszMBPModule2String( eMBPortLogFacility eModule )
{
    const CHAR     *pszRetValue;

    switch ( eModule )
    {
    case MB_LOG_CORE:
        pszRetValue = "CORE";
        break;
    case MB_LOG_RTU:
        pszRetValue = "RTU";
        break;
    case MB_LOG_ASCII:
        pszRetValue = "ASCII";
        break;
    case MB_LOG_TCP:
        pszRetValue = "TCP";
        break;
    case MB_LOG_PORT_EVENT:
        pszRetValue = "EVENT";
        break;
    case MB_LOG_PORT_TIMER:
        pszRetValue = "TIMER";
        break;
    case MB_LOG_PORT_SERIAL:
        pszRetValue = "SERIAL";
        break;
    case MB_LOG_PORT_TCP:
        pszRetValue = "TCP";
        break;
    case MB_LOG_PORT_OTHER:
        pszRetValue = "OTHER";
        break;
    default:
        pszRetValue = "UNKNOWN";
        break;
    }
    return pszRetValue;
}

const CHAR     *
pszMBPLevel2String( eMBPortLogLevel eLevel )
{
    const CHAR     *pszRetValue;

    switch ( eLevel )
    {
    case MB_LOG_ERROR:
        pszRetValue = "ERROR";
        break;
    case MB_LOG_WARN:
        pszRetValue = "WARN";
        break;
    case MB_LOG_INFO:
        pszRetValue = "INFO";
        break;
    case MB_LOG_DEBUG:
        pszRetValue = "DEBUG";
        break;
    default:
        pszRetValue = "UNKNOWN";
        break;
    }
    return pszRetValue;
}

// void
// vMBPPortLogLWIP( const CHAR * szFmt, ... )
// {
//     int             i = 0, j = 0, max_len;

//     va_list         args;


//     if( xSemaphoreTake( xDebugBufferLock, portMAX_DELAY ) )
//     {
//         max_len = sizeof( arubDebugBuffer ) / sizeof( arubDebugBuffer[0] );
//         j = snprintf( &arubDebugBuffer[i], max_len, "UNKNOWN;lwIP;" );
//         /* Truncation or snprintf error */
//         if( ( j < 0 ) || ( j >= max_len ) )
//         {
//             goto error;
//         }
//         else
//         {
//             max_len -= j;
//             i += j;
//         }

//         va_start( args, szFmt );
//         j = vsnprintf( &arubDebugBuffer[i], max_len, szFmt, args );
//         va_end( args );
//         /* Truncation or snprintf error */
//         if( ( j < 0 ) || ( j >= max_len ) )
//         {
//             goto error;
//         }
//         else
//         {
//             max_len -= j;
//             i += j;
//         }

//         j = 0;
//         while( j < i )
//         {
//             while(!LPC_UART1->LSR & ( 1UL << 5 ));
//             LPC_UART1->THR = arubDebugBuffer[j];
//             j++;
//         };

//       error:
//         /* Can not fail according to API dock if obtained correctly. */
//         ( void )xSemaphoreGive( xDebugBufferLock );
//     }
// }

void
vMBPPortLog( eMBPortLogLevel eLevel, eMBPortLogFacility eModule, const CHAR * szFmt, ... )
{

    int             i = 0, j = 0, max_len;

    va_list         args;

    /* The NULL check is necessary because this could be called during startup within 
     * an assertion.
     */
    if( ( NULL != xDebugBufferLock ) && xSemaphoreTake( xDebugBufferLock, portMAX_DELAY ) )
    {
        if( bMBPPortLogIsEnabled( eLevel, eModule ) )
        {
            max_len = sizeof( arubDebugBuffer ) / sizeof( arubDebugBuffer[0] );
            j = snprintf( &arubDebugBuffer[i], max_len, "%s;%s;", pszMBPLevel2String( eLevel ),
                          pszMBPModule2String( eModule ) );
            if( ( j < 0 ) || ( j >= max_len ) )
            {
                goto error;
            }
            else
            {
                max_len -= j;
                i += j;
            }

            va_start( args, szFmt );
            j = vsnprintf( &arubDebugBuffer[i], max_len, szFmt, args );
            va_end( args );
            /* Truncation or snprintf error */
            if( ( j < 0 ) || ( j >= max_len ) )
            {
                goto error;
            }
            else
            {
                max_len -= j;
                i += j;
            }
            
            /* Transmit using HAL_UART_Transmit_IT */
            // Assuming usb_uart is defined and initialized elsewhere (e.g., board_init.c)
            extern bsp_uart_handle_t usb_uart; 
            // status = HAL_UART_Transmit_IT(usb_uart.handle, (uint8_t*)arubDebugBuffer, i);
            bsp_uart_write(&usb_uart, (uint8_t*)arubDebugBuffer, i);
            // if (status != HAL_OK) {
            //   /* Handle error, e.g., log it or retry */
            //   // You might want to add some error handling here, like logging
            //   // the error using another mechanism if this logging function fails.
            // }
        }
      error:
        /* Can not fail according to API dock if obtained correctly. */
        ( void )xSemaphoreGive( xDebugBufferLock );
    }
}
#endif

void
vMBPAssert( void )
{
    volatile BOOL   bBreakOut = FALSE;

    vMBPEnterCritical(  );
    while( !bBreakOut );
}

void
vMBPEnterCritical( void )
{
    /* Disable interrupts and/or scheduler. Disabling interrupts is
     * necessary if you call ANY of the MODBUS functions from an 
     * interrupt. Disabling the scheduler is necessary if you call
     * any of the MODBUS functions from another thread.
     * In general you should ALWAYS start by disabling both.
     */

    /* Code for disabling interrupts and the scheduler. */
    if( ubNesting == 0 )
    {
        /* Store old processor status register, ... */
    }
    ubNesting++;
}

void
vMBPExitCritical( void )
{
    /* Code for disabling interrupts and the scheduler. */
    ubNesting--;
    if( 0 == ubNesting )
    {
        /* Check old status register if interrupts have been enabled.
         * If yes reenable them. Of course the same holds for the
         * scheduler.
         */
        if( 0 /* Interrupts where enabled */ )
        {
            /* Code for enabling the interrupts and the scheduler. */
        }
    }
}
