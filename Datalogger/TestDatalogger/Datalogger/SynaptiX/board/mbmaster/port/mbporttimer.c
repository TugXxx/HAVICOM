/* 
 * MODBUS Library: Port skeletion
 * Copyright (c) 2008 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * $Id: mbporttimer.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/

#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "common/mbtypes.h"
#include "common/mbportlayer.h"
#include "common/mbframe.h"
#include "common/mbutils.h"
#include "internal/mbmi.h"

#include "board.h"
#include "logger.h"

/* ----------------------- Defines ------------------------------------------*/

#define MAX_TIMER_HDLS          ( 4 )
#define IDX_INVALID             ( 255 )
#define TIMER_TIMEOUT_INVALID	( 65535U )

#define RESET_HDL( x ) do { \
    ( x )->bspTimerHdl = NULL; \
    ( x )->ubIdx = IDX_INVALID; \
	( x )->usNTimeOutMS = 0; \
	( x )->usNTimeLeft = TIMER_TIMEOUT_INVALID; \
    ( x )->xMBMHdl = MB_HDL_INVALID; \
    ( x )->pbMBPTimerExpiredFN = NULL; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    int bspTimerHdl;
    UBYTE           ubIdx;
    USHORT          usNTimeOutMS;
    USHORT          usNTimeLeft;
    xMBHandle       xMBMHdl;
    pbMBPTimerExpiredCB pbMBPTimerExpiredFN;
} xTimerInternalHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xTimerInternalHandle arxTimerHdls[MAX_TIMER_HDLS];
STATIC BOOL     bIsInitalized = FALSE;
STATIC USHORT in = 0;
/* ----------------------- Static functions ---------------------------------*/
STATIC void prvvTimerISR( void *agr);
/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPTimerInit( xMBPTimerHandle * xTimerHdl, USHORT usTimeOut1ms,
               pbMBPTimerExpiredCB pbMBPTimerExpiredFN, xMBHandle xHdl )
{
    eMBErrorCode    eStatus = MB_EPORTERR;
    UBYTE           ubIdx;
    xMBMInternalHandle* xMBMHdl = ( xMBMInternalHandle* )xHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( ( NULL != xTimerHdl ) && ( NULL != pbMBPTimerExpiredFN ) && ( MB_HDL_INVALID != xHdl ) )
    {
        if( !bIsInitalized )
        {
            /* Initialize a hardware timer for 1 millisecond. */
            for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( arxTimerHdls ); ubIdx++ )
            {
                RESET_HDL( &arxTimerHdls[ubIdx] );
            }

            bIsInitalized = TRUE;

        }
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( arxTimerHdls ); ubIdx++ )
        {
            if( IDX_INVALID == arxTimerHdls[ubIdx].ubIdx )
            {
                break;
            }
        }
        if( MAX_TIMER_HDLS != ubIdx )
        {
            // Initialize the timer handle for the response timeout.

            if (xTimerHdl == &( xMBMHdl->xRespTimeoutHdl ))
            {
                arxTimerHdls[ubIdx].bspTimerHdl = *((int *) xMBMHdl->xTmr_RespTimeoutHdl);
              
                bsp_timer_set_handle(arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                // bsp_timer_init(arxTimerHdls[ubIdx].bspTimerHdl);
            }
#if MBM_RTU_ENABLED == 1
            if (xMBMHdl->xIsRTU == TRUE)
            {
                xMBMRTUFrameHandle *xFrameHdl = xMBMHdl->xFrameHdl;
                if (xTimerHdl == &( xFrameHdl->xTmrHdl ))
                {
                    arxTimerHdls[ubIdx].bspTimerHdl = *((int *) xMBMHdl->xTmr_Hdl);
         
                    bsp_timer_set_handle(arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                    // bsp_timer_init(arxTimerHdls[ubIdx].bspTimerHdl);
                }
    #if MBM_RTU_WAITAFTERSEND_ENABLED == 1
                if (xTimerHdl == &( xFrameHdl->xTmrWaitHdl ))
                {
                    arxTimerHdls[ubIdx].bspTimerHdl = (int *) xMBMHdl->xTmr_WaitHdl;
                    if (arxTimerHdls[ubIdx].bspTimerHdl == NULL) return MB_ENOERR;
                    bsp_timer_set_handle(arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                    // bsp_timer_init(arxTimerHdls[ubIdx].bspTimerHdl);
                }
    #endif
            }
#endif

#if MBM_ASCII_ENABLED == 1
            if (xMBMHdl->xIsASCII == TRUE)
            {            
                xMBMASCIIFrameHandle *xFrameHdl = (int *) xMBMHdl->xFrameHdl;
                if (xTimerHdl == &( xFrameHdl->xTmrHdl ))
                {
                    arxTimerHdls[ubIdx].bspTimerHdl = xMBMHdl->xTmr_Hdl;
                    if (arxTimerHdls[ubIdx].bspTimerHdl == NULL) return MB_ENOERR;
                    bsp_timer_set_handle(arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                    // bsp_timer_init(arxTimerHdls[ubIdx].bspTimerHdl);
                }

    #if MBM_ASCII_WAITAFTERSEND_ENABLED == 1
                if (xTimerHdl == &( xFrameHdl->xWaitTmrHdl ))
                {
                    arxTimerHdls[ubIdx].bspTimerHdl = (int *) xMBMHdl->xTmr_WaitHdl;
                    if (arxTimerHdls[ubIdx].bspTimerHdl == NULL) return MB_ENOERR;
                    bsp_timer_set_handle(arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                    // bsp_timer_init(arxTimerHdls[ubIdx].bspTimerHdl);

                }
    #endif

    #if MBM_ASCII_BACKOF_TIME_MS == 1
                if (xTimerHdl == &( xFrameHdl->xBackOffTmrHdl ))
                {
                    arxTimerHdls[ubIdx].bspTimerHdl = xMBMHdl->xTmr_BackOffHdl;
                    if (arxTimerHdls[ubIdx].bspTimerHdl == NULL) return MB_ENOERR;
                    bsp_timer_set_handle(&arxTimerHdls[ubIdx].bspTimerHdl, prvvTimerISR);
                }
    #endif
            }
#endif
            arxTimerHdls[ubIdx].ubIdx = ubIdx;
            arxTimerHdls[ubIdx].usNTimeOutMS = usTimeOut1ms;
            arxTimerHdls[ubIdx].usNTimeLeft = TIMER_TIMEOUT_INVALID;
            arxTimerHdls[ubIdx].xMBMHdl = xHdl;
            arxTimerHdls[ubIdx].pbMBPTimerExpiredFN = pbMBPTimerExpiredFN;

            *xTimerHdl = &arxTimerHdls[ubIdx];
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

void
vMBPTimerClose( xMBPTimerHandle xTimerHdl )
{
    xTimerInternalHandle *pxTimerIntHdl = xTimerHdl;

    if( MB_IS_VALID_HDL( pxTimerIntHdl, arxTimerHdls ) )
    {
        RESET_HDL( pxTimerIntHdl );    
    }
}

eMBErrorCode
eMBPTimerSetTimeout( xMBPTimerHandle xTimerHdl, USHORT usTimeOut1ms )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xTimerInternalHandle *pxTimerIntHdl = xTimerHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxTimerIntHdl, arxTimerHdls ) &&
        ( usTimeOut1ms > 0 ) && ( usTimeOut1ms != TIMER_TIMEOUT_INVALID ) )
    {
        pxTimerIntHdl->usNTimeOutMS = usTimeOut1ms;
        eStatus = MB_ENOERR;
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPTimerStart( xMBPTimerHandle xTimerHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xTimerInternalHandle *pxTimerIntHdl = xTimerHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxTimerIntHdl, arxTimerHdls ) )
    {
        pxTimerIntHdl->usNTimeLeft = pxTimerIntHdl->usNTimeOutMS;
        eStatus = MB_ENOERR;
    }
    bsp_timer_start(pxTimerIntHdl->bspTimerHdl);
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPTimerStop( xMBPTimerHandle xTimerHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xTimerInternalHandle *pxTimerIntHdl = xTimerHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxTimerIntHdl, arxTimerHdls ) )
    {
        pxTimerIntHdl->usNTimeLeft = TIMER_TIMEOUT_INVALID;
        eStatus = MB_ENOERR;
    }
    bsp_timer_stop(pxTimerIntHdl->bspTimerHdl);
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

STATIC void prvvTimerISR( void *agr )
{
    UBYTE           ubIdx;

    for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( arxTimerHdls ); ubIdx++ )
    {
        if( ( IDX_INVALID != arxTimerHdls[ubIdx].ubIdx ) &&
            ( TIMER_TIMEOUT_INVALID != arxTimerHdls[ubIdx].usNTimeLeft ) )
        {
            arxTimerHdls[ubIdx].usNTimeLeft--;
            if( 0 == arxTimerHdls[ubIdx].usNTimeLeft )
            {
                arxTimerHdls[ubIdx].usNTimeLeft = TIMER_TIMEOUT_INVALID;
                ( void )arxTimerHdls[ubIdx].pbMBPTimerExpiredFN( arxTimerHdls[ubIdx].xMBMHdl );
            }
        }
    }
}
