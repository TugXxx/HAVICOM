#ifndef __APN_H
#define __APN_H

#ifdef __cplusplus
extern "C" {
#endif



typedef struct {
    const char *name; // APN name
    const char *apn;
    const char *username;
    const char *password;
} APN_Config_t;


APN_Config_t *get_apn(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* __APN_H */