/* 
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id: mbportserial.c,v 1.1 2008-04-06 07:47:26 cwalter Exp $
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

/* ----------------------- Defines ------------------------------------------*/
#define IDX_INVALID				( 255 )
#define UART_BAUDRATE_MIN		( 300 )
#define UART_BAUDRATE_MAX		( 256000 )
#define MAX_RX_TX_BUFF_SIZE	    ( 256 )
#define MAX_RX_QUEUE_SIZE	    ( 256 )

#define HDL_RESET( x ) do { \
	( x )->ubIdx = IDX_INVALID; \
	( x )->pbMBPTransmitterEmptyFN = NULL; \
	( x )->pvMBPReceiveFN = NULL; \
	( x )->xMBMHdl = MB_HDL_INVALID; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    UCHAR           ucPort;
    UBYTE           ubIdx;
    UBYTE			ubTxBuff[MAX_RX_TX_BUFF_SIZE];
    UBYTE			ubRxData;
#if MBM_SERIAL_API_VERSION == 1
    pbMBPSerialTransmitterEmptyAPIV1CB pbMBPTransmitterEmptyFN;
    pvMBPSerialReceiverAPIV1CB pvMBPReceiveFN;
#endif    
#if MBM_SERIAL_API_VERSION == 2
    pbMBPSerialTransmitterEmptyAPIV2CB pbMBPTransmitterEmptyFN;
    pvMBPSerialReceiverAPIV2CB pvMBPReceiveFN;
#endif 
    xMBHandle       xMBMHdl;
} xSerialHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xSerialHandle    xSerialHdls[3];
STATIC uint8_t             SerialQueue[MAX_RX_QUEUE_SIZE];
STATIC BOOL             bIsInitalized = FALSE;

/* ----------------------- Static functions ---------------------------------*/
STATIC void prrvUSARTTxISR( xMBPSerialHandle xSerialHdl  );
STATIC void prrvUSARTRxISR( xMBPSerialHandle xSerialHdl, uint8_t *data, uint16_t len );
/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UBYTE           ubIdx;
    xMBMInternalHandle* xModbus = ( xMBMInternalHandle* )xMBMHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( !bIsInitalized )
    {
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
        {
            HDL_RESET( &xSerialHdls[ubIdx] );
        }
    }

    if( NULL == pxSerialHdl )
    {
        eStatus = MB_EINVAL;
    }
    else if( IDX_INVALID == xSerialHdls[0].ubIdx ) 
    {
        *pxSerialHdl = NULL;

        if( ( ulBaudRate > UART_BAUDRATE_MIN ) && ( ulBaudRate < UART_BAUDRATE_MAX ) && ( MB_HDL_INVALID != xMBMHdl ) )
        {
            /* TODO: Initialize the serial port here. */
#if MBM_RTU_ENABLED == 1
            if (xModbus->xIsRTU == TRUE)
            {
                xSerialHdls[0].ucPort = ucPort;
                // bsp_uart_init(xSerialHdls[0].ucPort, &SerialQueue[0], MAX_RX_QUEUE_SIZE);
                bsp_com_set_rx_callback(xSerialHdls[0].ucPort, prrvUSARTRxISR, &xSerialHdls[0]);
                bsp_com_set_tx_callback(xSerialHdls[0].ucPort, prrvUSARTTxISR, &xSerialHdls[0]);
            }
#endif

#if MBM_ASCII_ENABLED == 1
            if (xModbus->xIsASCII == TRUE)
            {
                xSerialHdls[0].ucPort = (UARTHandle_t *) xModbus->xSer_Hdl;
                // bsp_uart_init(xSerialHdls[0].ucPort, &SerialQueue[0], MAX_RX_QUEUE_SIZE);
                bsp_com_set_rx_callback(xSerialHdls[0].ucPort, prrvUSARTRxISR, &xSerialHdls[0]);
                bsp_com_set_tx_callback(xSerialHdls[0].ucPort, prrvUSARTTxISR, &xSerialHdls[0]);
            }
#endif           
        
            xSerialHdls[0].ubIdx = 0;
            xSerialHdls[0].xMBMHdl = xMBMHdl;
			*pxSerialHdl = &xSerialHdls[0];
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_EINVAL;
        }
    }
    else
    {
        eStatus = MB_ENORES;
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialClose( xMBPSerialHandle xSerialHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        if( ( pxSerialIntHdl->pbMBPTransmitterEmptyFN == NULL ) && ( pxSerialIntHdl->pvMBPReceiveFN == NULL ) )
        {
            /* TODO: Close the serial port here. */
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_EAGAIN;
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl, pbMBPSerialTransmitterEmptyCB pbMBPTransmitterEmptyFN, xMBHandle xMBMHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;
    xMBMInternalHandle* xModbus = ( xMBMInternalHandle* )xMBMHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pbMBPTransmitterEmptyFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pbMBPTransmitterEmptyFN );
            pxSerialIntHdl->pbMBPTransmitterEmptyFN = pbMBPTransmitterEmptyFN;
#if MBM_SERIAL_API_VERSION == 1

#endif

#if MBM_SERIAL_API_VERSION == 2
            USHORT usTxLen = 0;
            pxSerialIntHdl->pbMBPTransmitterEmptyFN(pxSerialIntHdl->xMBMHdl,pxSerialIntHdl->ubTxBuff, MAX_RX_TX_BUFF_SIZE, &usTxLen);
            
#endif
            /* TODO: Enable the transmitter. */
            if (xModbus->rs485_de_select != NULL) xModbus->rs485_de_select(); 
            bsp_com_write(xSerialHdls[0].ucPort, pxSerialIntHdl->ubTxBuff, usTxLen);
        }
        else
        {
            pxSerialIntHdl->pbMBPTransmitterEmptyFN = NULL;
            
            /* TODO: Disable the transmitter. Make sure that all characters have been
             * transmitted in case you do any buffering internally.
             */
            if (xModbus->rs485_de_deselect != NULL) xModbus->rs485_de_deselect(); 
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBPReceiveFN , xMBHandle xMBMHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;
    xMBMInternalHandle* xModbus = ( xMBMInternalHandle* )xMBMHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pvMBPReceiveFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pvMBPReceiveFN );
            pxSerialIntHdl->pvMBPReceiveFN = pvMBPReceiveFN;
            /* TODO: Enable the receiver. */
            if (xModbus->rs485_de_deselect != NULL) xModbus->rs485_de_deselect(); 
        }
        else
        {
            pxSerialIntHdl->pvMBPReceiveFN = NULL;
            
            /* TODO: Disable the receiver. */
            if (xModbus->rs485_de_select != NULL) xModbus->rs485_de_select();
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

STATIC void 
prrvUSARTTxISR( xMBPSerialHandle xSerialHdl  )
{
    BOOL            bHasMoreData = TRUE;
    UBYTE           ubTxByte;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;
    xMBMInternalHandle* xModbus = ( xMBMInternalHandle* )pxSerialIntHdl->xMBMHdl;

    if( NULL != xSerialHdls[0].pbMBPTransmitterEmptyFN )
    {
#if MBM_SERIAL_API_VERSION == 1            
        bHasMoreData = xSerialHdls[0].pbMBPTransmitterEmptyFN( xSerialHdls[0].xMBMHdl, &ubTxByte );
#endif
#if MBM_SERIAL_API_VERSION == 2  
        bHasMoreData = xSerialHdls[0].pbMBPTransmitterEmptyFN( xSerialHdls[0].xMBMHdl, xSerialHdls[0].ubTxBuff,
                                                                MAX_RX_TX_BUFF_SIZE, (USHORT *)&ubTxByte );
#endif  
    }
    if( !bHasMoreData )
    {
        xSerialHdls[0].pbMBPTransmitterEmptyFN = NULL;
        
        /* TODO: Disable the transmitter. */
         if (xModbus->rs485_de_deselect != NULL) xModbus->rs485_de_deselect();
    }
    else
    {
        /* TODO: Place byte ubTxByte in the UART data register. */
        bsp_com_write(xSerialHdls[0].ucPort, xSerialHdls[0].ubTxBuff, ubTxByte);
    }
}

STATIC void 
prrvUSARTRxISR( xMBPSerialHandle xSerialHdl, uint8_t *data, uint16_t len)
{
    (void) len; // Unused parameter
    (void) data; // Unused parameter

    xSerialHandle  *pxSerialIntHdl = xSerialHdl;
    xMBMInternalHandle* xModbus = ( xMBMInternalHandle* )pxSerialIntHdl->xMBMHdl;
    bsp_com_read(xSerialHdls[0].ucPort, &pxSerialIntHdl->ubRxData, 256);

    UBYTE           ubUDR = pxSerialIntHdl->ubRxData; /* TODO: Get byte from UART. */

    MBP_ASSERT( IDX_INVALID != xSerialHdls[0].ubIdx );
    if( NULL != xSerialHdls[0].pvMBPReceiveFN )
    {
#if MBM_SERIAL_API_VERSION == 1       
        xSerialHdls[0].pvMBPReceiveFN( xSerialHdls[0].xMBMHdl, &ubUDR );
#endif
#if MBM_SERIAL_API_VERSION == 2
        xSerialHdls[0].pvMBPReceiveFN( xSerialHdls[0].xMBMHdl, &ubUDR, 1 );
#endif
    }
}
