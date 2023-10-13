#include "Arduino.h"
#include "M5Led.h"
#if (BOARD_USING == BOARD_M5ATOM)
#include "Adafruit_NeoPixel.h"
#endif


#define GET_RED(color)                             ((color >> 16ul) & 0xFFul)
#define GET_GREEN(color)                           ((color >> 8ul) & 0xFFul)
#define GET_BLUE(color)                            ((color) & 0xFFul)

#if (BOARD_USING == BOARD_M5ATOM)
Adafruit_NeoPixel pixels(NUM_LEDS, M5_LED, NEO_GRB + NEO_KHZ800);
#endif

M5Led::M5Led()
{
#if (BOARD_USING == BOARD_M5ATOM)
    pixels.begin();
    pixels.clear();
#elif (BOARD_USING == BOARD_ESP32CAM)
    pinMode(LEDR_PIN, OUTPUT);
    pinMode(LEDW_PIN, OUTPUT);
    digitalWrite(LEDW_PIN, LOW);
    digitalWrite(LEDR_PIN, LOW);
#elif (BOARD_USING == BOARD_M5TIMER_CAM)
    pinMode(LEDB_PIN, OUTPUT);
    digitalWrite(LEDB_PIN, HIGH);
#endif
}

M5Led::~M5Led()
{

}

void M5Led::LedOn(int ledi)
{

}

void M5Led::LedOff(int ledi)
{

}

void M5Led::LedSetColor(int ledi, uint32_t color)
{
#if (BOARD_USING == BOARD_M5ATOM)
    pixels.setPixelColor(ledi, pixels.Color(GET_RED(color), GET_GREEN(color), GET_BLUE(color)));
    pixels.show();
#elif (BOARD_USING == BOARD_ESP32CAM)
    switch (ledi)
    {
    case 0:
        if (GET_RED(color)) {
            digitalWrite(LEDR_PIN, HIGH);
        }
        else {
            digitalWrite(LEDR_PIN, LOW);
        }

        if (GET_GREEN(color)) {
            digitalWrite(LEDW_PIN, HIGH);
        }
        else {
            digitalWrite(LEDW_PIN, LOW);
        }
        break;
    
    default:
        break;
    }
#elif (BOARD_USING == BOARD_M5TIMER_CAM)
    switch (ledi)
    {
    case 0:
        if (GET_GREEN(color) || GET_RED(color)) {
            digitalWrite(LEDB_PIN, LOW);
        }
        else {
            digitalWrite(LEDB_PIN, HIGH);
        }
        break;
    
    default:
        break;
    }
#endif
}