

#include "cli_shell.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "app_settings.h"
#include "logger.h"
#include "app_config.h"

static const char *TAG = "CLI";

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static int cli_cmd_restart(ShellContext_t *shell, int argc, char *argv[]);
#if RS485_ENABLE || COM_ENABLE || RF_ENABLE
static int cli_serial_param(ShellContext_t *shell, int argc, char *argv[]);
#endif
#if APPLICATION_MODE_GPIO
static int cli_set_output(ShellContext_t *shell, int argc, char *argv[]);
static int cli_get_input(ShellContext_t *shell,int argc,char *argv[]);
#endif
#if TRANSFER_MODE_EN
static int cli_mode(ShellContext_t *shell,int argc,char *argv[]);
#endif
#if LORA_ENABLE
static int cli_lora(ShellContext_t *shell, int argc, char *argv[]);
#endif

#if ZIGBEE_ENABLE
static int cli_zigbee(ShellContext_t *shell, int argc, char *argv[]);
#endif

static const Cli_Shell_Cmd s_shell_commands[] = {
    {"help", cli_shell_help_handler, "Lists all commands\r\n"},
    {"restart", cli_cmd_restart, "Restart device\r\n"},
    #if RS485_ENABLE
    {"rs485", cli_serial_param, "\r\nrs485 "\
                                      "-i [info]\r\n"\
                                      "      -c [config] : \r\n"\
                                             "\t\t-b : baudrate[1200,2400,4800,9600,19200,38400,57600,115200]\r\n"\
                                             "\t\t-d : data bits[8]\r\n"\
                                             "\t\t-s : stopbits[1]\r\n"\
                                             "\t\t-p : parity[none,odd,even]\r\n"\
                                             "example : rs485 -c -b 115200 -d 8 -s 1 -p none\r\n"},
    #endif
    #if COM_ENABLE
    {"usb", cli_serial_param, "\r\nusb -i [info]\r\n"\
                                  "    -c [config] : \r\n"\
                                             "\t\t-b : baudrate[1200,2400,4800,9600,19200,38400,57600,115200]\r\n"\
                                             "\t\t-d : data bits[8]\r\n"\
                                             "\t\t-s : stopbits[1]\r\n"\
                                             "\t\t-p : parity[none,odd,even]\r\n"\
                                             "example : usb -c -b 115200 -d 8 -s 1 -p none\r\n"},
    #endif
    #if RF_ENABLE
    {"rf", cli_serial_param, "\r\nrf -i [info]\r\n"\
                                "   -c [config] : \r\n"\
                                "\t\t-b : baudrate[1200,2400,4800,9600,19200,38400,57600,115200]\r\n"\
                                "\t\t-d : data bits[8]\r\n"\
                                "\t\t-s : stopbits[1]\r\n"\
                                "\t\t-p : parity[none,odd,even]\r\n"\
                                "example : rf -c -b 115200 -d 8 -s 1 -p none\r\n"},
    #endif
    #if APPLICATION_MODE_GPIO
    {"output", cli_set_output, "output -o[output number] -on/off\r\nexample output 0 on\r\n"},
    {"input", cli_get_input,"input -i[input number] : Get input value\r\n"},
    #endif
    #if TRANSFER_MODE_EN
    {"mode",cli_mode,"mode -m[1-Parency Tranfer,2-Modbus Tranfer]\r\n"},
    #endif
#if LORA_ENABLE
    {"lora", cli_lora, "\r\nlora -c : Connect to Lora Module\r\n"\
                       "lora -r : Read parameter from Lora Module\r\n"\
                       "lora -rs : Restart Lora Module\r\n"\
                       "lora -lp : Enter low power mode\r\n" \
                       "lora -w : Write parameter to Lora Module\r\n"\
                                                                         "\t\t-p[panid] 0x0001 - 0xFF00\r\n"\
                                                                         "\t\t-c[channel]: 11-26\r\n"\
                                                                         "\t\t-add[address] 0x0001\r\n"\
                                                                         "\t\t-t[type] : 1-Coordinator,2-Router,3-End Device\r\n"\
                                                                         "\t\t-tm[transfer mode] : 1-Parency Transfer,2-Parency Transfer User Address,3-Parency Transfer Short Add,4-Parency Transfer Mac Address,5-Transfer N-N)\r\n"\
                                                                         "\t\t-b[baudrate] : 1-1200,2-2400,3-4800,4-9600,5-19200,6-38400,7-57600,8-115200\r\n"\
                                                                         "\t\t-d[databits] : 1-8bits\r\n"\
                                                                         "\t\t-s[stopbits] : 1-stop one\r\n"\
                                                                         "\t\t-p[parity]: 1-none,2-even,3-odd\r\n"\
                                                                         "\t\t-at[antenna] : 0-on board,1-external\r\n"\
                                                                         "\t\t-s[security] : 0-off,1-on\r\n"\
                                                                         "\t\t-sc[security code] : hex 4 bytes (0x01 0x02 0x03 0x04)\r\n" \
                       "lora -s : Send data to Lora Module [ -data 0x01 0x02 ... ]\r\n"},
#endif
#if ZIGBEE_ENABLE
    {"zigbee", cli_zigbee, "\r\nzigbee -c : Connect to Zigbee Module\r\n"\
                           "zigbee -r : Read parameter from Zigbee Module\r\n"\
                           "zigbee -rs : Restart Zigbee Module\r\n"\
                           "zigbee -lp : Enter low power mode\r\n" \
                           "zigbee -w : Write parameter to Zigbee Module:\r\n"\
                                                                         "\t\t-p[panid] 0x0001 - 0xFF00\r\n"\
                                                                         "\t\t-c[channel]: 11-26\r\n"\
                                                                         "\t\t-add[address] 0x0001\r\n"\
                                                                         "\t\t-t[type] : 1-Coordinator,2-Router,3-End Device\r\n"\
                                                                         "\t\t-tm[transfer mode] : 1-Parency Transfer,2-Parency Transfer User Address,3-Parency Transfer Short Add,4-Parency Transfer Mac Address,5-Transfer N-N)\r\n"\
                                                                         "\t\t-b[baudrate] : 1-1200,2-2400,3-4800,4-9600,5-19200,6-38400,7-57600,8-115200\r\n"\
                                                                         "\t\t-d[databits] : 1-8bits\r\n"\
                                                                         "\t\t-s[stopbits] : 1-stop one\r\n"\
                                                                         "\t\t-p[parity]: 1-none,2-even,3-odd\r\n"\
                                                                         "\t\t-at[antenna] : 0-on board,1-external\r\n"\
                                                                         "\t\t-s[security] : 0-off,1-on\r\n"\
                                                                         "\t\t-sc[security code] : hex 4 bytes (0x01 0x02 0x03 0x04)\r\n" \
                           "zigbee -s : Send data to Zigbee Module [ -d[data] 0x01 0x02 ... ]\r\n",
                           },
#endif
};

static int cli_cmd_restart(ShellContext_t *shell, int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  for (uint32_t i = 0; i < 2; i++)
  {
    cli_shell_printf(shell,"Device restart after %ld s\r\n", 2 - i);
    bsp_delay(1000);
  }
  cli_shell_put_line(shell, "Device restarted!");
  bsp_restart();
  return 0;
}

#if RS485_ENABLE || COM_ENABLE || RF_ENABLE
static int cli_serial_param(ShellContext_t *shell, int argc, char *argv[])
{

  Serial_t *serial = NULL;
#if COM_ENABLE
  if (strcmp(argv[0], "usb") == 0)
    serial = &app_setting.com;
#endif
#if RS485_ENABLE
  else if (strcmp(argv[0], "rs485") == 0)
    serial = &app_setting.rs485;
#endif
#if RF_ENABLE
  else if (strcmp(argv[0], "rf") == 0)
    serial = &app_setting.rf;
#endif
  if (argc == 2 && strcmp(argv[1], "-i") == 0)
  {
    cli_shell_printf(shell, "Baudrate : %lu\r\nData bits : %d\r\nStop bits: %d\r\nParity : %d\r\n", serial->baudrate, serial->databits == SERIAL_WORDLENGTH_8B ? 8 : 9, serial->stopbits == SERIAL_STOPBITS_1 ? 1 : 2, serial->parity);
  }
  else if ((argc > 2) && (strcmp(argv[1], "-c") == 0) && (argc % 2 == 0))
  {

    for (int i = 2; i < argc; i += 2)
    {
      cli_shell_printf(shell, "%s : %s", argv[i], argv[i + 1]);
      if (strcmp(argv[i], "-b") == 0)
      {
        if (strcmp(argv[i + 1], "115200") == 0)
          serial->baudrate = 115200;
        else if (strcmp(argv[i + 1], "57600") == 0)
          serial->baudrate = 57600;
        else if (strcmp(argv[i + 1], "38400") == 0)
          serial->baudrate = 38400;
        else if (strcmp(argv[i + 1], "19200") == 0)
          serial->baudrate = 19200;
        else if (strcmp(argv[i + 1], "9600") == 0)
          serial->baudrate = 9600;
        else if (strcmp(argv[i + 1], "4800") == 0)
          serial->baudrate = 4800;
        else if (strcmp(argv[i + 1], "2400") == 0)
          serial->baudrate = 2400;
        else if (strcmp(argv[i + 1], "1200") == 0)
          serial->baudrate = 1200;
        else
        {
          cli_shell_printf(shell, "Baudrate %s not support\r\n", argv[i + 1]);
          return -1;
        }
      }
      else if (strcmp(argv[i], "-d") == 0)
      {
        if (strcmp(argv[i + 1], "8") == 0)
          serial->databits = SERIAL_WORDLENGTH_8B;
        else if (strcmp(argv[i + 1], "9") == 0)
          serial->databits = SERIAL_WORDLENGTH_9B;
        else
        {
          cli_shell_printf(shell, "Databits %s not support\r\n", argv[i + 1]);
          return -1;
        }
      }
      else if (strcmp(argv[i], "-s") == 0)
      {
        if (strcmp(argv[i + 1], "1") == 0)
          serial->stopbits = SERIAL_STOPBITS_1;
        else if (strcmp(argv[i + 1], "2") == 0)
          serial->stopbits = SERIAL_STOPBITS_2;
        else
        {
          cli_shell_printf(shell, "Stopbits %s not support\r\n", argv[i + 1]);
          return -1;
        }
      }
      else if (strcmp(argv[i], "-p") == 0)
      {
        if (strcmp(argv[i + 1], "none") == 0)
          serial->parity = SERIAL_PARITY_NONE;
        else if (strcmp(argv[i + 1], "odd") == 0)
          serial->parity = SERIAL_PARITY_ODD;
        else if (strcmp(argv[i + 1], "even") == 0)
          serial->parity = SERIAL_PARITY_EVEN;
        else
        {
          cli_shell_printf(shell, "Parity %s not support\r\n", argv[i + 1]);
          return -1;
        }
      }
    }
    app_setting_save(&app_setting);
    cli_shell_printf(shell, "Save %s parameter success\r\n", argv[0]);
    app_setting_printf(&app_setting);
  }
  else
  {
    cli_shell_help_handler(shell, 0, NULL);
  }
  return 0;
}
#endif

#if APPLICATION_MODE_GPIO
static int cli_set_output(ShellContext_t *shell, int argc, char *argv[])
{
  if (argc != 3)
  {
    cli_shell_printf(shell, "Invalid cmd\r\n");
    return -1;
  }
  int output = 0;
  sscanf(argv[1], "%d", &output);
  if (output < 0 && output >= BSP_OUTPUT_NUM)
  {
    cli_shell_printf(shell, "Output %d not support\r\n", output);
    return -1;
  }

  if (strcmp(argv[2], "on") == 0)
    bsp_output_on(output);
  else if (strcmp(argv[2], "off") == 0)
    bsp_output_off(output);
  else if (strcmp(argv[2], "toggle") == 0)
    bsp_output_toggle(output);
  return 0;
}
static int cli_get_input(ShellContext_t *shell,int argc,char *argv[]){
  if(argc != 2){
    cli_shell_printf(shell,"Invalid cmd\r\n");
    return -1;
  }
  int input = 0;
  sscanf(argv[1],"%d",&input);
  if(input < 0 && input >= BSP_INPUT_NUM){
    cli_shell_printf(shell,"Input %d not support\r\n",input);
    return -1;
  }
  cli_shell_printf(shell,"%d",bsp_get_input(input));
  return 0;
}
#endif
#if LORA_ENABLE
static void lora_connect_cb(LoRaMesh_t *lora, uint8_t isSuccess, void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == LORA_RES_SUCCESS)
  {
    log_info(TAG, "Connect to Lora module success");
    // cli_shell_printf(shell, "Connect to Lora module success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Lora module\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == LORA_RES_FAIL)
  {
    log_error(TAG, "Connect to Lora module fail");
    // cli_shell_printf(shell, "Connect to Lora module fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Lora module\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Connect to Lora module timeout");
    // cli_shell_printf(shell, "Connect to Lora module timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Lora module\",\"result\" : \"timeout\"}\r\n");
  }
}

static void lora_read_cb(LoRaMesh_t *lora, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == LORA_RES_SUCCESS)
  {
    LoraParameter_t *param = &lora->param;
    log_info(TAG, "Read Lora module param success");
    log_info(TAG, "PAN ID : 0x%04X", param->PAN_ID);
    log_info(TAG, "Channel : %d", param->Channel);
    log_info(TAG, "User Address : 0x%04X", param->userAddress);
    log_info(TAG, "Point Type : %d", param->pointType);
    log_info(TAG, "Transfer Model : %d", param->transferModel);
    log_info(TAG, "Uart Baudrate : %d", param->uartBraudRate);
    log_info(TAG, "Uart Data bits : %d", param->uartDataBits);
    log_info(TAG, "Uart Stop bits : %d", param->uartStopBits);
    log_info(TAG, "Uart Parity : %d", param->uartParity);
    log_info(TAG, "Antenna Select : %d", param->antennaSelect);
    log_info(TAG, "Is Security : %d", param->isSecurity);
    log_info(TAG, "Security Code : 0x%02X 0x%02X 0x%02X 0x%02X", param->securityCode[0], param->securityCode[1], param->securityCode[2], param->securityCode[3]);
    log_info(TAG, "Short Address : 0x%04X", param->shortAddress);
    log_info(TAG, "Mac Address : 0x%02X%02X%02X%02X%02X%02X%02X%02X", param->macAddress[0], param->macAddress[1], param->macAddress[2], param->macAddress[3], param->macAddress[4], param->macAddress[5], param->macAddress[6], param->macAddress[7]);
    // cli_shell_printf(shell, "Read Lora module param success\r\n");
    // cli_shell_printf(shell, "PAN ID : 0x%04X\r\n", param->PAN_ID);
    // cli_shell_printf(shell, "Channel : %d\r\n", param->Channel);
    // cli_shell_printf(shell, "User Address : 0x%04X\r\n", param->userAddress);
    // cli_shell_printf(shell, "Point Type : %d\r\n", param->pointType);
    // cli_shell_printf(shell, "Transfer Model : %d\r\n", param->transferModel);
    // cli_shell_printf(shell, "Uart Baudrate : %d\r\n", param->uartBraudRate);
    // cli_shell_printf(shell, "Uart Data bits : %d\r\n", param->uartDataBits);
    // cli_shell_printf(shell, "Uart Stop bits : %d\r\n", param->uartStopBits);
    // cli_shell_printf(shell, "Uart Parity : %d\r\n", param->uartParity);
    // cli_shell_printf(shell, "Antenna Select : %d\r\n", param->antennaSelect);
    // cli_shell_printf(shell, "Is Security : %d\r\n", param->isSecurity);
    // cli_shell_printf(shell, "Security : 0x%02X 0x%02X 0x%02X 0x%02X\r\n", param->securityCode[0], param->securityCode[1], param->securityCode[2], param->securityCode[3]);
    // cli_shell_printf(shell, "Short Address : 0x%04X\r\n", param->shortAddress);
    // cli_shell_printf(shell, "Mac Address : 0x%02X%02X%02X%02X%02X%02X%02X%02X\r\n", param->macAddress[0], param->macAddress[1], param->macAddress[2], param->macAddress[3], param->macAddress[4], param->macAddress[5], param->macAddress[6], param->macAddress[7]);
    char buff[512];
    sprintf(buff, "{\"cmd\" : \"Read Lora module param\",\"result\" : \"success\",\"PAN_ID\" : \"0x%04X\",\"Channel\" : %d,\"userAddress\" : \"0x%04X\",\"pointType\" : %d,\"transferModel\" : %d,\"uartBraudRate\" : %d,\"uartDataBits\" : %d,\"uartStopBits\" : %d,\"uartParity\" : %d,\"antennaSelect\" : %d,\"isSecurity\" : %d,\"securityCode\" : \"0x%02X%02X%02X%02X\",\"shortAddress\" : \"0x%04X\",\"macAddress\" : \"0x%02X%02X%02X%02X%02X%02X%02X%02X\"}\r\n",
            param->PAN_ID, param->Channel, param->userAddress, param->pointType, param->transferModel, param->uartBraudRate, param->uartDataBits, param->uartStopBits, param->uartParity, param->antennaSelect, param->isSecurity,
            param->securityCode[0], param->securityCode[1], param->securityCode[2], param->securityCode[3],
            param->shortAddress,
            param->macAddress[0], param->macAddress[1], param->macAddress[2], param->macAddress[3], param->macAddress[4], param->macAddress[5], param->macAddress[6], param->macAddress[7]);
    cli_shell_put_line(shell, buff);
  }
  else if(isSuccess == LORA_RES_FAIL)
  {
    log_error(TAG, "Read Lora module param fail");
    // cli_shell_printf(shell, "Read Lora module param fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Read Lora module param\",\"result\" : \"fail\"}\r\n");
  }
  else{
    log_error(TAG, "Read Lora module param timeout");
    // cli_shell_printf(shell, "Read Lora module param timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Read Lora module param\",\"result\" : \"timeout\"}\r\n");
  }
}
static void lora_reset_cb(LoRaMesh_t *lora, uint8_t isSuccess,void *arg)
{
  if (isSuccess == LORA_RES_SUCCESS)
  {
    log_info(TAG, "Reset Lora module success");
    // cli_shell_printf((ShellContext_t *)arg, "Reset Lora module success\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Reset Lora module\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == LORA_RES_FAIL)
  {
    log_error(TAG, "Reset Lora module fail");
    // cli_shell_printf((ShellContext_t *)arg, "Reset Lora module fail\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Reset Lora module\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Reset Lora module timeout");
    // cli_shell_printf((ShellContext_t *)arg, "Reset Lora module timeout\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Reset Lora module\",\"result\" : \"timeout\"}\r\n");
  }
}
static void lora_lowpower_cb(LoRaMesh_t *lora, uint8_t isSuccess,void *arg)
{
  if (isSuccess == LORA_RES_SUCCESS)
  {
    log_info(TAG, "Enter low power mode success");
    // cli_shell_printf((ShellContext_t *)arg, "Enter low power mode success\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Enter low power mode\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == LORA_RES_FAIL)
  {
    log_error(TAG, "Enter low power mode fail");
    // cli_shell_printf((ShellContext_t *)arg, "Enter low power mode fail\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Enter low power mode\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Enter low power mode timeout");
    // cli_shell_printf((ShellContext_t *)arg, "Enter low power mode timeout\r\n");
    cli_shell_put_line((ShellContext_t *)arg, "{\"cmd\" : \"Enter low power mode\",\"result\" : \"timeout\"}\r\n");
  }
}
static void lora_write_cb(LoRaMesh_t *lora, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == LORA_RES_SUCCESS)
  {
    log_info(TAG, "Write param to Lora module success");
    // cli_shell_printf(shell, "Write param to Lora module success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write param to Lora module\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == LORA_RES_FAIL)
  {
    log_error(TAG, "Write param to Lora module fail");
    // cli_shell_printf(shell, "Write param to Lora module fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write param to Lora module\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Write param to Lora module timeout");
    // cli_shell_printf(shell, "Write param to Lora module timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write param to Lora module\",\"result\" : \"timeout\"}\r\n");
  }
}
static int cli_lora(ShellContext_t *shell, int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  if (argc < 2 && argc % 2 == 0)
  {
    cli_shell_help_handler(shell, 0, NULL);
    return -1;
  }

  if (strcmp(argv[1], "-c") == 0)
  {
    lora_connect(&lora, lora_connect_cb,shell);
  }
  else if (strcmp(argv[1], "-s") == 0)
  {
      uint8_t data[128] = {0};
      int data_len = 0;
      for(int i = 2 ; i < argc; i+=2)
      {
          if(strcmp(argv[i], "-data") == 0)
          {
              for(int j = i+1; j < argc; j++)
              {
                  int byte = 0;
                  sscanf(argv[j], "0x%02X", &byte);
                  data[data_len++] = byte & 0xFF;
              }
          }
      }
      if(data_len > 0)
      {
          lora.driver->write(data, data_len);
          log_info(TAG, "Send data to Lora module success");
      }
      else
      {
          log_error(TAG, "No data to send");
          return -1;
      }
  }
  else if (strcmp(argv[1], "-r") == 0)
  {
    lora_read_module(&lora, lora_read_cb,shell);
  }
  else if (strcmp(argv[1], "-rs") == 0)
  {
    lora_reset_module(&lora, lora_reset_cb,shell);
  }
  else if (strcmp(argv[1], "-lp") == 0)
  {
    lora_enter_lowpower(&lora, lora_lowpower_cb,shell);
  }
  else if (strcmp(argv[1], "-w") == 0)
  {
      for(int i = 2 ; i < argc; i+=2)
      {
          if(strcmp(argv[i], "-p") == 0)
          {
              int panid = 0;
              sscanf(argv[i+1], "0x%04X", &panid);
              lora.param.PAN_ID = panid;
          }
          else if(strcmp(argv[i], "-c") == 0)
          {
              int channel = 0;
              sscanf(argv[i+1], "%d", &channel);
              if(channel < 0 || channel > 15) channel = 0;
              lora.param.Channel = channel;
          }
          else if(strcmp(argv[i], "-add") == 0)
          {
              int address = 0;
              sscanf(argv[i+1], "0x%04X", &address);
              lora.param.userAddress = address;
          }
          else if(strcmp(argv[i], "-t") == 0)
          {
              int type = 0;
              sscanf(argv[i+1], "%d", &type);
              lora.param.pointType = type;
          }
          else if(strcmp(argv[i], "-tm") == 0)
          {
              int model = 0;
              sscanf(argv[i+1], "%d", &model);
              lora.param.transferModel = model;
          }
          else if(strcmp(argv[i], "-b") == 0)
          {
              unsigned int baudrate = 9600;
              sscanf(argv[i+1], "%u", &baudrate);
              lora.param.uartBraudRate = baudrate;
          }
          else if(strcmp(argv[i], "-d") == 0)
          {
              int databits = 8;
              sscanf(argv[i+1], "%d", &databits);
              lora.param.uartDataBits = databits;
          }
          else if(strcmp(argv[i], "-s") == 0)
          {
              int stopbits = 1;
              sscanf(argv[i+1], "%d", &stopbits);
              if(stopbits != 1 && stopbits != 2) stopbits = 1;
              lora.param.uartStopBits = stopbits;
          }
          else if(strcmp(argv[i], "-p") == 0)
          {
              if(strcmp(argv[i+1], "none") == 0)
                  lora.param.uartParity = 1;
              else if(strcmp(argv[i+1], "odd") == 0)
                  lora.param.uartParity = 3;
              else if(strcmp(argv[i+1], "even") == 0)
                  lora.param.uartParity = 2;
          }
          else if(strcmp(argv[i], "-at") == 0)
          {
              int antenna = 0;
              sscanf(argv[i+1], "%d", &antenna);
              lora.param.antennaSelect = antenna;
          }
          else if(strcmp(argv[i], "-s") == 0)
          {
              int security = 0;
              sscanf(argv[i+1], "%d", &security);
              lora.param.isSecurity = security;
          }
          else if(strcmp(argv[i], "-sc") == 0)
          {
              int scode[4] = {0};
              sscanf(argv[i+1], "0x%02X", &scode[0]);
              sscanf(argv[i+2], "0x%02X", &scode[1]);
              sscanf(argv[i+3], "0x%02X", &scode[2]);
              sscanf(argv[i+4], "0x%02X", &scode[3]);
              memcpy(lora.param.securityCode, scode, 4);
              i+=3;
          }
        }
      lora_write_module(&lora, lora_write_cb,shell);
  }
  else
  {
    cli_shell_help_handler(shell, 0, NULL);
    return -1;
  }
  return 0;
}
#endif

#if ZIGBEE_ENABLE

static void zigbee_connect_cb(ZigbeeMesh_t *zigbee, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == ZIGBEE_RES_SUCCESS)
  {
    log_info(TAG, "Connect to Zigbee module success");
    // cli_shell_printf(shell, "Connect to Zigbee module success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Zigbee module\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == ZIGBEE_RES_FAIL)
  {
    log_error(TAG, "Connect to Zigbee module fail");
    // cli_shell_printf(shell, "Connect to Zigbee module fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Zigbee module\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Connect to Zigbee module timeout");
    // cli_shell_printf(shell, "Connect to Zigbee module timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Connect to Zigbee module\",\"result\" : \"timeout\"}\r\n");
  }
}

static void zigbee_read_cb(ZigbeeMesh_t *zigbee,  uint8_t isSuccess, void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == ZIGBEE_RES_SUCCESS)
  {
    ZigbeeParameter_t *param = &zigbee->param;
    log_info(TAG, "Read Zigbee module param success");
    log_info(TAG, "PAN ID : 0x%04X", param->PAN_ID);
    log_info(TAG, "Channel : %d", param->Channel);
    log_info(TAG, "User Address : 0x%04X", param->userAddress);
    log_info(TAG, "Point Type : %d", param->pointType);
    log_info(TAG, "Transfer Model : %d", param->transferModel);
    log_info(TAG, "Uart Baudrate : %d", param->uartBraudRate);
    log_info(TAG, "Uart Data bits : %d", param->uartDataBits);
    log_info(TAG, "Uart Stop bits : %d", param->uartStopBits);
    log_info(TAG, "Uart Parity : %d", param->uartParity);
    log_info(TAG, "Antenna Select : %d", param->antennaSelect);
    log_info(TAG, "Is Security : %d", param->isSecurity);
    log_info(TAG, "Security Code : 0x%02X 0x%02X 0x%02X 0x%02X", param->securityCode[0], param->securityCode[1], param->securityCode[2], param->securityCode[3]);
    log_info(TAG, "Short Address : 0x%04X", param->shortAddress);
    log_info(TAG, "Mac Address : 0x%02X%02X%02X%02X%02X%02X%02X%02X", param->macAddress[0], param->macAddress[1], param->macAddress[2], param->macAddress[3], param->macAddress[4], param->macAddress[5], param->macAddress[6], param->macAddress[7]);
    char buffer[512];
    sprintf(buffer,"{\"cmd\" : \"Read zigbee parameter\",\"result\" : \"success\", \"PAN_ID\" : \"0x%04X\", \"Channel\" : \"%d\", \"User Address\" : \"0x%04X\", \"Point Type\" : \"%d\", \"Transfer Model\" : \"%d\", \"Uart Baudrate\" : \"%d\", \"Uart Data bits\" : \"%d\", \"Uart Stop bits\" : \"%d\", \"Uart Parity\" : \"%d\", \"Antenna Select\" : \"%d\", \"Is Security\" : \"%d\", \"Security Code\" : \"0x%02X%02X%02X%02X\", \"Short Address\" : \"0x%04X\", \"Mac Address\" : \"0x%02X%02X%02X%02X%02X%02X%02X%02X\"}\r\n",
            param->PAN_ID,
            param->Channel,
            param->userAddress,
            param->pointType,
            param->transferModel,
            param->uartBraudRate,
            param->uartDataBits,
            param->uartStopBits,
            param->uartParity,
            param->antennaSelect,
            param->isSecurity,
            param->securityCode[0], param->securityCode[1], param->securityCode[2], param->securityCode[3],
            param->shortAddress,
            param->macAddress[0], param->macAddress[1], param->macAddress[2], param->macAddress[3], param->macAddress[4], param->macAddress[5], param->macAddress[6], param->macAddress[7]);
    cli_shell_put_line(shell, buffer);
  }
  else if(isSuccess == ZIGBEE_RES_FAIL)
  {
    log_error(TAG, "Read Zigbee module param fail");
    // cli_shell_printf(shell, "Read Zigbee module param fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Read zigbee parameter\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Read Zigbee module param timeout");
    // cli_shell_printf(shell, "Read Zigbee module param timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Read zigbee parameter\",\"result\" : \"timeout\"}\r\n");
  }
}

void zigbee_reset_cb(ZigbeeMesh_t *zigbee, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == ZIGBEE_RES_SUCCESS)
  {
    log_info(TAG, "Reset Zigbee module success");
    // cli_shell_printf(shell, "Reset Zigbee module success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Reset zigbee module\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == ZIGBEE_RES_FAIL)
  {
    log_error(TAG, "Reset Zigbee module fail");
    // cli_shell_printf(shell, "Reset Zigbee module fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Reset zigbee module\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Reset Zigbee module timeout");
    // cli_shell_printf(shell, "Reset Zigbee module timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Reset zigbee module\",\"result\" : \"timeout\"}\r\n");
  }
}

void zigbee_lowpower_cb(ZigbeeMesh_t *zigbee, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == ZIGBEE_RES_SUCCESS)
  {
    log_info(TAG, "Enter low power mode success");
    // cli_shell_printf(shell, "Enter low power mode success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Enter zigbee low power mode\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == ZIGBEE_RES_FAIL)
  {
    log_error(TAG, "Enter low power mode fail");
    // cli_shell_printf(shell, "Enter low power mode fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Enter zigbee low power mode\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Enter low power mode timeout");
    // cli_shell_printf(shell, "Enter low power mode timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Enter zigbee low power mode\",\"result\" : \"timeout\"}\r\n");
  }
}
void zigbee_write_cb(ZigbeeMesh_t *zigbee, uint8_t isSuccess,void *arg)
{
  ShellContext_t *shell = (ShellContext_t *)arg;
  if (isSuccess == ZIGBEE_RES_SUCCESS)
  {
    log_info(TAG, "Write param to Zigbee module success");
    // cli_shell_printf(shell, "Write param to Zigbee module success\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write zigbee parameter\",\"result\" : \"success\"}\r\n");
  }
  else if (isSuccess == ZIGBEE_RES_FAIL)
  {
    log_error(TAG, "Write param to Zigbee module fail");
    // cli_shell_printf(shell, "Write param to Zigbee module fail\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write zigbee parameter\",\"result\" : \"fail\"}\r\n");
  }
  else
  {
    log_error(TAG, "Write param to Zigbee module timeout");
    // cli_shell_printf(shell, "Write param to Zigbee module timeout\r\n");
    cli_shell_put_line(shell, "{\"cmd\" : \"Write zigbee parameter\",\"result\" : \"timeout\"}\r\n");
  }
}

static int cli_zigbee(ShellContext_t *shell, int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  if( argc < 2 || argc % 2 != 0)
  {
    cli_shell_help_handler(shell, 0, NULL);
    return -1;
  }
  if(strcmp(argv[1],"-c") == 0)
  {
    zigbee_connect(&zigbee, zigbee_connect_cb,shell);
  }
  else if(strcmp(argv[1],"-s") == 0)
  {
      uint8_t data[128] = {0};
      int data_len = 0;
      for(int i = 2 ; i < argc; i+=2)
      {
          if(strcmp(argv[i], "-d") == 0)
          {
              for(int j = i+1; j < argc; j++)
              {
                  int byte = 0;
                  sscanf(argv[j], "0x%02X", &byte);
                  data[data_len++] = byte & 0xFF;
              }
          }
      }
      if(data_len > 0)
      {
          zigbee.driver->write(data, data_len);
          log_info(TAG, "Send data to Zigbee module success");
      }
      else
      {
          log_error(TAG, "No data to send");
          return -1;
      }
  }
  else if(strcmp(argv[1],"-r") == 0)
  {
    zigbee_read_module(&zigbee, zigbee_read_cb,shell);
  }
  else if(strcmp(argv[1],"-rs") == 0)
  {
    zigbee_reset_module(&zigbee, zigbee_reset_cb,shell);
  }
  else if(strcmp(argv[1],"-w") == 0)
  {
      for(int i = 2 ; i < argc; i+=2)
      {
          if(strcmp(argv[i], "-p") == 0)
          {
              int panid = 0;
              sscanf(argv[i+1], "0x%04X", &panid);
              if(panid < 0x0001 || panid > 0xFF00) panid = 0x1234;
              zigbee.param.PAN_ID = panid;
          }
          else if(strcmp(argv[i], "-c") == 0)
          {
              int channel = 0;
              sscanf(argv[i+1], "%d", &channel);
              if(channel < 0x0B || channel > 0x1A) channel = 0x0B;
              zigbee.param.Channel = channel;
          }
          else if(strcmp(argv[i], "-add") == 0)
          {
              int address = 0;
              sscanf(argv[i+1], "0x%04X", &address);
              zigbee.param.userAddress = address;
          }
          else if(strcmp(argv[i], "-t") == 0)
          {
              int type = 0;
              sscanf(argv[i+1], "%d", &type);
              if(type < 1 || type > 3) type = 2;
              zigbee.param.pointType = type;
          }
          else if(strcmp(argv[i], "-tm") == 0)
          {
              int model = 0;
              sscanf(argv[i+1], "%d", &model);
              zigbee.param.transferModel = model;
          }
          else if(strcmp(argv[i], "-b") == 0)
          {
              unsigned int baudrate = 0;
              sscanf(argv[i+1], "%u", &baudrate);
              if(baudrate < 1 || baudrate > 8) baudrate = 8;
              zigbee.param.uartBraudRate = baudrate;
          }
          else if(strcmp(argv[i], "-d") == 0)
          {
              int databits = 8;
              sscanf(argv[i+1], "%d", &databits);
              if(databits != 1) databits = 1;
              zigbee.param.uartDataBits = databits;
          }
          else if(strcmp(argv[i], "-s") == 0)
          {
              int stopbits = 1;
              // sscanf(argv[i+1], "%d", &stopbits);
              stopbits = 1;
              zigbee.param.uartStopBits = stopbits;
          }
          else if(strcmp(argv[i], "-p") == 0)
          {
              if(strcmp(argv[i+1], "1") == 0)
                  zigbee.param.uartParity = 1;
              else if(strcmp(argv[i+1], "2") == 0)
                  zigbee.param.uartParity = 3;
              else if(strcmp(argv[i+1], "3") == 0)
                  zigbee.param.uartParity = 2;
              else zigbee.param.uartParity = 1;
          }
          else if(strcmp(argv[i], "-at") == 0)
          {
              int antenna = 0;
              sscanf(argv[i+1], "%d", &antenna);
              if(antenna == 0) antenna = 0;
              else antenna = 1;
              zigbee.param.antennaSelect = antenna;
          }
          else if(strcmp(argv[i], "-s") == 0)
          {
              int security = 0;
              sscanf(argv[i+1], "%d", &security);
              zigbee.param.isSecurity = security;
          }
          else if(strcmp(argv[i], "-sc") == 0)
          {
              int scode[4] = {0};
              sscanf(argv[i+1], "0x%02X", &scode[0]);
              sscanf(argv[i+2], "0x%02X", &scode[1]);
              sscanf(argv[i+3], "0x%02X", &scode[2]);
              sscanf(argv[i+4], "0x%02X", &scode[3]);
              memcpy(zigbee.param.securityCode, scode, 4);
              i+=3;
          }
        }
      zigbee_write_module(&zigbee, zigbee_write_cb,shell);
  }
  else
  {
    cli_shell_help_handler(shell, 0, NULL);
    return -1;
  }
  return 0;
}
#endif

#if TRANSFER_MODE_EN
static int cli_mode(ShellContext_t *shell,int argc,char *argv[]){
  if(argc != 2){
    cli_shell_printf(shell,"Invalid\r\n");
  }
  if(strcmp(argv[1],"1") == 0){
    app_setting.app_mode = Parency_Transfer;
  }
  else if(strcmp(argv[1],"2") == 0){
    app_setting.app_mode = Parency_Modbus;
  }
  app_setting_save(&app_setting);
  cli_shell_printf(shell,"Switch mode %s\r\n",app_setting.app_mode == Parency_Modbus ? "Parency Modbus" : "Parency Transfer");
  return 0;
}
#endif
const Cli_Shell_Cmd *const g_shell_commands = s_shell_commands;
const size_t g_num_shell_commands = ARRAY_SIZE(s_shell_commands);