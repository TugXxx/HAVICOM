#ifndef __A76XX_H
#define __A76XX_H

#include "modem.h"
#include "apn.h"

typedef struct {
    Modem_t modem;
    APN_Config_t *apn_config;
    char imei[20]; // IMEI of the A76xx modem
    char ccid[24]; // CCID of the A76xx modem
    int signal_strength; // Signal strength in dBm
    int rssi_level; // RSSI level (0-5)
    int ber; // Bit Error Rate
    char operator[32]; // Current operator name
} A76xx_t;
void a76xx_init(A76xx_t *a76xx, int uart, void (*reset)(), void (*power)(bool enable));
void a76xx_reset(A76xx_t *a76xx);
void a76xx_power_on(A76xx_t *a76xx);
void a76xx_power_off(A76xx_t *a76xx);
int a76xx_echo(A76xx_t *a76xx);
void a76xx_set_mode(A76xx_t *a76xx, Modem_Mode_t mode);
int a76xx_get_signal_strength(A76xx_t *a76xx);
char* a76xx_get_imei(A76xx_t *a76xx);
char* a76xx_get_ccid(A76xx_t *a76xx);
char* a76xx_get_operator(A76xx_t *a76xx);
char* a76xx_get_operator2(A76xx_t *a76xx);
int a76xx_set_operator(A76xx_t *a76xx);
int a76xx_set_sim_host_swap_level(A76xx_t *a76xx, int level);
int a76xx_define_PDP_context(A76xx_t *a76xx);
int a76xx_authen_PDP_context(A76xx_t *a76xx);
int a76xx_set_active_PDP_context(A76xx_t *a76xx, bool active);
int a76xx_registration_network(A76xx_t *a76xx);
int a76xx_operation_selection(A76xx_t *a76xx, uint8_t mode);
int a76xx_operation_get_mode(A76xx_t *a76xx);
int a76xx_get_ip_context(A76xx_t *a76xx);
#endif /* __A76XX_H */
