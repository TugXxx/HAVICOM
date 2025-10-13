#include "apn.h"
#include <string.h>
#define MAX_APN_CONFIGS 8

static APN_Config_t apns[] = {
    {"Viettel", "v-internet",NULL,NULL},
    {"MobiFone", "m-wap","mms","mms"},
    {"Vinaphone", "m3-world","mms","mms"},
    {"Vietnammobile", "internet",NULL,NULL},
	{"VINAPHONE", "m3-world","mms","mms"},
	{"VIETTEL", "v-internet",NULL,NULL},
	{"MOBIFONE", "m-wap","mms","mms"},
	{"VIETNAMMOBILE", "internet",NULL,NULL},
};


APN_Config_t *get_apn(const char *name){
    for (int i = 0; i < (sizeof(apns) / sizeof(APN_Config_t)); i++) {
        if (strcmp(apns[i].name, name) == 0) {
            return &apns[i];
        }
    }
    return NULL;
}
