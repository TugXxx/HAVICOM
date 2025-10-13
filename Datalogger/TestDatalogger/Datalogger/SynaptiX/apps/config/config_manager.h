#ifndef CONFIG_MANAGER_H_
#define CONFIG_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include "logger.h"

	#define MAX_SENSOR 10

    typedef enum {
        MOVING_MODE = 0x00,
        PARKED_MODE = 0x01
    } VehiceMode;

    typedef struct {
        float latitude;
        float longitude;
    } Location;

    typedef struct {
        float pm25;
        float pm10;
        int co2;
        float so2;
        float co;
        float o3;
    } SensorData;

    typedef struct {
        float temperature;
        float humidity;
    } EnvironmentData;

    typedef struct {
        char deviceID[32];
        char timestamp[25];      // ISO 8601 format
        char motionState[16];    // "moving" or "parked"
        Location location;
        SensorData sensors;
        EnvironmentData environment;
        int period;
    } MSU_DataPacket;

    typedef struct {
        int signalStrength;      // RSSI (dBm)
        char operator[32];       // Tên nhà mạng
    } NetworkInfo;

    typedef struct {
        char source[16];         // "vehicle" or "battery"
        float batteryLevel;      // %
    } PowerInfo;

    typedef struct {
        int storageUsed;         // %
    } MemoryInfo;

    typedef struct {
        int satellites;
        int fix;                 // 0 = false, 1 = true
    } GPSInfo;

    typedef struct {
        char name[16];
        char status[16];         // "OK", "ERROR", "UNKNOWN"
    } SensorStatus;

    typedef struct {
        char deviceID[32];
        char timestamp[25];          // ISO 8601 format
        char firmwareVersion[16];
        int uptime;                  // Runtime
        NetworkInfo network;
        PowerInfo power;
        MemoryInfo memory;
        GPSInfo gps;
        SensorStatus sensorStatus[MAX_SENSOR]; // MAX_SENSOR
        int sensorCount;
        int period;
    } MSU_HeartbeatPacket;

    typedef struct device
    {
        LOGGING_LEVELS log_level;
        // uint16_t data_period; 
        // char fw_version[16];
        // char hw_version[16];

        // char ProductKey[30];
        // char Production_Number[30];
        // char Production_Date[30];
        MSU_DataPacket data_packet;
        MSU_HeartbeatPacket heartbeat_packet;
        VehiceMode mode;
    } msu_t;

    // struct mbserial
    // {
    //     enum mbMode{
    //         RTU = 0,
    //         ASCII = 1
    //     } mode; // 0:RTU, 1:ASCII
    //     enum mbParity{
    //         PAR_NONE = 0,
    //         PAR_ODD,
    //         PAR_EVEN
    //     } parity; // 0:none, 1:odd, 2:even
    //     enum mbStopbits{
    //         STOP_ONE = 1,
    //         STOP_TWO
    //     } stopbits;
    //     enum mbDatabits{
    //         DATA_8_Bits = 8,
    //         DATA_9_Bits = 9
    //     } databits;
    //     uint32_t baudrate;
    // };

    struct mqtt
    {
        char clientID[32];
        char password[32];
        char username[32];
        int port;
        char url[64];
        char pub_topic[64];
        char sub_topic[64];
    };

    // struct http
    // {
    //     char url[100];
    //     char token[64];
    //     char user_name[64];
    //     char password[64];
    //     int port;
    // };

    // Device configuration structure (same as before but extended)
    typedef struct
    {
        struct device dev;
        // struct mbserial mb_serial;
        struct mqtt mqtt;
        // struct http http;
        uint32_t hash;
    } device_config_t;

    /**
     * @brief Load configuration from persistent storage
     * @param config Pointer to application configuration structure
     */
    int load_config(device_config_t *config);

    /**
     * @brief Save configuration to persistent storage
     * @param config Pointer to application configuration structure
     */
    int save_config(device_config_t *config);

    /**
     * @brief Remove the current configuration from persistent storage
     * @param file_path Path to the configuration file to be removed
     * @return bool True if configuration was successfully removed, false otherwise
     */
    bool remove_config(char *file_path);

    /**
     * @brief Reset the application configuration to default settings
     * @param arg Pointer to the application configuration structure to reset
     */
    void reset_config(void *arg);

    /**
     * @brief Log the details of the application configuration
     * @param arg Pointer to the application configuration structure to log
     */
    void log_config_info(void *arg);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_MANAGER_H_
