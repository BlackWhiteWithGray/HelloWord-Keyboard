#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------- C Scope ---------------------------*/
#include <stdbool.h>
#include "stdint-gcc.h"


typedef enum configStatus_t
{
    CONFIG_RESTORE = 0,
    CONFIG_OK,
    CONFIG_COMMIT
} configStatus_t;


typedef struct KeyboardConfig_t
{
    configStatus_t configStatus;
    uint64_t serialNum;
    float KeyboardBrightness;
    uint8_t LightON;
    uint16_t   FilterTime;
    uint8_t  TouchEndable;
    uint8_t  RGBLEDMode;
    int8_t keyMap[128];
} KeyboardConfig_t;

extern KeyboardConfig_t config;


#ifdef __cplusplus
}
/*---------------------------- C++ Scope ---------------------------*/



#endif
#endif
