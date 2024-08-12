#ifndef SRC_AP_BOOT_BOOT_H_
#define SRC_AP_BOOT_BOOT_H_


#include "ap_def.h"



// void bootInit(void);
// void bootProcessCmd(cmd_t *p_cmd);
// bool bootJumpToFw(void);
// bool bootVerifyFw(void);
// uint8_t bootUpdateFw(void);
// uint8_t bootUpdateTagVerify(void);


uint16_t bootVerifyUpdate(void);
uint16_t bootVerifyFirm(void);
uint16_t bootUpdateFirm(void);
uint16_t bootUpdateFirmFromFile(const char *file_name);
uint16_t bootJumpFirm(void);


#endif