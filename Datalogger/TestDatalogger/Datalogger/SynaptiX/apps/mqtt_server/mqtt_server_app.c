#include "mqtt_server_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "logger.h"
#include "mqtt_interface.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static char const *TAG = "server";
static struct mqtt_client_cfg client_cfg;
static mqtt_interface_t client;

static TaskHandle_t data_packet_handle = NULL;
static TaskHandle_t heartbeat_handle = NULL;
static SemaphoreHandle_t xPubMutex = NULL;

static int create_msu_json(char *json, uint16_t len, MSU_DataPacket *packet);
static int create_heartbeat_json(char *json, uint16_t len, MSU_HeartbeatPacket *packet);
static void data_packet_task(void *arg);
static void heartbeat_task(void *arg);
static void mqtt_public(char *payload, char *topic, uint8_t qos);

void mqtt_server_init(device_config_t *sys_config)
{
    memset(&client_cfg, 0, sizeof(struct mqtt_client_cfg));
	// if ((sys_config->mqtt.url[0] == '\0') || (sys_config->mqtt.username[0] == '\0') || (sys_config->mqtt.password[0] == '\0')) {
		log_warn(TAG, "MQTT configuration is not set in sys_config");
		strcpy(client_cfg.clientID,"SynaptiX" );
		strcpy(client_cfg.host,"27.71.27.73");
		strcpy(client_cfg.username, "root");
		strcpy(client_cfg.password, "lottte135@");
		strcpy(client.pub_topic,"tte/up");
		strcpy(client.sub_topic,"tte/down");
		client_cfg.port = MQTT_PORT;
	// } else {
	// 	strcpy(client_cfg.host, sys_config->mqtt.url);
	// 	strcpy(client_cfg.username, sys_config->mqtt.username);
	// 	strcpy(client_cfg.password, sys_config->mqtt.password);
	// 	client_cfg.port = sys_config->mqtt.port;
	// }

    client_cfg.keepAlive = MQTT_KEEP_ALIVE_S;
//    client = &client;
    client.msgPub.payload_len = 0;
    client.msgPub.topic_len = 0;
    mqtt_interface_init(&client, &client_cfg);

    xPubMutex = xSemaphoreCreateMutex();
    if (xPubMutex == NULL) {
        log_error(TAG, "Failed to create mutexes for public to Partner Cloud");
        return;
    }

    // xTaskCreate(data_packet_task, "data_packet_task", 256*10, &sys_config->dev, 6, &data_packet_handle);
    // if(data_packet_handle == NULL){
    //     log_error(TAG,"data_packet_task task create false");
    // }
    // xTaskCreate(heartbeat_task, "heartbeat_task", 256*6, &sys_config->dev, 4, &heartbeat_handle);
    // if(heartbeat_handle == NULL){
    //     log_error(TAG,"heartbeat_task task create false");
    // }
}

static void data_packet_task(void *arg)
{
    msu_t *device = (msu_t *)arg;
    while (1)
    {
        char msg[MQTT_PUB_MSG_SIZE];
        create_heartbeat_json(msg, MQTT_PUB_MSG_SIZE, &device->data_packet);
        mqtt_public(msg, "topic", 1);
        vTaskDelay(pdMS_TO_TICKS(device->data_packet.period));
    }
}

static void heartbeat_task(void *arg)
{
    msu_t *device = (msu_t *)arg;
    while (1)
    {
        char msg[MQTT_PUB_MSG_SIZE];
        create_heartbeat_json(msg, MQTT_PUB_MSG_SIZE, &device->heartbeat_packet);
        mqtt_public(msg, "topic", 1);
        vTaskDelay(pdMS_TO_TICKS(device->heartbeat_packet.period));
    }
}

static int create_msu_json(
    char *json, 
    uint16_t len,
    MSU_DataPacket *packet) 
{
    if ((json == NULL) || (packet == NULL))
    {
        log_error(TAG, "Failed to create cJson for device information [%s]", packet->deviceID);
        return false;
    }

    // Get time now (ISO 8601 standard)
    time_t now = time(NULL);
    struct tm *utc_time = gmtime(&now);
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", utc_time);

    // Create root object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }

    cJSON_AddStringToObject(root, "deviceID", packet->deviceID);
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "motionState", packet->motionState);

    // GPS
    cJSON *location = cJSON_CreateObject();
    if (location == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddNumberToObject(location, "latitude", packet->location.latitude);
    cJSON_AddNumberToObject(location, "longitude", packet->location.longitude);
    cJSON_AddItemToObject(root, "location", location);

    // Sensor data
    cJSON *sensors = cJSON_CreateObject();
    if (sensors == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }

    cJSON_AddNumberToObject(sensors, "pm25", packet->sensors.pm25);
    cJSON_AddNumberToObject(sensors, "pm10", packet->sensors.pm10);
    cJSON_AddNumberToObject(sensors, "co2", packet->sensors.co2);
    cJSON_AddNumberToObject(sensors, "so2", packet->sensors.so2);
    cJSON_AddNumberToObject(sensors, "co", packet->sensors.co);
    cJSON_AddNumberToObject(sensors, "o3", packet->sensors.o3);
    cJSON_AddItemToObject(root, "sensors", sensors);

    // Environment data
    cJSON *environment = cJSON_CreateObject();
    cJSON_AddNumberToObject(environment, "temperature", packet->environment.temperature);
    cJSON_AddNumberToObject(environment, "humidity", packet->environment.humidity);
    cJSON_AddItemToObject(root, "environment", environment);

    cJSON_PrintPreallocated(root, json, len, false);
    cJSON_Delete(root); 

    return 0;
}


static int create_heartbeat_json(
    char *json, 
    uint16_t len,
    MSU_HeartbeatPacket *packet) {
    // Get time now (ISO 8601 standard)
    time_t now = time(NULL);
    struct tm *utc_time = gmtime(&now);
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", utc_time);

    // Root JSON
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddStringToObject(root, "deviceID", packet->deviceID);
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "firmwareVersion", packet->firmwareVersion);
    cJSON_AddNumberToObject(root, "uptime", packet->uptime);

    // Network
    cJSON *network = cJSON_CreateObject();
    if (network == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddNumberToObject(network, "signalStrength", packet->network.signalStrength);
    cJSON_AddStringToObject(network, "operator", packet->network.operator);
    cJSON_AddItemToObject(root, "network", network);

    // Power
    cJSON *power = cJSON_CreateObject();
    if (power == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddStringToObject(power, "source", packet->power.source);
    cJSON_AddNumberToObject(power, "batteryLevel", packet->power.batteryLevel);
    cJSON_AddItemToObject(root, "power", power);

    // Memory
    cJSON *memory = cJSON_CreateObject();
    if (memory == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddNumberToObject(memory, "storageUsed", packet->memory.storageUsed);
    cJSON_AddItemToObject(root, "memory", memory);

    // GPS
    cJSON *gps = cJSON_CreateObject();
    if (gps == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    cJSON_AddNumberToObject(gps, "satellites", packet->gps.satellites);
    cJSON_AddBoolToObject(gps, "fix", packet->gps.fix);
    cJSON_AddItemToObject(root, "gps", gps);

    // Sensor Status
    cJSON *sensorStatus = cJSON_CreateArray();
    if (sensorStatus == NULL)
    {
        log_error(TAG, "Failed to create cJson for device [%s]", packet->deviceID);
        return -1;
    }
    for (int i = 0; i < packet->sensorCount; i++) {
        cJSON *sensor = cJSON_CreateObject();
        cJSON_AddStringToObject(sensor, "name", packet->sensorStatus[i].name);
        cJSON_AddStringToObject(sensor, "status", packet->sensorStatus[i].status);
        cJSON_AddItemToArray(sensorStatus, sensor);
    }
    cJSON_AddItemToObject(root, "sensorStatus", sensorStatus);

    cJSON_PrintPreallocated(root, json, len, false);
    cJSON_Delete(root); 
    return 0;
}

static void mqtt_public(char *payload, char *topic, uint8_t qos)
{
    // vTaskDelay(pdMS_TO_TICKS(250));
//    if (client == NULL){
//        log_error(TAG, "mqtt_public: Client or topic is NULL");
//        return;
//    }
    xSemaphoreTake(xPubMutex, portMAX_DELAY);
    // log_info(TAG, "pcloud_mqtt_public: Publishing message for device [%d] with payload: %s", deviceID, payload);
    char full_topic[128];
    // char topic[PCLOUD_TOPIC_SIZE];

    uint8_t retry_count = 0;
    uint16_t timeout = 0;

    // START_PORTING 
    // if (topic != NULL) {
    //     sprintf(full_topic, "%s/%s/", client.pub_topic, topic);
    //     client.msgPub.topic = full_topic;
    // } else {
    //     client.msgPub.topic = client.pub_topic;
    // }
    // END_PORTING 

    client.msgPub.topic_len = strlen(client.pub_topic);
    client.msgPub.payload = payload;
    client.msgPub.payload_len = strlen(payload);
    client.msgPub.qos = qos;

retry:
    client.msgPub.state = MQTT_PUB_DOING;
    while ((client.msgPub.state == MQTT_PUB_DOING) ||
        (client.msgPub.state == MQTT_PUB_WAITING))
    {
        // Wait for the previous message to be sent
        vTaskDelay(pdMS_TO_TICKS(10));
        timeout++;
        if (timeout > 500) // Check case: mqtt disconect
        {
            log_debug(TAG, "Public timeout 5s => Public fail");
            client.msgPub.state = MQTT_PUB_FAIL;
            break;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(250));
    if ((MQTT_PUB_FAIL == client.msgPub.state) && (retry_count < 3))
    {
        timeout = 0;
        retry_count++;
        goto retry;
    }
    client.msgPub.state == MQTT_PUB_IDLE;
    xSemaphoreGive(xPubMutex);
}
