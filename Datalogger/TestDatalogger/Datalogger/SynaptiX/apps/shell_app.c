#include "cli_shell.h"
#include "board.h"
#include <stdio.h>
static int shell_send_char(void *arg,char c){
    (void) arg;
   return (int) bsp_com_write(BSP_DEBUG_COM_PORT,(uint8_t*)&c,1);
}

ShellContext_t shell;
// sCliShellImpl_t shell_impl = {.arg = NULL,.send_char = shell_send_char};
void shell_app_init();
void shell_app_poll();
void shell_app_init(){
    shell.impl.arg = NULL;
    shell.impl.send_char = shell_send_char;
    cli_shell_boot(&shell);
}

void shell_app_poll(){
    char c = 0;
    if(bsp_com_read(BSP_DEBUG_COM_PORT,(uint8_t*)&c,1) == 1){
        cli_shell_receive_char(&shell,c);
    }
}