#ifndef __M5_PERIPHERAL_H
#define __M5_PERIPHERAL_H

#include "Arduino.h"

#define M5LCD_LINE1                                1
#define M5LCD_LINE2                                2

#define cOFF                                       0x000000
#define cYELLOW                                    0xfff000
#define cRED                                       0xff0000
#define cBLUE                                      0x0000f0
#define cGREEN                                     0x00ff00

enum {
    LED_EVENT_NONE = 0,
    LED_EVENT_NOT_CONFIGURED,
    LED_EVENT_CONFIGURED,
    LED_EVENT_ERROR,
    LED_EVENT_WATER_NONE,
    LED_EVENT_WATER_DETECTED,
    LED_EVENT_WATER_ALERT1,
    LED_EVENT_WATER_ALERT2,
    LED_EVENT_WATER_ALERT3,
    LED_EVENT_WATER_ALERT4,
    LED_EVENT_WATER_KILLER,
};

enum {
    BUZZER_EVENT_NONE = 0,
    BUZZER_EVENT_APP_INIT,
    BUZZER_EVENT_BTN_HOLS5s,
    BUZZER_EVENT_WATER_ALERT1,
    BUZZER_EVENT_WATER_ALERT2,
    BUZZER_EVENT_WATER_ALERT3,
    BUZZER_EVENT_WATER_ALERT4,
    BUZZER_EVENT_WATER_KILLER,
};

void M5PeripheralInit();

void LedShow(uint8_t event);

void BuzzerPlay(uint8_t event);

void M5LedOn();

void M5LedOff();

void M5PowerOff();

void M5LCDOff();

void M5LCDOn();

void M5Beep(int tone, int timeOn);

void M5BeepC(uint16_t tone, uint16_t interval, uint16_t timeOn, uint16_t count);

bool M5BuzzIsOn(void);

void M5LCDPrintf(int line, String txt);

void M5LCDShowSignal(int16_t * buffer);

int M5BuzzCount(void);

#endif /* __M5_PERIPHERAL_H */
