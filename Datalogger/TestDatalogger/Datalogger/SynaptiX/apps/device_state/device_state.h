#ifndef _DEVICE_STATE
#define _DEVICE_STATE
#ifdef __cplusplus
extern "C" {
#endif

typedef enum Device_State{
    POWER_ON = 0,
    POWER_OFF,
    REQUEST_NETWORK,
    CONNECTED_NETWORK,
    CONNECTING_SERVER,
    CONNECTED_SERVER,
    DISCONECT_SERVER,
    RECONECT_SERVER,
    DOWNLOAD_FIRMWARE,
    UPDATE_FIRMWARE,
    ERROR_HANDLE
}Device_State_t;

void device_state_init();
void device_set_state(Device_State_t state);
Device_State_t device_get_state();

#ifdef __cplusplus
}
#endif
#endif /* _DEVICE_STATE */