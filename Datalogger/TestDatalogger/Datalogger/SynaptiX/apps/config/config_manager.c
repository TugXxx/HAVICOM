//#include "config_manager.h"
////#include "file_io.h"
//#include "logger.h"
//
//#define FILE_CONFIG_PATH "/config.json"
//#define CONFIG_HASH 0x12345679
//
//static const char *TAG = "config_manager";
//
//static void load_config_default(device_config_t *config){
//    strcpy(config->dev.data_packet.deviceID,"MSU-HN-001");
//    config->dev.log_level = LOGGER_INFO;
//    config->dev.lattitude = 0;
//    config->dev.longtitude = 0;
//
//    config->dev.data_packet.period = 200;// seconds
//    config->dev.data_packet.location.latitude = 0;
//    config->dev.data_packet.location.longitude = 0;
//    strcpy(config->dev.data_packet.motionState, "parked");
//
//    config->dev.heartbeat_packet.period = 900;// 15 minutes
//    strcpy(config->dev.heartbeat_packet.deviceID, "MSU-HN-001");
//    strcpy(config->dev.heartbeat_packet.firmwareVersion, "1.0.0");
//    strcpy(config->dev.heartbeat_packet.network.operator, "NULL");
//    strcpy(config->dev.heartbeat_packet.network.signalStrength, 0);
//    strcpy(config->dev.heartbeat_packet.power.source, "NULL");
//    config->dev.heartbeat_packet.power.batteryLevel = 0;
//
//    config->dev.mode = PARKED_MODE;
//
//
//    // config->mb_serial.mode = RTU;
//    // config->mb_serial.parity = PAR_EVEN;
//    // config->mb_serial.stopbits = STOP_ONE;
//    // config->mb_serial.databits = DATA_9_Bits;
//    // config->mb_serial.baudrate = 9600;
//    strcpy(config->mqtt.clientID,"nh49c0wlw6t05gqssmtz");
//    strcpy(config->mqtt.username,"root");
//    strcpy(config->mqtt.password,"lottte135@");
//    strcpy(config->mqtt.url,"27.71.27.73");
//    strcpy(config->mqtt.pub_topic,"tte/up");
//    strcpy(config->mqtt.sub_topic,"tte/down");
//    config->mqtt.port = 1883;
//    config->hash = CONFIG_HASH;
//}
//
//int load_config(device_config_t *config){
//    uint8_t retry_cnt = 0; // Fix bug: sometimes, open file is fail.
//retry:
//    FILE *fp = fopen(FILE_CONFIG_PATH, "r");
//    if (fp == NULL) {
//        retry_cnt++;
//        if (retry_cnt <= 5) { // Fix bug: sometimes, open file is fail.
//            log_warn(TAG,"Opening config file, retry ..%d", retry_cnt);
//            vTaskDelay(pdMS_TO_TICKS(100));
//            goto retry;
//        }
//        log_error(TAG,"Failed to open config file");
//        // Load default configuration if file doesn't exist
//        load_config_default(config);
//        if (save_config(config)) {
//            log_error(TAG,"Failed to save default configuration");
//            return -1;
//        }
//    }
//    int len = fread(config,1, sizeof(device_config_t), fp);
//    log_info(TAG,"Len = %d",len);
//    log_info(TAG,"Sizeof : %d",sizeof(device_config_t));
//    if (len != sizeof(device_config_t) || config->hash != CONFIG_HASH) {
//        log_error(TAG,"Failed to read config file");
//        load_config_default(config);
//        if (save_config(config)) {
//            log_error(TAG,"Failed to save default configuration");
//            return -1;
//        }
//        fclose(fp);
//        return -1;
//    }
//    log_info(TAG,"Load config success");
//    fclose(fp);
//    return 0;
//}
//
//int save_config(device_config_t *config){
//    FILE *fp = fopen(FILE_CONFIG_PATH, "w");
//    if (fp == NULL) {
//        log_error(TAG,"Failed to open config file for writing");
//        return -1;
//    }
//
//    int len = fwrite(config,1, sizeof(device_config_t),  fp);
//    if (len != sizeof(device_config_t)) {
//        fclose(fp);
//        log_error(TAG,"Failed to write config file");
//        return -1;
//    }
//    fclose(fp);
//    fp = fopen(FILE_CONFIG_PATH,"r");
//    if(fp == NULL){
//        log_error(TAG,"Failed to open config file for reading");
//        return -2;
//    }
//
//    len = fread(config,1,sizeof(device_config_t),fp);
//    if(len != sizeof(device_config_t) || config->hash != CONFIG_HASH){
//        fclose(fp);
//        log_error(TAG,"Failed to write config file");
//        return -1;
//    }
//    log_info(TAG,"Save success");
//    return 0;
//}
//
//
//bool remove_config(char *file_path) {
//    FILE *fp = fopen(file_path, "r");
//    if (fp != NULL) {
//        fclose(fp);
//        log_info(TAG, "File %s does exist.", file_path);
//        if (remove(file_path) == 0) {
//            log_info(TAG, "File %s has been deleted successfully.", file_path);
//        } else {
//            log_error(TAG, "Failed to delete the file %s.", file_path);
//            return false;
//        }
//    } else {
//        log_info(TAG, "File %s does not exist.", file_path);
//        return false;
//    }
//    return true;
//}
//
//void reset_config(void *arg) {
//    const device_config_t *config = (const device_config_t *)arg;
//    if (remove_config(FILE_CONFIG_PATH)) {
//        load_config_default(config);
//        save_config(config);
//        log_info(TAG, "Reset config to default has been reset successfully.");
//    } else {
//        log_error(TAG, "Failed to reset config to default.");
//    }
//}
//
//void log_config_info(void *arg) {
//
//}
