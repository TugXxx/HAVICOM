/* 
 * MODBUS Library: Port skeletion
 * Copyright (c) 2008 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * $Id: mbport.h,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */

#ifndef _MB_PORT_H
#define _MB_PORT_H

#include <assert.h>

//#include "common/mbportlayer.h"
//#include "common/mbframe.h"
//#include "common/mbutils.h"
//
//#include "internal/mbmi.h"
#include "internal/mbmiconfig.h"
//#include "internal/mbmiframe.h"

#ifdef __cplusplus
extern          "C"
{
#endif

/* ----------------------- Defines ------------------------------------------*/

#define INLINE                              inline
#define STATIC                              static

#define PR_BEGIN_EXTERN_C                   extern "C" {
#define	PR_END_EXTERN_C                     }

#define MBP_ASSERT( x )                     \
    ( ( x ) ? ( void )0 : vMBPAssert( ) )

#define MBP_ENTER_CRITICAL_SECTION( )       vMBPEnterCritical( )
#define MBP_EXIT_CRITICAL_SECTION( )        vMBPExitCritical( )

#ifndef TRUE
#define TRUE                                ( BOOL )1
#endif

#ifndef FALSE
#define FALSE                               ( BOOL )0
#endif

#define MBP_EVENTHDL_INVALID                NULL
#define MBP_TIMERHDL_INVALID                NULL
#define MBP_SERIALHDL_INVALID               NULL
#define MBP_TCPHDL_INVALID                  NULL
#define MBP_TCPHDL_CLIENT_INVALID           NULL

 #define MBP_ENABLE_DEBUG_FACILITY ( 0 )

 #define MBM_SER_PDU_SIZE_MAX ( 256 )

/* ----------------------- Function prototypes ------------------------------*/


/* ----------------------- Type definitions ---------------------------------*/
typedef void      *xMBPEventHandle;
typedef void      *xMBPTimerHandle;
typedef void      *xMBPSerialHandle;
typedef void      *xMBPTCPHandle;
typedef void      *xMBPTCPClientHandle;

typedef char      BOOL;

typedef char      BYTE;
typedef unsigned char UBYTE;

typedef unsigned char UCHAR;
typedef char      CHAR;

typedef unsigned short USHORT;
typedef short     SHORT;

typedef unsigned long ULONG;
typedef long      LONG;

#if MBM_RTU_ENABLED == 1
typedef enum
{
    MBM_STATE_RX_IDLE, /*!< Receiver is in idle state. */
    MBM_STATE_RX_RCV,  /*!< Frame is beeing received. */
    MBM_STATE_RX_ERROR /*!< Receiver error condition. */
} eMBMRTURcvState;

typedef enum
{
    MBM_STATE_TX_IDLE,  /*!< Transmitter is in idle state. */
    MBM_STATE_TX_XMIT,  /*!< Transmitter is sending data. */
    MBM_STATE_TX_WAIT   /*!< Trasnmitter is in wait for inter frame time after a broadcast frame has been sent */
} eMBMRTUSndState;

typedef struct
{
    UBYTE          ubIdx;
    volatile UBYTE ubRTUFrameBuffer[MBM_SER_PDU_SIZE_MAX];

    volatile eMBMRTURcvState eRcvState;
    volatile USHORT          usRcvBufferPos;

    volatile eMBMRTUSndState eSndState;
    volatile USHORT          usSndBufferCnt;
    UBYTE *                  pubSndBufferCur;

    xMBPTimerHandle xTmrHdl;
    xMBPSerialHandle xSerHdl;
#if MBM_RTU_WAITAFTERSEND_ENABLED == 1
    xMBPTimerHandle xTmrWaitHdl;
#endif
} xMBMRTUFrameHandle;

#endif // MBM_RTU_ENABLED == 1

#if MBM_ASCII_ENABLED == 1
typedef enum
{
    STATE_RX_IDLE,     /*!< Receiver is in idle state. */
    STATE_RX_RCV,      /*!< Frame is beeing received. */
    STATE_RX_WAIT_EOF, /*!< Wait for End of Frame. */
    STATE_RX_ERROR     /*!< Error during receive. */
} eMBMASCIIRcvState;

typedef enum
{
    STATE_TX_IDLE,  /*!< Transmitter is in idle state. */
    STATE_TX_START, /*!< Starting transmission (':' sent). */
    STATE_TX_DATA,  /*!< Sending of data (Address, Data, LRC). */
    STATE_TX_END,   /*!< End of transmission. */
    STATE_TX_NOTIFY /*!< Notify sender that the frame has been sent. */
} eMBMASCIISndState;

typedef enum
{
    BYTE_HIGH_NIBBLE, /*!< Character for high nibble of byte. */
    BYTE_LOW_NIBBLE   /*!< Character for low nibble of byte. */
} eMBMASCIIBytePos;

typedef struct
{
    UBYTE                      ubIdx;
    volatile UBYTE             ubASCIIFrameBuffer[MBM_SER_PDU_SIZE_MAX];
    eMBMASCIIBytePos           eBytePos;
    volatile eMBMASCIIRcvState eRcvState;
    volatile USHORT            usRcvBufferPos;

    volatile eMBMASCIISndState eSndState;
    volatile USHORT            usSndBufferCnt;
    UBYTE *                    pubSndBufferCur;

    xMBPTimerHandle xTmrHdl;
#if MBM_ASCII_BACKOF_TIME_MS > 0
    xMBPTimerHandle xBackOffTmrHdl;
#endif
#if MBM_ASCII_WAITAFTERSEND_ENABLED > 0
    xMBPTimerHandle xWaitTmrHdl;
#endif
    xMBPSerialHandle xSerHdl;
} xMBMASCIIFrameHandle;
#endif // MBM_ASCII_ENABLED == 1
/* ----------------------- Function prototypes ------------------------------*/
void              vMBPEnterCritical( void );
void              vMBPExitCritical( void );
void              vMBPAssert( void );

#ifdef __cplusplus
}
#endif

#endif
