#ifndef ENVIRONTMENT_H_
#define ENVIRONTMENT_H_
#ifdef __cplusplus
extern "C" {
#endif    
	#define MAX_SENSOR 10

// Data packet
    typedef struct {
        double latitude;
        double longitude;
    } Location;

    typedef struct {
        float pm25;
        float pm10;
        float co2;
        float so2;
        float co;
        float o3;
    } SensorData;

    typedef struct {
        float temperature;
        int humidity;
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

// Hearrtbeat packet
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

    typedef struct {
        MSU_DataPacket data_packet;
        MSU_HeartbeatPacket heartbeat_packet;
    } MSU_t;
int getAQI(SensorData *data);
#ifdef __cplusplus
}
#endif   

#endif /* ENVIRONTMENT_H_ */
