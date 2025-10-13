#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

void button_init();
void button_poll(int time_stamp);

#ifdef __cplusplus
}
#endif

#endif