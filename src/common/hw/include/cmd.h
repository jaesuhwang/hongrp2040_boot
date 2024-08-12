// #ifndef SRC_COMMON_HW_INCLUDE_CMD_H_
// #define SRC_COMMON_HW_INCLUDE_CMD_H_


// #ifdef __cplusplus
//  extern "C" {
// #endif

// #include "hw_def.h"

// #ifdef _USE_HW_CMD



// #define CMD_MAX_DATA_LENGTH       HW_CMD_MAX_DATA_LENGTH


// #define CMD_OK                0

// #define CMD_DIR_M_TO_S        0
// #define CMD_DIR_S_TO_M        1



// typedef struct
// {
//   uint8_t   cmd;
//   uint8_t   dir;
//   uint8_t   error;
//   uint16_t  length;
//   uint8_t   check_sum;
//   uint8_t   check_sum_recv;
//   uint8_t   buffer[CMD_MAX_DATA_LENGTH+8];
//   uint8_t   *data;
// } cmd_packet_t;


// typedef struct
// {
//   uint8_t   ch;
//   bool      is_init;
//   uint32_t  baud;
//   uint8_t   state;
//   uint32_t  pre_time;
//   uint32_t  index;
//   uint8_t   error;

//   cmd_packet_t  rx_packet;
//   cmd_packet_t  tx_packet;
// } cmd_t;


// void cmdInit(cmd_t *p_cmd);
// bool cmdOpen(cmd_t *p_cmd, uint8_t ch, uint32_t baud);
// bool cmdClose(cmd_t *p_cmd);
// bool cmdReceivePacket(cmd_t *p_cmd);
// void cmdSendCmd(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length);
// void cmdSendResp(cmd_t *p_cmd, uint8_t cmd, uint8_t err_code, uint8_t *p_data, uint32_t length);
// bool cmdSendCmdRxResp(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length, uint32_t timeout);

// #endif



// #ifdef __cplusplus
//  }
// #endif

// #endif /* SRC_COMMON_HW_INCLUDE_CMD_H_ */


#ifndef CMD_H_
#define CMD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_CMD


#define CMD_MAX_DATA_LENGTH   HW_CMD_MAX_DATA_LENGTH


#define CMD_OK      0x0000



typedef enum CmdType
{
  PKT_TYPE_CMD        = 0x00,
  PKT_TYPE_RESP       = 0x01,
  PKT_TYPE_EVENT      = 0x02,
  PKT_TYPE_LOG        = 0x03,
  PKT_TYPE_PING       = 0x04,
  PKT_TYPE_CAN        = 0x05,
  PKT_TYPE_UART       = 0x06,
} CmdType_t;


typedef struct
{
  CmdType_t type;
  uint16_t  cmd;
  uint16_t  err_code;
  uint16_t  length;
  uint8_t   check_sum;
  uint8_t   check_sum_recv;
  uint8_t   buffer[CMD_MAX_DATA_LENGTH + 10];
  uint8_t  *data;
} cmd_packet_t;


typedef struct cmd_driver_t_ cmd_driver_t;

typedef struct cmd_driver_t_
{
  uint32_t  args[32];

  bool     (*open)(void *args);
  bool     (*close)(void *args);  
  uint32_t (*available)(void *args);
  bool     (*flush)(void *args);
  uint8_t  (*read)(void *args);
  uint32_t (*write)(void *args, uint8_t *p_data, uint32_t length);  
} cmd_driver_t;


typedef struct
{
  bool      is_init;
  bool      is_open;
  
  uint8_t   state;
  uint32_t  pre_time;
  uint32_t  index;

  cmd_driver_t *p_driver;

  cmd_packet_t  packet;
} cmd_t;


void cmdInit(cmd_t *p_cmd, cmd_driver_t *p_driver);
bool cmdOpen(cmd_t *p_cmd);
bool cmdClose(cmd_t *p_cmd);
bool cmdIsBusy(cmd_t *p_cmd);
bool cmdReceivePacket(cmd_t *p_cmd);

bool cmdSend(cmd_t *p_cmd, CmdType_t type, uint16_t cmd, uint16_t err_code, uint8_t *p_data, uint32_t length);
bool cmdSendType(cmd_t *p_cmd, CmdType_t type, uint8_t *p_data, uint32_t length);
bool cmdSendCmd(cmd_t *p_cmd, uint16_t cmd, uint8_t *p_data, uint32_t length);
bool cmdSendResp(cmd_t *p_cmd, uint16_t cmd, uint16_t err_code, uint8_t *p_data, uint32_t length);
bool cmdSendCmdRxResp(cmd_t *p_cmd, uint16_t cmd, uint8_t *p_data, uint32_t length, uint32_t timeout);


#endif

#ifdef __cplusplus
 }
#endif


#endif 




















