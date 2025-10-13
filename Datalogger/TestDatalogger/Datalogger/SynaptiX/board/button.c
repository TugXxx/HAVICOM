#include "button.h"
#include "logger.h"


static const char *TAG = "Button";

uint32_t button_tick = 0;

#define TIME_DEBOUND_CLICK_mS 200
#define TIME_DEBOUND_HOLD_mS 2000

void button_init(){
    log_info(TAG,"Button init...");
}
void button_poll(int time_stamp){
   if(1 == bsp_button_status()){
        button_tick+=time_stamp;
   }
   if(button_tick > 0 && bsp_button_status() == 0){
        button_tick-=time_stamp;
        if(button_tick == 0) {
            log_info(TAG,"Clicked");
            bsp_restart();
        }
   }
   if(button_tick == TIME_DEBOUND_HOLD_mS){
        log_info(TAG,"Holded");
        bsp_restart();
   }
   if(button_tick > TIME_DEBOUND_HOLD_mS && 0 == bsp_button_status()){
        button_tick = 0;
   }
}