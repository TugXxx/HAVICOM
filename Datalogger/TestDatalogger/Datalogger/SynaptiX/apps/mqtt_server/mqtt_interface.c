#include "mqtt_interface.h"
#include "logger.h"
#include "string.h"
#include "lwip/dns.h"
//#include "event_app.h"
//#include "user_fdk_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "cJSON.h"
#include "device_state.h"

#define MQTT_INTERFACE_STACK_SIZE (1024 * 4)
#define MQTT_INTERFACE_PRIORITY (configMAX_PRIORITIES - 10)


#define MQTT_PUSHLISH_TIME 2000
#define p_client(x) ((mqtt_client_t *)(x))
static const char *TAG = "mqtt interface";
static void MQTTClientProcess(void *arg);
// static void PowerDownNotify(void *arg);
static err_t mqtt_do_connect(mqtt_interface_t *client);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
							   mqtt_connection_status_t status);
static void mqtt_sub_request_cb(void *arg, err_t result);
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
									 u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
								  u8_t flags);
static void mqtt_pub_request_cb(void *arg, err_t result);
static void mqtt_subscribe_handle(mqtt_interface_t *client);
static void mqtt_reconnect_task(void *arg);

static void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *arg)
{
		log_info(TAG, "Host %s have ipv4 : %s", name, ip_ntoa(ipaddr));
	mqtt_interface_t *client = (mqtt_interface_t *)arg;
	client->ip.addr = ipaddr->addr;
}

void mqtt_interface_init(mqtt_interface_t *client, struct mqtt_client_cfg *_cfg)
// ,get_last_telemetry_json _get_telemetry)
{
	log_info(TAG, "MQTT Interface Init");
		// memcpy(&client->configs, _cfg, sizeof(struct mqtt_client_cfg));
	client->configs = _cfg;
	// client->telemetry.get_telemetry = _get_telemetry;
	xTaskCreate(MQTTClientProcess, "things-board", MQTT_INTERFACE_STACK_SIZE,
				client, MQTT_INTERFACE_PRIORITY, (TaskHandle_t *)&client->task);
	// xTaskCreate(PowerDownNotify, "Power Notify", 1024, client, configMAX_PRIORITIES - 1, NULL);
}

/**
 * @brief Processes client tasks.
 *
 * This function handles the main processing loop or tasks for the client.
 * It may include communication with the server, handling incoming messages,
 * sending telemetry, and managing connection state.
 *
 * @param arg Pointer to user-defined data or context required for processing.
 */
static void MQTTClientProcess(void *arg)
{

	// event_get(PPPOS_NET_CONNECTED_BIT, true, portMAX_DELAY);

	mqtt_interface_t *client = (mqtt_interface_t *)arg;
	log_info(TAG, "MQTT Initialize: %s Server", client->configs->host);
	EventBits_t mqtt_publish_event;
	client->events = xEventGroupCreate();


	Device_State_t state = device_get_state();

	while(state != CONNECTED_NETWORK)
	{
		vTaskDelay(3000);
		log_info(TAG, "Waiting for network...%d", state);
		state = device_get_state();
	}

	device_set_state(CONNECTING_SERVER);
connecting:
	log_info(TAG, "Connecting to %s , port %d", client->configs->host, client->configs->port);
	client->ip.addr = 0;
	err_t err = dns_gethostbyname((const char *)client->configs->host, &client->ip,
								  dns_found_cb, client);
	uint32_t timeout = 0;
	while (client->ip.addr == 0 && timeout++ < 60000)
	{
		vTaskDelay(1);
	}
	if (err == ERR_OK && client->ip.addr > 0)
		mqtt_do_connect(client);
	else
	{
		log_error(TAG, "Can't get ip from host %s. Error: %d", client->configs->host, err);
		goto connecting;
	}
	while (1)
	{
		if (mqtt_client_is_connected(p_client(client)))
		{
			mqtt_publish_event = xEventGroupWaitBits(client->events,
													 MQTT_PUBLISH_SUCCESS_BIT | 
													 MQTT_PUBLISH_FAIL_BIT | 
													 MQTT_CONNECTED_BIT | 
													 MQTT_DISCONNECTED_BIT			
													 , pdFALSE, pdFALSE,
													 portMAX_DELAY);

			if (mqtt_publish_event & MQTT_PUBLISH_SUCCESS_BIT || mqtt_publish_event & MQTT_CONNECTED_BIT) 
			{
				if(client->msgPub.state == MQTT_PUB_DOING)
				{
					char *payload = client->msgPub.payload;
					char *topic = client->msgPub.topic;
					uint8_t qos = client->msgPub.qos;
					if ((payload != NULL) && (topic != NULL ))
					{
						// log_info(TAG, "Pub payload: %s", payload);
						xEventGroupClearBits(client->events, mqtt_publish_event);
						if (mqtt_publish(p_client(client), topic, payload,
										strlen(payload), qos, 0, mqtt_pub_request_cb, client) != 0)
						{
							log_error(TAG, "Publish false");

						}
						client->msgPub.state = MQTT_PUB_WAITING;
						// vTaskDelay(pdMS_TO_TICKS(MQTT_PUSHLISH_TIME));
					}	
					else {
						log_error(TAG, "Topic or payload are NULL!");
					}
				}
				vTaskDelay(1);

			}
			else
			{
				 xEventGroupClearBits(client->events,
								 MQTT_PUBLISH_SUCCESS_BIT | 
								 MQTT_PUBLISH_FAIL_BIT | 
								 MQTT_CONNECTED_BIT | 
								 MQTT_DISCONNECTED_BIT);
				log_error(TAG, "Disconnect form host %s", client->configs->host);
				mqtt_disconnect(p_client(client));
				vTaskDelay(pdMS_TO_TICKS(2000));
				mqtt_do_connect(client);
			}
		}
		else
			vTaskDelay(1);
	}

	// DELETE_TASK:
	//	vTaskDelete((TaskHandle_t)client->task);
}

/**
 * @brief Establishes a connection to the MQTT broker for the ThingsBoard client.
 *
 * This function initiates the MQTT connection process for the specified ThingsBoard client instance.
 * It handles the necessary steps to connect to the broker, including authentication and session setup.
 *
 * @param client Pointer to the mqtt_interface_t client structure.
 * @return err_t Error code indicating the result of the connection attempt.
 *         ERR_OK on success, or an appropriate error code on failure.
 */
static err_t mqtt_do_connect(mqtt_interface_t *client)
{
	Device_State_t state = device_get_state();
	if (state == REQUEST_NETWORK) return ERR_CONN;
	struct mqtt_connect_client_info_t ci;
	err_t err;
	log_info(TAG, "Connect to host : %s", ip_ntoa(&client->ip));
	memset(&ci, 0, sizeof(ci));
	memset(&client->base, 0, sizeof(mqtt_client_t));
	ci.client_id = client->configs->clientID;
	ci.client_user = client->configs->username;
	ci.client_pass = client->configs->password;
	ci.keep_alive = 60;
	// ci.will_msg = "{\"method\": \"thing.event.evt_Offline.post\"}";
	// ci.will_qos = 1;
	// ci.will_retain = 1;
	// ci.will_topic = "/WILL";
	err = mqtt_client_connect(&client->base, &client->ip,
							  client->configs->port, mqtt_connection_cb, client, &ci);
	if (err != ERR_OK)
	{
		log_error(TAG, "mqtt_connect return %d", err);
		xTaskCreate(mqtt_reconnect_task, "mqtt reconnect", 4096, client, 10, NULL);
	}
	return err;
}
/**
 * @brief Callback function invoked upon MQTT connection events.
 *
 * This function is called when the MQTT client establishes or loses a connection.
 *
 * @param client Pointer to the MQTT client instance.
 * @param arg    User-defined argument passed to the callback.
 */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
							   mqtt_connection_status_t status)
{
	mqtt_interface_t *server = (mqtt_interface_t *)arg;
	if (status == MQTT_CONNECT_ACCEPTED)
	{
		log_info(TAG, "mqtt_connection_cb: %s successfully connected", server->configs->host);
		// log_info(TAG, "Subscribe to topic %s", thingsboard->sub_topic);
		if (strlen(server->sub_topic) > 0) // When disconnected, need to sub again.
		{
			mqtt_subscribe(client, server->sub_topic, 1,
						mqtt_sub_request_cb, arg);
		}
//		if (server->srv_mqtt_sub_cb != NULL)
//		{
//			server->srv_mqtt_sub_cb();
//		}
		
		// mqtt_subscribe(client, RPC_REQUEST_API, 1,
		// 			   mqtt_sub_request_cb, arg);
		// mqtt_subscribe(client, ATTRIBUTE_REQUEST_API, 1,
		// 			   mqtt_sub_request_cb, arg);
		// mqtt_subscribe(client, ATTRIBUTE_FW_REQUEST_API, 1,
		// 				mqtt_sub_request_cb, arg);
		// mqtt_subscribe(client, ATTRIBUTE_UPDATE_API, 1,
		// 			   mqtt_sub_request_cb, arg);

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb,
								mqtt_incoming_data_cb, server);
		xEventGroupSetBits(server->events, MQTT_CONNECTED_BIT);
		device_set_state(CONNECTED_SERVER);
	}
	else
	{
		log_error(TAG, "mqtt_connection_cb: Disconnected, reason: %d",
				  status);
//		if (status == MQTT_CONNECT_DISCONNECTED) event_set(MQTT1_DISCONNECT_BIT);
		/* Its more nice to be connected, so try to reconnect */
		xEventGroupSetBits(server->events, MQTT_DISCONNECTED_BIT);
		xEventGroupClearBits(server->events, MQTT_CONNECTED_BIT);
		device_set_state(DISCONECT_SERVER);
		xTaskCreate(mqtt_reconnect_task, "mqtt reconnect", 4096, server, 10, NULL);
		
	}
}
/**
 * @brief Callback function for MQTT subscription requests.
 *
 * This function is called when an MQTT subscription request completes.
 *
 * @param arg   User-defined argument passed to the callback.
 * @param result Result of the subscription request (err_t).
 */
static void mqtt_sub_request_cb(void *arg, err_t result)
{
	/* Just print the result code here for simplicity,
	 normal behaviour would be to take some action if subscribe fails like
	 notifying user, retry subscribe or disconnect from server */
	mqtt_interface_t *thingsboard = (mqtt_interface_t *)arg;
	log_info(TAG, "Subscribe %s result: %d",thingsboard->sub_topic, result);
}
/**
 * @brief Callback function invoked when an MQTT PUBLISH message is received.
 *
 * This function is called by the MQTT client library whenever a new message is published
 * to a topic that the client is subscribed to. It provides the topic name and allows
 * processing of the incoming message.
 *
 * @param arg   User-defined argument passed to the callback (can be NULL).
 * @param topic The topic string on which the message was published.
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic,
									 u32_t tot_len)
{

	mqtt_interface_t *client = (mqtt_interface_t *)arg;
	client->msgSub.topic = (char *)topic;
	client->msgSub.topic_len = tot_len;
}
/**
 * @brief Callback function for handling incoming MQTT data.
 *
 * This function is called when new data is received from the MQTT broker.
 *
 * @param arg   User-defined argument passed to the callback.
 * @param data  Pointer to the received data buffer.
 * @param len   Length of the received data in bytes.
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
								  u8_t flags)
{
	mqtt_interface_t *client = (mqtt_interface_t *)arg;
	client->msgSub.payload = (char *)data;
	client->msgSub.payload_len = len;
	mqtt_subscribe_handle(client);
}
/**
 * @brief Callback function invoked upon completion of an MQTT publish request.
 *
 * This function is called when an MQTT publish operation completes, either successfully
 * or with an error. It can be used to handle post-publish logic such as resource cleanup,
 * logging, or triggering subsequent actions.
 *
 * @param arg    User-defined argument passed to the callback.
 * @param result Result of the publish operation (err_t). Indicates success or type of error.
 */
static uint8_t reconnect = 0;
static void mqtt_pub_request_cb(void *arg, err_t result)
{
	mqtt_interface_t *client = (mqtt_interface_t *)arg;
	client->msgPub.payload_len = 0;
	
	if (result == 0)
	{
		client->msgPub.payload_len = 0;
		log_info(TAG, "Publish success", result);
		xEventGroupSetBits(client->events, MQTT_PUBLISH_SUCCESS_BIT);
		client->msgPub.state = MQTT_PUB_SUCCES;
		return;
	}
	reconnect++;
	log_error(TAG, "Publish fail with result: %d\r\n", result);
	client->msgPub.state = MQTT_PUB_FAIL;
	xEventGroupSetBits(client->events, MQTT_PUBLISH_FAIL_BIT);
	xEventGroupClearBits(client->events, MQTT_PUBLISH_SUCCESS_BIT);
	if (reconnect > 1)
	{
		reconnect = 0;
		//device_set_state(DISCONECT_SERVER);
	}
	// device_set_state(DISCONECT_SERVER);
}
/**
 * @brief Handles MQTT subscription logic for the ThingsBoard client.
 *
 * This static function manages the process of subscribing to MQTT topics
 * relevant to the ThingsBoard client. It is intended for internal use within
 * the ThingsBoard client implementation.
 *
 * @param client Pointer to the mqtt_interface_t structure representing the client instance.
 */

static void mqtt_subscribe_handle(mqtt_interface_t *client)
{
	log_info(TAG, "Incoming publish at topic %s with total length %u",
			 client->msgSub.topic, (unsigned int)client->msgSub.topic_len);
	log_info(TAG, "Incoming publish payload with length %d", client->msgSub.payload_len);

	// tb_ota_parse_firmware_info(client->msgSub.topic, client->msgSub.payload);
	// tb_ota_parse_firmware_chunk(client->msgSub.topic, 
	// 	client->msgSub.topic_len,
	// 	client->msgSub.payload, 
	// 	client->msgSub.payload_len);

	// pcloud_handle_ctrl_service_json(client->msgSub.topic, client->msgSub.payload);
	
	memset(client->msgSub.payload, 0, client->msgSub.payload_len);
	memset(client->msgSub.topic, 0, client->msgSub.topic_len);
}
/**
 * @brief Task function to handle MQTT reconnection logic.
 *
 * This function is intended to be run as a separate task/thread. It manages
 * the reconnection process to the MQTT broker in case the connection is lost.
 * The function typically includes logic to attempt reconnection at regular
 * intervals, handle connection errors, and ensure the MQTT client remains
 * connected.
 *
 * @param arg Pointer to user-defined data or task parameters (can be NULL).
 */
static void mqtt_reconnect_task(void *arg)
{
	mqtt_interface_t *thingsboard = (mqtt_interface_t *)arg;
	while (1)
	{
		device_set_state(RECONECT_SERVER);
		mqtt_disconnect(p_client(thingsboard));
		vTaskDelay(pdMS_TO_TICKS(2000));
		err_t err = mqtt_do_connect(thingsboard);
		if (err == ERR_OK)
			break;
	}
	vTaskDelete(NULL);
}
