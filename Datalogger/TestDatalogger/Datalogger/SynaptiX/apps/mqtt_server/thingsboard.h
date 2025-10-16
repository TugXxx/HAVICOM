#ifndef THINGSBOARD_H_
#define THINGSBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "logger.h"
#define TB_HTTP (0)
#define TB_MQTT (0)
#define TB_OTA  (0)

#define TB_SERVER_HOST "demo.thingsboard.io"
#define TB_SERVER_PORT 1883
#define TB_SERVER_KEEP_ALIVE_S 60

/*! MQTT topic to send a telemetry to ThingsBoard */
#define TB_PUB_TOPIC_HEARDER               "v1/devices/me"
/*! MQTT topic to send a telemetry to ThingsBoard */
#define TB_PUB_TOPIC_TELEMETRY              "v1/devices/me/telemetry"

/*! MQTT topic to send the shared attributes to ThingsBoard */
#define TB_PUB_TOPIC_ATTRIBUTES             "v1/devices/me/attributes" 

/*! MQTT topic to receive the shared attributes if they were updated on ThingsBoard */
#define TB_SUB_TOPIC_ATTRIBUTES             "v1/devices/me/attributes" 
/*! MQTT topic to subscribe for the receiving of the specified shared attribute after the request to ThingsBoard */
#define TB_SUB_TOPIC_ATTRIBUTES_RESPONSE    "v1/devices/me/attributes/response/+"

/*! */
#define TB_SUB_TOPIC_RPC                    "v1/devices/me/rpc/response"
#define TB_PUB_TOPIC_RPC                    "v1/devices/me/rpc/request/%u"

/*! MQTT topic to public to request firmware to ThingsBoard  */
#define TB_PUB_TOPIC_FIRMWARE_RESPONSE      "v2/fw/request/%u/chunk/%u"
/*! MQTT topic to subscribe for the receiving of the firmware after the request to ThingsBoard */
#define TB_SUB_TOPIC_FIRMWARE_RESPONSE      "v2/fw/response/+/chunk/+"


#define TB_COLOR_RED     "\x1b[31m"
#define TB_COLOR_GREEN   "\x1b[36m"//"\x1b[32m"
#define TB_COLOR_YELLOW  "\x1B[38;5;208m"//"\x1b[33m"
#define TB_COLOR_RESET   "\x1b[0m"

#ifdef LOGGER_H
    #define TB_LOG_DEBUG log_debug
    #define TB_LOG_INFO log_info
    #define TB_LOG_WARN log_warn
    #define TB_LOG_ERR log_error
#else
    #define TB_LOG_DEBUG(TAG, fmt,...)   do { \
        static char _info_buf[512]; \
        snprintf(_info_buf, sizeof(_info_buf), TB_COLOR_GREEN "[%s] " fmt TB_COLOR_RESET, TAG, ##__VA_ARGS__); \
        printf(_info_buf); \
    } while(0)

    #define TB_LOG_INFO(TAG, fmt,...)  do { \
        static char _info_buf[512]; \
        snprintf(_info_buf, sizeof(_info_buf), TB_COLOR_GREEN "[%s] " fmt TB_COLOR_RESET, TAG, ##__VA_ARGS__); \
        printf(_info_buf); \
    } while(0)   

    #define TB_LOG_WARN(TAG, fmt,...)  do { \
        static char _info_buf[512]; \
        snprintf(_info_buf, sizeof(_info_buf), TB_COLOR_YELLOW "[%s] " fmt TB_COLOR_RESET, TAG, ##__VA_ARGS__); \
        printf(_info_buf); \
    } while(0)

    #define TB_LOG_ERR(TAG, fmt,...) do { \
        static char _info_buf[512]; \
        snprintf(_info_buf, sizeof(_info_buf), TB_COLOR_RED "[%s] " fmt TB_COLOR_RESET, TAG, ##__VA_ARGS__); \
        printf(_info_buf); \
    } while(0)

#endif

#if TB_MQTT
    #include "server_mqtt.h"
    typedef struct {
        uint16_t payload_len;
        char payload[2048];
        uint16_t topic_len;
        char topic[SERVER_MQTT_TOPIC_SIZE];
        mqtt_client_t *client;
        bool isChunkFW;
    } ota_t;

    void thingsboard_init(void);
    void thingsboard_ota_init(ServerMQTT_t *tb_srv);
#endif


#ifdef __cplusplus
}
#endif

#endif /* THINGSBOARD_H_ */
