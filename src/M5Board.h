#ifndef _M5ATOM_H_
#define _M5ATOM_H_

#if defined(ESP32)

#include "AppConfig.h"
#include <Arduino.h>
#include "M5Button.h"
#include "M5Led.h"

class M5Board
{
private:
    bool _isInited = false;

public:
    M5Board();
    ~M5Board();

#if BOARD_USING == BOARD_ESP32

#elif BOARD_USING == BOARD_M5STICKC

#elif BOARD_USING == BOARD_M5STICKCPLUS

#elif BOARD_USING == BOARD_M5ATOM
    M5Button Btn = M5Button(BUTTON_PIN, true, 10);
#elif BOARD_USING == BOARD_ESP32CAM
    M5Button Btn = M5Button(BUTTON_PIN, true, 10);
#elif BOARD_USING == BOARD_M5TIMER_CAM
    M5Button Btn = M5Button(BUTTON_PIN, true, 10);
#else

#endif

    M5Led Led = M5Led();

    void begin();
    void update();
};

extern M5Board M5;

#else
#error “This library only supports boards with ESP32 processor.”
#endif
#endif
