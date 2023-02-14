#include <cstdio>
#include "common_inc.h"
#include "configurations.h"
#include "HelloWord/RGBLight.h"

extern HWKeyboard_RGBLED RGBLED;

void OnUartCmd(uint8_t* _data, uint16_t _len)
{
    float cur, pos, vel, time;
    int ret = 0;
    switch (_data[0])
    {
        case 'p':
            break;
    }

}

