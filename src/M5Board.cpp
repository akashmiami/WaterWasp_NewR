#include "M5Board.h"

M5Board::M5Board()
{
}

M5Board::~M5Board()
{
}

void M5Board::begin()
{
    if (_isInited)
        return;

    _isInited = true;
}

void M5Board::update()
{

#if BOARD_USING == BOARD_ESP32

#elif BOARD_USING == BOARD_M5STICKC

#elif BOARD_USING == BOARD_M5STICKCPLUS

#elif BOARD_USING == BOARD_M5ATOM
    M5.Btn.read();
#elif BOARD_USING == BOARD_ESP32CAM
    M5.Btn.read();
#elif BOARD_USING == BOARD_M5TIMER_CAM
    M5.Btn.read();
#else

#endif

}

M5Board M5;
