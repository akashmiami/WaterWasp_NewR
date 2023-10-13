#ifndef __M5LED_H
#define __M5LED_H

#include <stdint.h>
#include "AppConfig.h"

class M5Led
{
private:

public:
    M5Led();
    ~M5Led();

    void LedOn(int ledi);
    void LedOff(int ledi);
    void LedSetColor(int ledi, uint32_t color);
};

#endif /* __M5LED_H */
