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

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <lwip/sockets.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"

#if MB_TCP_ENABLED == 1
/* ----------------------- Platform includes ----------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "logger.h"

/* ----------------------- MBAP Header --------------------------------------*/
/*
 *
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |                                           
 * (2)   (3)   (4)      (5)    (6)                                          
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte) 
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502 /* TCP listening port. */
#define MB_TCP_POOL_TIMEOUT 1000  /* pool timeout for event waiting. */
#define MB_TCP_POOL_CYCLE   10 /* Time between checking for new data. */
#define MB_TCP_READ_TIMEOUT 1000        /* Maximum timeout to wait for packets. */
#define MB_TCP_READ_CYCLE   100 /* Time between checking for new data. */

#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */

#define EV_CONNECTION       0
#define EV_CLIENT           1
#define EV_NEVENTS          EV_CLIENT + 1

#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)

#define MAX_CLIENTS         5
/* ----------------------- Types --------------------------------------------*/
typedef int SOCKET;

typedef struct {
    eModbus_t   modbus;
    SOCKET      xClientSocket;
    int         index;
    UCHAR       aucTCPBuf[MB_TCP_BUF_SIZE];
    USHORT      usTCPBufPos;
    USHORT      usTCPFrameBytesLeft;
    bool        connected;
} ModbusTCPClientContext;

/* ----------------------- Static variables ---------------------------------*/
static char                 *TAG = "MB(porttcp.c)";
static SOCKET               xListenSocket;
static SemaphoreHandle_t    xClientIndexMutex = NULL;

static ModbusTCPClientContext client_ctx[MAX_CLIENTS];
static TaskHandle_t xTaskHandle[MAX_CLIENTS] = {NULL};
static int current_client_index;

/* ----------------------- External functions -------------------------------*/
// CHAR           *WsaError2String( int dwError );

/* ----------------------- Static functions ---------------------------------*/
static BOOL     prvMBTCPPortAddressToString( SOCKET xSocket, CHAR * szAddr, USHORT usBufSize );
static BOOL     prvbMBPortAcceptClient( void );
static void     prvvMBPortReleaseClient( void );

static void     xMBPortAcceptClient_task(void *pvParameters);
static void     xMBPortPoll_task(void *pvParameters);

/* ----------------------- Begin implementation -----------------------------*/

BOOL
xMBTCPPortInit( eModbus_t modbus, USHORT usTCPPort )
{
    USHORT          usPort;
    struct sockaddr_in serveraddr;

    // Initialize the mutex for client index
    xClientIndexMutex = xSemaphoreCreateMutex();
    if (xClientIndexMutex == NULL)
    {
        log_error(TAG, "Failed to create client index mutex");
        return FALSE;
    }
    // Initialize the port
    if( usTCPPort == 0 )
    {
        usPort = MB_TCP_DEFAULT_PORT;
    }
    else
    {
        usPort = ( USHORT ) usTCPPort;
    }

    // Create a socket address structure
    memset( &serveraddr, 0, sizeof( serveraddr ) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
    serveraddr.sin_port = htons( usPort );

    // Create a socket
    if( ( xListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
    {
        log_error( TAG, "Create socket failed.\r\n" );
        return FALSE;
    }

    // Set socket options
    else if( bind( xListenSocket, ( struct sockaddr * )&serveraddr, sizeof( serveraddr ) ) == -1 )
    {
        log_error( TAG, "Bind socket failed.\r\n" );
        return FALSE;
    }
    
    // Set the socket to listen for incoming connections
    else if( listen( xListenSocket, 5 ) == -1 )
    {
        log_error( TAG, "Listen socket failed.\r\n" );
        return FALSE;
    }

    log_debug(TAG, "Listening on port %d\r\n", usPort);
    
    // Initialize the client context
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_ctx[i].xClientSocket = INVALID_SOCKET;
        client_ctx[i].connected = false;
    }
    
    // Create a task to accept client connections
    xTaskCreate(xMBPortAcceptClient_task, "xMBPortAcceptClient_task",4096*2, modbus, 5, NULL);

    return TRUE;
}

void
vMBTCPPortClose( eModbus_t modbus )
{
    // Close all client sockets. 
    if(  client_ctx[current_client_index].xClientSocket != SOCKET_ERROR )
    {
        prvvMBPortReleaseClient(  );
    }
    // Close the listener socket.
    if( xListenSocket != SOCKET_ERROR )
    {
        close( xListenSocket );
    }
    // Delete the mutex
    if (xClientIndexMutex != NULL)
    {
        vSemaphoreDelete(xClientIndexMutex);
        xClientIndexMutex = NULL;
    }
}

void
vMBTCPPortDisable( eModbus_t modbus )
{
    /* Close all client sockets. */
    if( client_ctx[current_client_index].xClientSocket != SOCKET_ERROR )
    {
        prvvMBPortReleaseClient(  );
    }
}

/**
 * @brief Task function to handle Modbus TCP client communication.
 *
 * This function is responsible for managing the communication with a Modbus TCP client.
 * It handles receiving data from the client, processing Modbus frames, and posting events
 * to the Modbus stack for further handling. The task runs in an infinite loop until the
 * client disconnects or an error occurs.
 *
 * @param pvParameters Pointer to the ModbusTCPClientContext structure containing client-specific data.
 *
 * The function performs the following steps:
 * - Logs the client's IP address upon connection.
 * - Receives data from the client socket and processes it to form complete Modbus frames.
 * - Posts events to the Modbus stack when a complete frame is received.
 * - Manages a mutex to ensure thread-safe access to shared resources.
 * - Handles client disconnection and cleans up resources.
 *
 * Notes:
 * - The function uses FreeRTOS APIs for task management, delays, and synchronization.
 * - The task deletes itself when the client disconnects or an error occurs.
 * - Timeout handling for client communication is managed elsewhere in the code.
 *
 * @return This function does not return as it is a FreeRTOS task.
 */
static void xMBPortPoll_task(void *pvParameters)
{
    ModbusTCPClientContext *ctx = (ModbusTCPClientContext *)pvParameters;
    int             ret;
    uint16_t        usLength;
    char            ipBuffer[64];
    eMBEventType    eEvent;
    bool            mutexHeld = false;

    if (prvMBTCPPortAddressToString(ctx->xClientSocket, ipBuffer, sizeof(ipBuffer))) {
        log_debug(TAG, "Client connected from: %s", ipBuffer);
    } else {
        log_debug(TAG, "Client connected from unknown address");
    }  

    while (1)
    {
        vTaskDelay(10);
        // If the client does not send any data, the recv function will block until the timeout occurs.
        if(((ret = recv( ctx->xClientSocket, &ctx->aucTCPBuf[ctx->usTCPBufPos],ctx-> usTCPFrameBytesLeft,0)) == SOCKET_ERROR ) || ( !ret ) )
        {
            log_error(TAG, "recv client[%d] failed: errno=%d (%s)", 
                ctx->index, errno, strerror(errno));
            break;
        }
        ctx->usTCPBufPos += ret;
        ctx->usTCPFrameBytesLeft -= ret;
        if( ctx->usTCPBufPos >= MB_TCP_FUNC )
        {
            /* Length is a byte count of Modbus PDU (function code + data) and the
            * unit identifier. */
            usLength = ctx->aucTCPBuf[MB_TCP_LEN] << 8U;
            usLength |= ctx->aucTCPBuf[MB_TCP_LEN + 1];

            /* Is the frame already complete. */
            if( ctx->usTCPBufPos < ( MB_TCP_UID + usLength ) )
            {
                ctx->usTCPFrameBytesLeft = usLength + MB_TCP_UID - ctx->usTCPBufPos;
            }
            /* The frame is complete. */
            else if( ctx->usTCPBufPos == ( MB_TCP_UID + usLength ) )
            {       
                // Take the mutex to ensure thread-safe access to the client index          
                if (xSemaphoreTake(xClientIndexMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
                    ( void )xMBPortEventPost(ctx->modbus, EV_FRAME_RECEIVED); 
                    log_debug(TAG, "Received frame from client: %s", ipBuffer);
                    current_client_index = ctx->index;  
                    mutexHeld = true;
                }
            }
            /* This can not happend because we always calculate the number of bytes
            * to receive. */
            else
            {
                assert( ctx->usTCPBufPos <= ( MB_TCP_UID + usLength ) );
            }
        }

        while (mutexHeld) {
            vTaskDelay(10);
            // Check for events, if frame is received and repose is successful,
            // then clear the event and release the mutex
            if ( xMBPortEventLook(ctx->modbus, &eEvent) == TRUE )
            {
                if ((eEvent == EV_ERR) || (eEvent == EV_IDLE)) {
                    xMBPortEventClear(ctx->modbus);
                    xSemaphoreGive(xClientIndexMutex);
                    mutexHeld = false;
                }
            }
            // NOTE: No need check timeout in here, because 
            // it is handled in xMBTCPPortSendResponse below.
            // Task will be deleted when timeout. So task is blocked case, is not happen.
            
            /* int iTimeOut = MB_TCP_POOL_TIMEOUT;
            if( iTimeOut > 0 )
            {
                iTimeOut -= MB_TCP_POOL_CYCLE;
            }
            else
            {
                log_error(TAG, "Client[%d] timeout", ctx->index);
                iTimeOut = MB_TCP_POOL_TIMEOUT;
                xMBPortEventClear(ctx->modbus);
                xSemaphoreGive(xClientIndexMutex);
                mutexHeld = false;
                break;
            }*/
        }
    }

    // If we are here, the client has disconnected or an connect error occurred
    log_info(TAG, "Client disconnected from: %s", ipBuffer);
    close(ctx->xClientSocket);
    ctx->xClientSocket = INVALID_SOCKET;
    ctx->connected = false;
    vTaskDelete(NULL);
}

/*!
 * \brief Receives parts of a Modbus TCP frame and if complete notifies
 *    the protocol stack.
 * \internal 
 *
 * This function reads a complete Modbus TCP frame from the protocol stack.
 * It starts by reading the header with an initial request size for
 * usTCPFrameBytesLeft = MB_TCP_FUNC. If the header is complete the 
 * number of bytes left can be calculated from it (See Length in MBAP header).
 * Further read calls are issued until the frame is complete.
 *
 * \return \c TRUE if part of a Modbus TCP frame could be processed. In case
 *   of a communication error the function returns \c FALSE.
 */

BOOL
xMBTCPPortGetRequest( eModbus_t modbus, UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
        *ppucMBTCPFrame = &(client_ctx[current_client_index].aucTCPBuf[0]);
        *usTCPLength = client_ctx[current_client_index].usTCPBufPos;
        /* Reset the buffer. */
        client_ctx[current_client_index].usTCPBufPos = 0;
        client_ctx[current_client_index].usTCPFrameBytesLeft = MB_TCP_FUNC;
    return TRUE;
}

BOOL
xMBTCPPortSendResponse( eModbus_t modbus, const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL            bFrameSent = FALSE;
    BOOL            bAbort = FALSE;
    int             res;
    int             iBytesSent = 0;
    int             iTimeOut = MB_TCP_READ_TIMEOUT;

    do
    {
        res = send( client_ctx[current_client_index].xClientSocket, &pucMBTCPFrame[iBytesSent], usTCPLength - iBytesSent, 0 );
        switch ( res )
        {
        case -1:
            if( iTimeOut > 0 )
            {
                iTimeOut -= MB_TCP_READ_CYCLE;
                vTaskDelay(pdMS_TO_TICKS(MB_TCP_READ_CYCLE));
            }
            else
            {
                log_error(TAG, "send to client[%d] failed: errno=%d (%s)", 
                    client_ctx[current_client_index].index , errno, strerror(errno));
                bAbort = TRUE;
            }
            break;   
        case 0:
            log_error(TAG, "send to client[%d] failed: errno=%d (%s)", 
                client_ctx[current_client_index].index , errno, strerror(errno));
            prvvMBPortReleaseClient(  );
            bAbort = TRUE;
            break;
        default:
            iBytesSent += res;
            break;
        }
    }
    // Continue sending until all bytes are sent or an error occurs
    while( ( iBytesSent != usTCPLength ) && !bAbort );
    

    bFrameSent = iBytesSent == usTCPLength ? TRUE : FALSE;
    return bFrameSent;
}

static void
prvvMBPortReleaseClient(  )
{
    log_info(TAG, "Client[%d] disconnected", current_client_index);
    if (xTaskHandle[current_client_index] != NULL) {
        vTaskDelete(xTaskHandle[current_client_index]);
    }
    ( void )recv( client_ctx[current_client_index].xClientSocket, &(client_ctx[current_client_index].aucTCPBuf[0]), MB_TCP_BUF_SIZE, 0 );
    client_ctx[current_client_index].connected = false;
    ( void )close( client_ctx[current_client_index].xClientSocket );
    client_ctx[current_client_index].xClientSocket = INVALID_SOCKET;
}


/**
 * @brief Task to handle incoming Modbus TCP client connections
 *
 * This task listens for new client connections on a socket and manages client assignments.
 * When a new connection is accepted, it attempts to assign the client to an available context.
 * If a free client slot is found, it creates a polling task for the new client.
 * If no slots are available, the connection is rejected.
 *
 * @param pvParameters Modbus protocol type passed as a parameter
 */
static void xMBPortAcceptClient_task(void *pvParameters)
{
    eModbus_t           modbus = (eModbus_t)pvParameters;
    SOCKET              xNewSocket;
    int                 size;
    struct sockaddr_in  remotehost;
    struct timeval      timeout;

    log_debug(TAG, "Waiting for client connection");
    while (1)
    {
        // Handle incoming connections and data
        xNewSocket = INVALID_SOCKET;
        xNewSocket = accept( xListenSocket, (struct sockaddr *)&remotehost, (socklen_t *)&size );
        if (xNewSocket < 0) {
            log_error(TAG, "Socket accept failed");
            continue;
        } 

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(xNewSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

        bool assigned = false;
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!client_ctx[i].connected) {
                client_ctx[i].connected = true;
                client_ctx[i].index = i;
                client_ctx[i].xClientSocket = xNewSocket;
                client_ctx[i].usTCPBufPos = 0;
                client_ctx[i].usTCPFrameBytesLeft = MB_TCP_FUNC;
                client_ctx[i].modbus = modbus;
                log_info(TAG, "Client[%d]", i);
                char task_name[16];
                snprintf(task_name, sizeof(task_name), "MBClient%d", i);
                xTaskCreate(xMBPortPoll_task, task_name, 4096,
                            &client_ctx[i], configMAX_PRIORITIES-10, &xTaskHandle[i]);
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            log_warn(TAG, "Too many clients. Connection rejected.");
            close(xNewSocket);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
/**
 * @brief Converts a TCP socket's address to a string representation.
 *
 * This function retrieves the IP address and port number of a given socket
 * and formats them into a string. The resulting string is stored in the 
 * provided buffer. If the address cannot be retrieved or the buffer is 
 * insufficient, the string "unknown" is stored in the buffer.
 *
 * @param xSocket The socket whose address is to be converted.
 * @param szAddr A pointer to the buffer where the resulting string will be stored.
 * @param usBufSize The size of the buffer pointed to by szAddr.
 * 
 * @return TRUE if the address was successfully converted and stored in the buffer,
 *         FALSE otherwise.
 *
 * @note The resulting string will be in the format "IP:Port", e.g., "192.168.1.1:502".
 *       Ensure that the buffer size (usBufSize) is large enough to hold the resulting
 *       string, including the null terminator.
 */

static BOOL
prvMBTCPPortAddressToString(SOCKET xSocket, CHAR *szAddr, USHORT usBufSize)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    BOOL bOkay = FALSE;
    
    memset(&client_addr, 0, sizeof(client_addr));
    memset(szAddr, 0, usBufSize);
    
    // Get the address of the client
    if (getpeername(xSocket, (struct sockaddr *)&client_addr, &addr_len) == 0) {
        // Convert the address to a string
        const char *ip_str = inet_ntoa(client_addr.sin_addr);
        if (ip_str != NULL) {
            // Check if the buffer is large enough
            if (strlen(ip_str) < usBufSize) {
                strcpy(szAddr, ip_str);
                // Append the port number
                char port_str[8];
                snprintf(port_str, sizeof(port_str), ":%d", ntohs(client_addr.sin_port));
                if (strlen(szAddr) + strlen(port_str) < usBufSize) {
                    strcat(szAddr, port_str);
                    bOkay = TRUE;
                }
            }
        }
    }
    // If we couldn't get the address, set it to "unknown"
    if (!bOkay) {
        snprintf(szAddr, usBufSize, "unknown");
    }
    
    return bOkay;
}
#endif
