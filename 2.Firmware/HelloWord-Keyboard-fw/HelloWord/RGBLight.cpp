//aaa
// Created by Administrator on 2022/11/4.
//
#include "spi.h"
#include "RGBLight.h"
#include "HelloWord/hw_keyboard.h"
#include "configurations.h"

extern HWKeyboard keyboard;
extern KeyboardConfig_t config;
float MonochromeH=0;

void HWKeyboard_RGBLED::RGBLED_Init(void)
{
    // Init RGB buffer
    for (uint8_t i = 0;i<LED_NUMBER;i++)
       SetRgbBufferByID(i, Color_r{0, 0, 0}, KeyboardBrightness);

    CapsColorSet(Color_r{255, 100, 255});
    NumColorSet(Color_r{0, 200, 0});
    MonochromeColorSet(Color_r{255, 0, 0});
    if(config.FilterTime==100){
        FNColorSet(Color_r{(uint8_t) 187, 167, 16});
    }else if(config.FilterTime==200){
        FNColorSet(Color_r{(uint8_t) 72, 200, 224});
    }else if(config.FilterTime==300){
        FNColorSet(Color_r{(uint8_t) 45, 23, 222});
    }else if(config.FilterTime==400){
        FNColorSet(Color_r{(uint8_t) 213, 18, 232});
    }
}


void HWKeyboard_RGBLED::SetRgbBufferByID(uint8_t _keyId, Color_r _color, float _brightness)
{
    // To ensure there's no sequence zero bits, otherwise will case ws2812b protocol error.
    if (_color.b < 1)_color.b = 1;
    for (int i = 0; i < 8; i++)
    {
        rgbBuffer[_keyId][0][i] =
                ((uint8_t) ((float) _color.g * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
        rgbBuffer[_keyId][1][i] =
                ((uint8_t) ((float) _color.r * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
        rgbBuffer[_keyId][2][i] =
                ((uint8_t) ((float) _color.b * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
    }
}


void HWKeyboard_RGBLED::SyncLights()
{
    while (isRgbTxBusy);
    isRgbTxBusy = true;
    HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) rgbBuffer, LED_NUMBER * 3 * 8);
    while (isRgbTxBusy);
    isRgbTxBusy = true;
    HAL_SPI_Transmit_DMA(&hspi2, wsCommit, 64);
}

void HWKeyboard_RGBLED::RGBLED_Handle()
{
    /*---- This is a demo RGB effect ----*/
    RGBLED_ModeHandle(RGBMode);
    //右下角三个灯
    if (isNumLocked) {
        //   RGBLED.SetRgbBufferByID(0, Color_r{(uint8_t)  0, 180, 0}, 1);
        SetRgbBufferByID(82, NumColor, 1);
        SetRgbBufferByID(83, NumColor, 1);
        SetRgbBufferByID(84, NumColor, 1);
    }
    if (isCapsLocked) {
        //  RGBLED.SetRgbBufferByID(0, Color_r{(uint8_t) 180, 0, 0}, 1);
        SetRgbBufferByID(82, CapsColor, 1);
        SetRgbBufferByID(83, CapsColor, 1);
        SetRgbBufferByID(84, CapsColor, 1);
    }

    if (keyboard.isFNPress) {
        //   RGBLED.SetRgbBufferByID(0, Color_r{(uint8_t)  0, 0, 180}, 1);
        SetRgbBufferByID(82, FNColor, 1);
        SetRgbBufferByID(83, FNColor, 1);
        SetRgbBufferByID(84, FNColor, 1);
    }

    if (keyboard.TouchTestLED) {
        if (keyboard.GetTouchBarState(1))
            SetRgbBufferByID(12, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(12, Color_r{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(2))
            SetRgbBufferByID(11, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(11, Color_r{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(3))
            SetRgbBufferByID(10, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(10, Color_r{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(4))
            SetRgbBufferByID(9, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(9, Color_r{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(5))
            SetRgbBufferByID(8, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(8, Color_r{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(6))
            SetRgbBufferByID(7, Color_r{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(7, Color_r{(uint8_t) 255, 180, 0}, 0);
    }

    /*-----------------------------------*/
    // Send RGB buffers to LEDs ++
    SyncLights();
}

void HWKeyboard_RGBLED::FNColorSet(HWKeyboard_RGBLED::Color_r Color)
{
    FNColor   = Color;
}

void HWKeyboard_RGBLED::CapsColorSet(HWKeyboard_RGBLED::Color_r Color)
{
    CapsColor   = Color;
}

void HWKeyboard_RGBLED::NumColorSet(HWKeyboard_RGBLED::Color_r Color)
{
    NumColor   = Color;
}
void HWKeyboard_RGBLED::MonochromeColorSet(HWKeyboard_RGBLED::Color_r Color)
{
    MonochromeColor   = Color;
}

void HWKeyboard_RGBLED::RGBLED_ModeHandle(uint8_t mode)
{
    if(LightON) {
        switch (mode) {
            case monochrome: {
                MonochromeH+=0.4;
                if(MonochromeH>255)MonochromeH=0;
                for (uint8_t i = 0; i < 85; i++) {
                    SetRgbBufferByID(i, HSVToRGB(Color_h{(uint8_t)MonochromeH,245,255}), KeyboardBrightness);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, HSVToRGB(Color_h{(uint8_t)MonochromeH,245,255}), KeyboardBrightness);
                }
            }
                break;
            case breathing: {
                static float LEDBreadBrightness = 0.09;
                static bool LEDBreadDir = true;
                static float W = 0;
                if (LEDBreadBrightness >= 1.1)LEDBreadDir = false;
                else if (LEDBreadBrightness <= 0.06)LEDBreadDir = true;
                LEDBreadDir ? LEDBreadBrightness += 0.005 : LEDBreadBrightness -= 0.005;
                if (LEDBreadBrightness > 1)
                    W = 1;
                else
                    W = LEDBreadBrightness;
                //正贴灯
                for (uint8_t i = 0; i < 85; i++) {
                    SetRgbBufferByID(i, Color_r{255, 255, 255}, KeyboardBrightness * W);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, Color_r{255, 255, 255}, KeyboardBrightness * W);
                }
            }
                break;
            case Illusorycolor: {
                static uint32_t t = 1;
                static bool fadeDir = true;
                fadeDir ? t++ : t--;

                if (t > 250) fadeDir = false;
                else if (t < 1) fadeDir = true;

                //正贴灯
                for (uint8_t i = 0; i < 85; i++) {
                    SetRgbBufferByID(i, Color_r{(uint8_t) t, 50, 0}, KeyboardBrightness);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, Color_r{(uint8_t) t, 50, 0}, KeyboardBrightness);
                }
            }
                break;
            case Follow:
                for (uint8_t i = 0; i < 82; i++) {
                    if (RGBLEDKeyBuf[i / 8] & (0x80 >> (i % 8))) {
                        SetRgbBufferByID(RGBLEDKeymap[i], Color_r{255, 255, 255}, KeyboardBrightness);
                    } else {
                        SetRgbBufferByID(RGBLEDKeymap[i], Color_r{0, 0, 0}, 0);
                    }
                }
                for (uint8_t i = 82; i < 85; i++) {
                    SetRgbBufferByID(i, Color_r{255, 255, 255}, 0);
                }
                break;
            case Orange:
                if (sw == swing_speed) {
                    LedSwing[15] = LedSwing[0];
                    for (uint8_t i = 0; i < 15; i++) {
                        LedSwing[i] = LedSwing[i + 1];
                    }
                    LedSwing[14] = LedSwing[15];
                    sw = 0;
                }

                sw++;

                for (uint8_t i = 0; i < 15; i++) {
                    LedSwing[15] = LedSwing[i];
                    LedSwing[15].r += float(LedSwing[i + 1].r - LedSwing[i].r) / (float) swing_speed * (float) sw;
                    LedSwing[15].g += float(LedSwing[i + 1].g - LedSwing[i].g) / (float) swing_speed * (float) sw;
                    LedSwing[15].b += float(LedSwing[i + 1].b - LedSwing[i].b) / (float) swing_speed * (float) sw;

                    LedOut[0 + i] = LedSwing[15];
                    LedOut[27 - i] = LedSwing[15];
                    LedOut[28 + i] = LedSwing[15];
                    LedOut[57 - i] = LedSwing[15];
                    LedOut[58 + i] = LedSwing[15];
                    LedOut[85 - i] = LedSwing[15];
                    LedOut[86 + i] = LedSwing[15];
                }

                for (uint8_t i = 0; i < 82; i++) {
                    if (RGBLEDKeyBuf[i / 8] & (0x80 >> (i % 8))) {
                        if (i == 0)//esc
                        {
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress3;
                        } else if (i == 1)//F1
                        {
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress3;
                        } else if (i > 1&&i < 5)//F2 F3 F4
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress3;
                        } else if (i > 4 && i < 9)//F5 F6 F7 F8
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress5;
                        }  else if (i > 8 && i < 11)//F9 F10
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress3;
                        } else if (i == 11)//F11
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress5;
                        } else if (i == 12)//F12
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress5;
                        } else if (i == 13)//PAUSE
                        {
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 14)//`
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress5;
                        } else if (i == 15)//1
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[42] = BrightPress5;
                            LedPress[43] = BrightPress5;
                        } else if (i == 16)//2
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 17)//3
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 18)//4
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 19)//5
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 20)//6
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 21)//7
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 22)//8
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 23)//9
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 24)//0
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 25)//-
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 26)//+
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 27)//BACKSPACE
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress3;
                        } else if (i == 28)//INS
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress3;
                        } else if (i == 29)//TAB
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 16]] = BrightPress3;
                        } else if (i > 29 && i < 42)//QWERTYUIOP[]|
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 43)//del
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                        } else if (i == 44)//CAP
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress3;
                        } else if (i > 44 && i < 57)//ASDFGHJKL;' enter
                        {
                            LedPress[RGBLEDKeymap[i - 16]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 13]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                        } else if (i == 57)//PGUP
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 13]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                        } else if (i == 58)//SHIFTL
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress3;
                        } else if (i == 59)//Z
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 60)//X
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress5;
                        } else if (i == 61)//C
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 15]] = BrightPress1;
                        } else if (i == 62)//V
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 13]] = BrightPress5;
                        } else if (i == 63)//B
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 12]] = BrightPress7;
                        } else if (i == 64)//N
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 11]] = BrightPress5;
                        } else if (i == 65)//M
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress1;
                        } else if (i == 66)//<
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress5;
                        } else if (i == 67)//>
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress5;
                        } else if (i == 68)//?
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress5;
                        } else if (i == 69)//SHIFTR
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress5;
                        } else if (i == 70)//UP
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i + 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i + 11]] = BrightPress5;
                        } else if (i == 71)//PGUN
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 10]] = BrightPress5;
                        } else if (i == 72)//CTRL
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 73)//WIN
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 74)//ALTLL
                        {
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 75)//SPACE
                        {
                            LedPress[RGBLEDKeymap[i - 15]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i - 14]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 13]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 12]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i - 11]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress1;
                        } else if (i == 76)//ALTR
                        {
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress1;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 77)//FN
                        {
                            LedPress[RGBLEDKeymap[i - 11]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 9]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 78)//TRALR
                        {
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 9]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress5;
                        } else if (i == 79)//RIGHT
                        {
                            LedPress[RGBLEDKeymap[i - 11]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress7;
                        } else if (i == 80)//DOWN
                        {
                            LedPress[RGBLEDKeymap[i - 11]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i - 9]] = BrightPress3;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                            LedPress[RGBLEDKeymap[i + 1]] = BrightPress5;
                        } else if (i == 81)//LEFT
                        {
                            LedPress[RGBLEDKeymap[i - 11]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 10]] = BrightPress5;
                            LedPress[RGBLEDKeymap[i - 1]] = BrightPress7;
                            LedPress[RGBLEDKeymap[i]] = BrightPress;
                        }
                    }
                }
                if (isCapsLocked) {
                    LedOut[RGBLEDKeymap[44]] = CapsColor;
                }
                for (uint8_t i = 0; i < 82; i++) {
                    if(LedPress[i].r > LedOut[i].r)LedOut[i].r = LedPress [i].r;
                    if(LedPress[i].g > LedOut[i].g)LedOut[i].g = LedPress [i].g;
                    if(LedPress[i].b > LedOut[i].b)LedOut[i].b = LedPress [i].b;
                    if(LedPress[i].r > 1)LedPress[i].r-=2;
                    else if (LedPress[i].r == 1)LedOut[i].r = 0;
                    if(LedPress[i].g > 1)LedPress[i].g-=2;
                    else if (LedPress[i].g == 1)LedOut[i].g = 0;
                    if(LedPress[i].b > 1)LedPress[i].b-=2;
                    else if (LedPress[i].b == 1)LedOut[i].b = 0;
                }
                for (uint8_t i = 0; i < 104; i++) {
                    SetRgbBufferByID(i, LedOut[i], KeyboardBrightness);
                }
                break;
        }
    }else{
        for (uint8_t i = 0; i < LED_NUMBER; i++) {
                SetRgbBufferByID(i, Color_r{0,0,0}, 0);
        }
    }
}


void HWKeyboard_RGBLED::RGBLED_ctrl(HWKeyboard_RGBLED *rgb,uint8_t trigger,bool  iswriteEep)
{
    switch(trigger)
    {
        case RGBLEDON: {
            rgb->LightON = true;
            if(iswriteEep)config.LightON = 1;
        }
        break;
        case RGBLEDOFF:
        {
            rgb->LightON = false;
            if(iswriteEep) config.LightON = 0;
        }
        break;
        case RGBLEDNegate:
        {
            if (rgb->LightON == true) {
                rgb->LightON = false;
                if(iswriteEep)config.LightON = 0;
            } else {
                rgb->LightON = true;
                if(iswriteEep)config.LightON = 1;
            }
        }
        break;
    }
    if(iswriteEep)keyboard.isEepPush = true;
}

HWKeyboard_RGBLED::Color_r HWKeyboard_RGBLED::HSVToRGB(Color_h HSV)
{
    int i,hi;
    float RGB_min, RGB_max;
    float RGB_Adj;
    float difs;
    Color_r RGB;
    hi = HSV.h*360/255;
    if(hi>359)hi=359;
    RGB_max = HSV.v * 1.0F;
    RGB_min = RGB_max*(255 - HSV.s) / 255.0F;
    i = hi / 60;
    difs = hi % 60;

    /* RGB adjustment amount by hue */
    RGB_Adj = (RGB_max - RGB_min)*difs / 60;

    switch (i) {
        case 0:
            RGB.r = (uint8_t)RGB_max;
            RGB.g = (uint8_t)(RGB_min + RGB_Adj);
            RGB.b = (uint8_t)RGB_min;
            break;
        case 1:
            RGB.r = (uint8_t)(RGB_max - RGB_Adj);
            RGB.g = (uint8_t)RGB_max;
            RGB.b = (uint8_t)RGB_min;
            break;
        case 2:
            RGB.r = (uint8_t)RGB_min;
            RGB.g = (uint8_t)RGB_max;
            RGB.b = (uint8_t)(RGB_min + RGB_Adj);
            break;
        case 3:
            RGB.r = (uint8_t)RGB_min;
            RGB.g = (uint8_t)(RGB_max - RGB_Adj);
            RGB.b = (uint8_t)RGB_max;
            break;
        case 4:
            RGB.r = (uint8_t)(RGB_min + RGB_Adj);
            RGB.g = (uint8_t)RGB_min;
            RGB.b = (uint8_t)RGB_max;
            break;
        default:
            RGB.r = (uint8_t)RGB_max;
            RGB.g = (uint8_t)RGB_min;
            RGB.b = (uint8_t)(RGB_max - RGB_Adj);
            break;
    }
    return RGB;
}