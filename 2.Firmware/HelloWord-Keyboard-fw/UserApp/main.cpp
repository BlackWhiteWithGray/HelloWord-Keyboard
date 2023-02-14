#include "common_inc.h"
#include "configurations.h"
#include "hw_keyboard.h"
#include "time.h"
#include "stm32f1xx_hal_uart.h"


/* Component Definitions -----------------------------------------------------*/
KeyboardConfig_t config;
HWKeyboard keyboard(&hspi1);
HWKeyboard_RGBLED RGBLED;
EEPROM eeprom;
extern short MediaCount;
extern uint8_t Dynamic_BUF[2];
/* Main Entry ----------------------------------------------------------------*/
void Main()
{
    eeprom.Pull(0, config);
    if (config.configStatus != CONFIG_OK)
    {
        // Use default settings
        config = KeyboardConfig_t{
            .configStatus = CONFIG_OK,
            .serialNum=123,
            .KeyboardBrightness = 0.7,
            .LightON = 1,
            .FilterTime = 100,
            .TouchEndable = 1,
            .RGBLEDMode = 0,
            .keyMap={},
        };
        memset(config.keyMap, -1, 128);
        eeprom.Push(0, config);
    }
    if(config.KeyboardBrightness<0.3){
        eeprom.Push(0, config);
    }
    if(config.FilterTime>100||config.FilterTime>400)
    {
        config.FilterTime = 100;
        eeprom.Push(0, config);
    }
    if(config.TouchEndable>1){
        config.TouchEndable = 0;
        eeprom.Push(0, config);
    }
    if(config.RGBLEDMode>=HWKeyboard_RGBLED::ModeTypeMax)
    {
        config.RGBLEDMode = 0;
        eeprom.Push(0, config);
    }
    RGBLED.KeyboardBrightness = config.KeyboardBrightness;
    config.LightON?RGBLED.LightON= true:RGBLED.LightON= false;
    RGBLED.RGBMode = config.RGBLEDMode;
    // Keyboard Report Start
    HAL_TIM_Base_Start_IT(&htim4);
    RGBLED.RGBLED_Init();

    while (true) {

        RGBLED.RGBLED_Handle();

        if(keyboard.isEepPush){     //存入eep
            eeprom.Push(0, config);
            keyboard.isEepPush = false;
        }
        if(config.TouchEndable) {
            switch (keyboard.TouchBarSlide()) {
                case 1:
                    keyboard.DeskTop(HWKeyboard::DeskTopRight);
                    break;
                case 2:
                    keyboard.DeskTop(HWKeyboard::DeskTopLeft);
                    break;
                case 0:
                    break;
            }
        }
    }
}

bool Keyrstatus = false;

/* Event Callbacks -----------------------------------------------------------*/
extern "C" void OnTimerCallback() // 1000Hz callback
{
    keyboard.ScanKeyStates();  // Around 40us use 4MHz SPI clk
    keyboard.ApplyDebounceFilter( config.FilterTime);
    keyboard.Remap(keyboard.FnPressed() ? 2 : 1);  // When Fn pressed use layer-2

    if (keyboard.FnPressed()) {
        keyboard.isFNPress = true;
        if (keyboard.KeyPressed(HWKeyboard::F12)) {
            // do something...
            if (RGBLED.LightON) {
                if (!Keyrstatus) {
                    if (RGBLED.KeyboardBrightness < 1) {
                        RGBLED.KeyboardBrightness += 0.1;
                    } else {
                        RGBLED.KeyboardBrightness = 0.3;
                    }
                    Keyrstatus = true;
                    config.KeyboardBrightness = RGBLED.KeyboardBrightness;
                    keyboard.isEepPush = true;
                }
                // or trigger some keys...
            }
            keyboard.Release(HWKeyboard::F12);
        } else if (keyboard.KeyPressed(HWKeyboard::F11)) {
            if (!Keyrstatus) {
                if(RGBLED.RGBMode<HWKeyboard_RGBLED::ModeTypeMax-1)RGBLED.RGBMode++;
                else RGBLED.RGBMode = 0;
                config.RGBLEDMode = RGBLED.RGBMode;
                Keyrstatus = true;
                keyboard.isEepPush = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::F11);

        } else if (keyboard.KeyPressed(HWKeyboard::SPACE)) {
            if (!Keyrstatus) {
                RGBLED.RGBLED_ctrl(&RGBLED,HWKeyboard_RGBLED::RGBLEDNegate,1);
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::SPACE);
        } else if (keyboard.KeyPressed(HWKeyboard::NUM_1)) {
            if (!Keyrstatus) {
                config.FilterTime = 100;
                RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_r{(uint8_t) 187, 167, 16});
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::NUM_1);
        } else if (keyboard.KeyPressed(HWKeyboard::NUM_2)) {
            if (!Keyrstatus) {
                config.FilterTime = 200;
                RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_r{(uint8_t) 72, 200, 224});
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::NUM_2);
        } else if (keyboard.KeyPressed(HWKeyboard::NUM_3)) {
            if (!Keyrstatus) {
                config.FilterTime = 300;
                RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_r{(uint8_t) 45, 23, 222});
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::NUM_3);

        } else if (keyboard.KeyPressed(HWKeyboard::NUM_4)) {
            if (!Keyrstatus) {
                config.FilterTime = 400;
                RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_r{(uint8_t) 213, 18, 232});
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::NUM_4);
        } else if (keyboard.KeyPressed(HWKeyboard::F1)) {
            if (!Keyrstatus) {
                config.TouchEndable = 1-config.TouchEndable;
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::F1);
        } else if (keyboard.KeyPressed(HWKeyboard::F2)) {
            if (!Keyrstatus) {
                keyboard.isEepPush = true;
                if(keyboard.TouchTestLED)
                    keyboard.TouchTestLED = false;
                else
                    keyboard.TouchTestLED = true;
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::F2);

        } else if (keyboard.KeyPressed(HWKeyboard::VOLUME_UP)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::VOLUME_UP);
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::VOLUME_UP);

        } else if (keyboard.KeyPressed(HWKeyboard::VOLUME_DOWN)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
                Keyrstatus = true;
            }
            // or trigger some keys...
            keyboard.Release(HWKeyboard::VOLUME_DOWN);

        } else {
            keyboard.MediaRelease();
            Keyrstatus = false;
        }
    } else {
        keyboard.MediaRelease();
        keyboard.isFNPress = false;
        Keyrstatus = false;
    }

    if(MediaCount>0)
    {
        if (!Keyrstatus) {
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
        }
        keyboard.Release(HWKeyboard::VOLUME_UP);
        MediaCount--;
    }
    else if(MediaCount<0)
    {
        if (!Keyrstatus) {
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
        }
        keyboard.Release(HWKeyboard::VOLUME_DOWN);
        MediaCount++;
    }

    if(Dynamic_BUF[0]==0x01)
    {
        if (Dynamic_BUF[1] == 0x01) {
            MediaCount++;
        } else if (Dynamic_BUF[1] == 0x02) {
            MediaCount--;
        }
    }
    else if(Dynamic_BUF[0]==0x02)
    {
        RGBLED.KeyboardBrightness=(float)Dynamic_BUF[1]/100;
        if(RGBLED.KeyboardBrightness>1)RGBLED.KeyboardBrightness=1;
        else if(RGBLED.KeyboardBrightness<0)RGBLED.KeyboardBrightness=0;
    }
    else if(Dynamic_BUF[0]==0x03)
    {
        keyboard.MouseMove(Dynamic_BUF[1]);
    }
    else if(Dynamic_BUF[0]==0x08)
    {
        config.KeyboardBrightness = RGBLED.KeyboardBrightness;
        keyboard.isEepPush = true;
    }
    Dynamic_BUF[0] = 0x00;
    Dynamic_BUF[1] = 0x00;


    if(keyboard.KeyDelayCnt) {
        keyboard.KeyDelayCnt--;
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                                   keyboard.GetHidReportBuffer(1),
                                   HWKeyboard::KEY_REPORT_SIZE);
    }

}

extern "C"
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    RGBLED.isRgbTxBusy = false;
}
extern "C"
void HID_RxCpltCallback(uint8_t* _data)
{
    //键盘LED回调
    RGBLED.isNumLocked = _data[1]&0x01?true: false;
    RGBLED.isCapsLocked = _data[1]&0x02? true: false;
    RGBLED.isScrlkLocked = _data[1]&0x04?true: false;
    RGBLED.isComposeLocked = _data[1]&0x08?true: false;
    RGBLED.isKanaLocked = _data[1]&0x10?true: false;
}
