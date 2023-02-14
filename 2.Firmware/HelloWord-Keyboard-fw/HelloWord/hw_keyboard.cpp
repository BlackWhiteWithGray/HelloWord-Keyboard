#include <cstring>
#include "hw_keyboard.h"
#include "usbd_custom_hid_if.h"
#include "RGBLight.h"

extern HWKeyboard_RGBLED RGBLED;

inline void DelayUs(uint32_t _us)
{
    for (int i = 0; i < _us; i++)
        for (int j = 0; j < 8; j++)  // ToDo: tune this for different chips
            __NOP();
}


uint8_t* HWKeyboard::ScanKeyStates()
{
    memset(spiBuffer, 0xFF, IO_NUMBER / 8 + 1);
    PL_GPIO_Port->BSRR = PL_Pin; // Latch

    spiHandle->pRxBuffPtr = (uint8_t*) spiBuffer;
    spiHandle->RxXferCount = IO_NUMBER / 8 + 1;
    __HAL_SPI_ENABLE(spiHandle);
    while (spiHandle->RxXferCount > 0U)
    {
        if (__HAL_SPI_GET_FLAG(spiHandle, SPI_FLAG_RXNE))
        {
            /* read the received data */
            (*(uint8_t*) spiHandle->pRxBuffPtr) = *(__IO uint8_t*) &spiHandle->Instance->DR;
            spiHandle->pRxBuffPtr += sizeof(uint8_t);
            spiHandle->RxXferCount--;
        }
    }
    __HAL_SPI_DISABLE(spiHandle);

    PL_GPIO_Port->BRR = PL_Pin; // Sample
    return scanBuffer;
}


void HWKeyboard::ApplyDebounceFilter(uint32_t _filterTimeUs)
{
    memcpy(debounceBuffer, spiBuffer, IO_NUMBER / 8 + 1);

    DelayUs(_filterTimeUs);
    ScanKeyStates();

    uint8_t mask;
    for (int i = 0; i < IO_NUMBER / 8 + 1; i++)
    {
        mask = debounceBuffer[i] ^ spiBuffer[i];
        spiBuffer[i] |= mask;
    }
}


uint8_t* HWKeyboard::Remap(uint8_t _layer)
{
    int16_t index, bitIndex;
    memset(RGBLED.RGBLEDKeyBuf,0,11);
    memset(remapBuffer, 0, IO_NUMBER / 8);
    for (int16_t i = 0; i < IO_NUMBER / 8; i++)
    {
        for (int16_t j = 0; j < 8; j++)
        {
            index = (int16_t) (keyMap[0][i * 8 + j] / 8);
            bitIndex = (int16_t) (keyMap[0][i * 8 + j] % 8);
            if (scanBuffer[index] & (0x80 >> bitIndex))
                remapBuffer[i] |= 0x80 >> j;
        }

        remapBuffer[i] = ~remapBuffer[i];
        RGBLED.RGBLEDKeyBuf[i] = remapBuffer[i];
    }
    memcpy(LastHidBuffer,hidBuffer,KEY_REPORT_SIZE);
    memset(hidBuffer, 0, KEY_REPORT_SIZE);

    int i = 0, j = 0;
    while (8 * i + j < IO_NUMBER - 6)
    {
        for (j = 0; j < 8; j++) {
            index = (int16_t) (keyMap[_layer][i * 8 + j] / 8 + 1); // +1 for modifier
            bitIndex = (int16_t) (keyMap[_layer][i * 8 + j] % 8);
            if (bitIndex < 0) {
                index -= 1;
                bitIndex += 8;
            } else if (index > 100)
                continue;

            if (remapBuffer[i] & (0x80 >> j)) {
                hidBuffer[index + 1] |= 1 << (bitIndex); // +1 for Report-ID
            }
        }
        i++;
        j = 0;
    }

    if(memcmp(LastHidBuffer+1,hidBuffer+1,KEY_REPORT_SIZE-1)==0) {
        isKeyReport = false;
    }else{
        isKeyReport = true;
        KeyDelayCnt = 20;
    }
    return hidBuffer;
}


bool HWKeyboard::FnPressed()
{
    return remapBuffer[9] & 0x04;
}

uint16_t HWKeyboard::GetHidReportSize(uint8_t _reportId)
{
    switch (_reportId)
    {
        case 1:
            return KEY_REPORT_SIZE;
        //case 2:
        //    return RAW_REPORT_SIZE;
        case 2:
            return MEDIA_REPORT_SIZE;
        case 3:
            return MOUSE_REPORT_SIZE;
        default:
            return KEY_REPORT_SIZE;
    }

}

uint8_t* HWKeyboard::GetHidReportBuffer(uint8_t _reportId)
{
    switch (_reportId)
    {
        case 1:
            hidBuffer[0] = 1;
            return hidBuffer;
        case 2:
            hidBuffer[KEY_REPORT_SIZE] = 2;
            return hidBuffer + KEY_REPORT_SIZE;
        case 3:
            hidBuffer[RAW_REPORT_SIZE] = 3;
            return hidBuffer + RAW_REPORT_SIZE;
        case 4:
            hidBuffer[MEDIA_REPORT_SIZE] = 4;
            return hidBuffer + MEDIA_REPORT_SIZE;
        default:
            return hidBuffer;
    }
}

//检测按键是否被按下
bool HWKeyboard::KeyPressed(KeyCode_t _key)
{
    int index, bitIndex;

    if (_key < RESERVED)
    {
        //index = _key / 8;
        index = 0;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }

    return hidBuffer[index + 1] & (1 << bitIndex);
}

//报告按键触发
void HWKeyboard::Press(HWKeyboard::KeyCode_t _key)
{
    int index, bitIndex;
    memcpy(LastHidBuffer,hidBuffer,KEY_REPORT_SIZE);
    if (_key < RESERVED)
    {
        //index = _key / 8;
        index = 0;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }
    if(memcmp(LastHidBuffer+1,hidBuffer+1,KEY_REPORT_SIZE-1)==0) {
        isKeyReport = false;
    }else{
        isKeyReport = true;
        KeyDelayCnt = 20;
    }
    hidBuffer[index + 1] |= (1 << bitIndex);
}

//报告释放按键
void HWKeyboard::Release(HWKeyboard::KeyCode_t _key)
{
    int index, bitIndex;
    memcpy(LastHidBuffer,hidBuffer,KEY_REPORT_SIZE);
    if (_key < RESERVED)
    {
        //index = _key / 8;
        index = 0;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }
    if(memcmp(LastHidBuffer+1,hidBuffer+1,KEY_REPORT_SIZE-1)==0) {
        isKeyReport = false;
    }else{
        isKeyReport = true;
        KeyDelayCnt = 20;
    }
    hidBuffer[index + 1] &= ~(1 << bitIndex);
}

extern USBD_HandleTypeDef hUsbDeviceFS;

void HWKeyboard::MediaPress(HWKeyboard::KeyCode_t _key)
{
    memcpy(LastHidBuffer+RAW_REPORT_SIZE,hidBuffer+RAW_REPORT_SIZE,MEDIA_REPORT_SIZE);
    switch(_key){
        case VOLUME_UP:
            MediaDelaycnt = 10;
            hidBuffer[RAW_REPORT_SIZE+1] = 0x40;
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                                       GetHidReportBuffer(3),
                                       HWKeyboard::MEDIA_REPORT_SIZE);
            break;
        case VOLUME_DOWN:
            MediaDelaycnt = 10;
            hidBuffer[RAW_REPORT_SIZE+1] = 0x80;
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                                       GetHidReportBuffer(3),
                                       HWKeyboard::MEDIA_REPORT_SIZE);
            break;
        default:

            break;
    }

    if(memcmp(LastHidBuffer+RAW_REPORT_SIZE+1,hidBuffer+RAW_REPORT_SIZE+1,MEDIA_REPORT_SIZE-1)==0) {
        isMediaReport = false;
    }else{
        isMediaReport = true;
    }
}



void HWKeyboard::MediaRelease(void)
{

    if(MediaDelaycnt){
        MediaDelaycnt--;
        hidBuffer[RAW_REPORT_SIZE+1] = 0x00;
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                                   GetHidReportBuffer(3),
                                   HWKeyboard::MEDIA_REPORT_SIZE);
    }
}


void HWKeyboard::MouseMove(uint8_t move)
{
    hidBuffer[MEDIA_REPORT_SIZE+4] = move-128;
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                               GetHidReportBuffer(4),
                               HWKeyboard::MOUSE_REPORT_SIZE);
}

uint8_t HWKeyboard::GetTouchBarState(uint8_t _id)
{
    uint8_t tmp = (remapBuffer[10] & 0b00000001)  |
                  (remapBuffer[10] & 0b00000010) << 4 |
                  (remapBuffer[10] & 0b00000100) << 2 |
                  (remapBuffer[10] & 0b00001000) >> 0 |
                  (remapBuffer[10] & 0b00010000) >> 2 |
                  (remapBuffer[10] & 0b00100000) >> 4;
    return _id == 0 ? tmp : (tmp & (1 << (_id - 1)));
}


uint8_t  HWKeyboard::TouchBarSlide(void) {
    bool TouchBarState[6];
    for(int i=0;i<6;i++) {
        TouchBarState[i] = GetTouchBarState(i+1);
    }
    if(TouchBarState[0]+TouchBarState[1]+TouchBarState[2]+TouchBarState[3]+TouchBarState[4]+TouchBarState[5]==0)
    {
        if(TouchBarTime>20)TouchBarTime = 0,TouchBarCount=0;
        else TouchBarTime++;
    }
    else TouchBarTime = 0;

    if(TouchBarState[0]==1&&TouchBarCount==0)TouchBarCount=1;
    if(TouchBarState[1]==1&&TouchBarCount==1)TouchBarCount=2;
    if(TouchBarState[2]==1&&TouchBarCount==2)TouchBarCount=3;
    if(TouchBarState[3]==1&&TouchBarCount==3)TouchBarCount=4;
    if(TouchBarState[4]==1&&TouchBarCount==4)TouchBarCount=5;
    if(TouchBarState[5]==1&&TouchBarCount==5)
    {
        TouchBarCount=0;
        return 1;
    }

    if(TouchBarState[5]==1&&TouchBarCount==0)TouchBarCount=11;
    else if(TouchBarState[4]==1&&TouchBarCount==11)TouchBarCount=12;
    else if(TouchBarState[3]==1&&TouchBarCount==12)TouchBarCount=13;
    else if(TouchBarState[2]==1&&TouchBarCount==13)TouchBarCount=14;
    else if(TouchBarState[1]==1&&TouchBarCount==14)TouchBarCount=15;
    else if(TouchBarState[0]==1&&TouchBarCount==15)
    {
        TouchBarCount=0;
        return 2;
    }
    return 0;
}

void HWKeyboard::DeskTop(HWKeyboard::KeyCode_t _key)
{
    switch(_key){
        case DeskTopRight:
            hidBuffer[1] |= 0x09;
            hidBuffer[11] |= 0x80;
            break;
        case DeskTopLeft:
            hidBuffer[1] |= 0x09;
            hidBuffer[12] |= 0x01;
            break;
        default:
            break;
    }
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                               GetHidReportBuffer(1),
                               HWKeyboard::KEY_REPORT_SIZE);

}