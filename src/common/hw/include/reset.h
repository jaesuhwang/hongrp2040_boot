#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RESET

// #define RESET_MODE_FW     0
// #define RESET_MODE_BOOT   1

#define MODE_BIT_BOOT     0
#define MODE_BIT_UPDATE   1

// #define MODE_BIT_BOOT     1
// #define MODE_BIT_UPDATE   0

void resetInit(void);

void resetSetBootMode(uint32_t mode);
uint32_t resetGetBootMode(void);
const char *resetGetBootModeMsg(void);
uint32_t resetGetCount(void);

void resetToBoot(uint32_t timeout);
void resetToRunBoot(void);
void resetToRunFw(void);

#endif

#ifdef __cplusplus
 }
#endif

#endif /* SRC_COMMON_HW_INCLUDE_RESET_H_ */