#ifndef MQTT_INTERFACE_H_
#define MQTT_INTERFACE_H_

#include "mqtt.h"
#include "mqtt_opts.h"
#include "mqtt_priv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include <stdint.h>
#include <stdbool.h>

#define MQTT_CLIENT_USER_NAME_SIZE 		64
#define MQTT_CLIENT_PASSWORD_SIZE 		32
#define MQTT_CLIENT_CLIENTID_SIZE 		32
#define MQTT_CLIENT_HOST_SIZE 			64

#define MQTT_TOPIC_SIZE 			    64
#define MQTT_CLIENT_PAYLOAD_SIZE 		1024

#define MQTT_PUBLISH_SUCCESS_BIT 		1 << 0
#define MQTT_PUBLISH_FAIL_BIT 			1 << 1
#define MQTT_CONNECTED_BIT 				1 << 2
#define MQTT_DISCONNECTED_BIT	 		1 << 3

typedef enum {
	MQTT_PUB_SUCCES = 0,
	MQTT_PUB_FAIL,
	MQTT_PUB_DOING,
	MQTT_PUB_WAITING,
	MQTT_PUB_IDLE
} mqtt_interface_state_t;

typedef void (*mqtt_client_sub)(void);

typedef struct Server{
	mqtt_client_t base;
	struct mqtt_client_cfg {
		char username[MQTT_CLIENT_USER_NAME_SIZE];
		char password[MQTT_CLIENT_PASSWORD_SIZE];
		char clientID[MQTT_CLIENT_CLIENTID_SIZE];
		char host[MQTT_CLIENT_HOST_SIZE];
		int port;
		int keepAlive;
	}*configs;

	mqtt_client_sub mqtt_client_sub_cb;

	struct pub_message{
		char *topic;
		char *payload;
		int topic_len;
		int payload_len;
		uint8_t qos;
		mqtt_interface_state_t state;
	}msgPub;

	char sub_topic[MQTT_TOPIC_SIZE];
	char pub_topic[MQTT_TOPIC_SIZE];

	struct sub_message{
		char *topic;
		char *payload;
		int topic_len;
		int payload_len;
		uint8_t qos;
	}msgSub;

	TaskHandle_t task;
	EventGroupHandle_t events;
	ip_addr_t ip;
}mqtt_interface_t;

/**
 * @brief Initializes the client with the specified configuration and telemetry callback.
 *
 * This function sets up the client instance using the provided configuration structure
 * and a callback function to retrieve the latest telemetry data in JSON format.
 *
 * @param client Pointer to a mqtt_interface_t structure to be initialized.
 * @param _cfg Pointer to a mqtt_client_cfg structure containing configuration parameters.
 */
void mqtt_interface_init(mqtt_interface_t *client,struct mqtt_client_cfg *_cfg);

#endif /* MQTT_INTERFACE_H_ */
