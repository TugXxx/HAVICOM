#ifndef MQTT_SERVER_H_
#define MQTT_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mqtt_interface.h"
#include "config_manager.h"
#include "environment.h"

#define MQTT_PERIOD_s                 30
#define MQTT_PUB_MSG_SIZE             1024//SERVER_MQTT_PAYLOAD_SIZE//2048
#define MQTT_TOPIC_SIZE               128//SERVER_MQTT_TOPIC_SIZE  //128
#define MQTT_KEEP_ALIVE_S             60
#define MQTT_PORT                     1883



void mqtt_server_init(device_config_t *sys_config);
extern MSU_t msu;
#ifdef __cplusplus
}
#endif

#endif /* MQTT_SERVER_H_ */
