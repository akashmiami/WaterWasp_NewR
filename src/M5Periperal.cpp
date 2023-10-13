
#include "AppConfig.h"
#include "Log.h"
#include "VTimer.h"
#include "Portal.h"
#include "M5Board.h"
#include "M5Peripheral.h"

#define BLINK_FOREVER                          0xFFFF
#define BEEP_FORERVER                          0xFFFF

typedef struct __ledblink {
    uint32_t color;           /* Color blink ON */
    uint32_t lcolor;          /* Color after blink done */
    uint16_t count;           /* Toggle count */
} ledblink_t;

typedef struct __buzzplay {
    uint16_t freq;
    uint16_t timeon;
    uint16_t interval;
    uint16_t count;
} buzzplay_t;

bool buzzIsOn = false;
buzzplay_t buzp;
uint8_t led_old_event = LED_EVENT_NONE;
uint8_t bz_old_event = BUZZER_EVENT_NONE;

void M5PeripheralInit()
{
    #if (BOARD_USING == BOARD_M5STICKC)
    LOG_DBG("%s", "M5StickC Peripheral Init");
    #elif (BOARD_USING == BOARD_M5STICKCPLUS)
    LOG_DBG("%s", "M5StickCPlus Peripheral Init");
    #elif (BOARD_USING == BOARD_M5ATOM)
    LOG_DBG("%s", "M5Atom Peripheral Init");
    #elif (BOARD_USING == BOARD_ESP32)
    LOG_DBG("%s", "ESP32 Peripheral Init");
    #endif
    
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    // Init Button
    pinMode(M5_BUTTON_HOME, INPUT);
    pinMode(M5_BUTTON_RST, INPUT);
    // Init Led
    pinMode(M5_LED, OUTPUT);
    M5LedOff();
    #elif ENABLE_EXTERNAL_BUZZER
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, 10);
    ledcAttachPin(BUZZER_EXT_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0); 
    #endif
}

void M5LedSet(uint32_t color)
{
    M5.Led.LedSetColor(0, color);
}

void LedBlink(void *arg)
{
    static uint8_t toggle = 0;

    if (arg == NULL) {
        return;
    }

    ledblink_t *bl = (ledblink_t *)arg;

    toggle = !toggle;
    if (toggle) {
        M5LedSet(bl->color);
    }
    else {
        M5LedSet(cOFF);
    }

    if (bl->count != BLINK_FOREVER) {
        bl->count--;
    }

    if (bl->count) {
        VTimerStart(200, LedBlink, arg);
    }
    else if (led_old_event >= LED_EVENT_WATER_ALERT1 && led_old_event <= LED_EVENT_WATER_ALERT4) {
        bl->color = cGREEN;
        VTimerStart(200, LedBlink, arg);
    }
}

void LedShow(uint8_t event) 
{
    static ledblink_t cled = { cOFF, cOFF, 0 };

    if (led_old_event == event) {
        return;
    }

    if (LED_EVENT_WATER_KILLER >= led_old_event && led_old_event >= LED_EVENT_WATER_DETECTED) {
        VTimerCancel(LedBlink);
        cled.color = cOFF;
        cled.count = 0;
    }

    led_old_event = event;
    LOG_INFO("Led ev %d", event);
    switch (event)
    {
        case LED_EVENT_NOT_CONFIGURED:
            M5LedSet(cYELLOW);
            break;

        case LED_EVENT_CONFIGURED:
            M5LedSet(cBLUE);
            break;

        case LED_EVENT_ERROR:
            M5LedSet(cRED);
            break;

        case LED_EVENT_WATER_KILLER:
            /* Blink Red */
            cled.color = cRED;
            VTimerStart(200, LedBlink, &cled);
            break;

        case LED_EVENT_WATER_DETECTED:
            M5LedOn();
            cled.color = cGREEN;
            VTimerStart(500, LedBlink, &cled);
            break;

        case LED_EVENT_WATER_ALERT1:
        case LED_EVENT_WATER_ALERT2:
        case LED_EVENT_WATER_ALERT3:
        case LED_EVENT_WATER_ALERT4:
            cled.count = (event - LED_EVENT_WATER_ALERT1 + 1) * 10 + 1;
            cled.color = cRED;
            M5LedSet(cRED);
            VTimerStart(500, LedBlink, &cled);
            break;

        case LED_EVENT_WATER_NONE:
            M5LedOff();
            if (PortalExistConfig()) {
                M5LedSet(cBLUE);
            }
            else {
                M5LedSet(cYELLOW);
            }
            
            break;

        default:
            break;
    }
}

void M5LedOn()
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    digitalWrite(M5_LED, LOW);  // LED ON
    #endif
}

void M5LedOff()
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    digitalWrite(M5_LED, HIGH);  // LED OFF
    #endif
}

void M5PowerOff()
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    M5.Axp.PowerOff();
    #endif
}

void M5LCDOff()
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    M5.Lcd.fillScreen(BLACK);
    #endif
}

void M5LCDOn()
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    M5.Lcd.fillScreen(WHITE);
    #endif
}

void BuzzOffCallback(void *arg)
{
    #if (BOARD_USING == BOARD_M5STICKCPLUS)
    M5.Beep.mute();
    LOG_DBG("%s", "Buzz off");
    buzzIsOn = false;
    #elif ENABLE_EXTERNAL_BUZZER
    ledcWrite(BUZZER_CHANNEL, 0);
    LOG_DBG("%s", "Buzz off");
    buzzIsOn = false;
    #endif
}

void M5Beep(int tone, int timeOn)
{
    int enbuzz = ConfigGetInt(CONF_BUZZ_EN);

    if (!enbuzz) {
        return;
    }

    #if (BOARD_USING == BOARD_M5STICKCPLUS)
    buzzIsOn = true;
    M5.Beep.tone(tone);
    M5.Beep.beep();
    LOG_DBG("%s", "Buzz on");
    VTimerStart(timeOn, BuzzOffCallback, NULL);
    #elif ENABLE_EXTERNAL_BUZZER
    buzzIsOn = true;
    ledcWrite(BUZZER_CHANNEL, 512);
    LOG_DBG("%s", "Buzz on");
    VTimerStart(timeOn, BuzzOffCallback, NULL);
    #endif
}

void BuzzerOnInterval(void *arg)
{
    buzzplay_t *bz = (buzzplay_t *)arg;

    if (arg == NULL) {
        return;
    }

    M5Beep(bz->freq, bz->timeon);
    if (bz->count != BEEP_FORERVER) {
        bz->count--;
    }

    if (bz->count) {
        VTimerStart(bz->interval, BuzzerOnInterval, arg);
    }
    else {

    }
}

void M5BeepC(uint16_t tone, uint16_t interval, uint16_t timeOn, uint16_t count)
{
    buzp.freq = tone;
    buzp.interval = interval;
    buzp.timeon = timeOn;
    buzp.count = count;

    BuzzerOnInterval(&buzp);
}

bool M5BuzzIsOn(void)
{
    return buzzIsOn;
}

void BuzzerPlay(uint8_t event) {
    if (bz_old_event == event) {
        return;
    }

    if (bz_old_event == BUZZER_EVENT_WATER_KILLER) {
        VTimerCancel(BuzzerOnInterval);
    }

    LOG_INFO("Buzzer event %d", event);
    bz_old_event = event;
    switch (event) {
        case BUZZER_EVENT_APP_INIT:
            M5Beep(4000, 500);
            break;

        case BUZZER_EVENT_BTN_HOLS5s:
            M5Beep(4000, 500);
            break;

        case BUZZER_EVENT_WATER_ALERT1:
        case BUZZER_EVENT_WATER_ALERT2:
        case BUZZER_EVENT_WATER_ALERT3:
        case BUZZER_EVENT_WATER_ALERT4: {
            LOG_INFO(">>> Water Alert %d ===================",
                     event + 1 - BUZZER_EVENT_WATER_ALERT1);
            // int interval = GetBuzeerIntervalWarning();
            // if (interval < 800) {
            //     LOG_ERR("Interval msec error [ %.1f > 800 ]", interval);
            //     interval = DEF_CONF_BEEP_WARNING_INTERVAL * 1000;
            // }
            buzp.count = (event - BUZZER_EVENT_WATER_ALERT1) + 1;
            LOG_INFO(">>> Buzz count %d ---------",
                     buzp.count);
            M5BeepC(4000, 1500, 800, buzp.count);
            break;
        }

        case BUZZER_EVENT_WATER_KILLER: {
            LOG_INFO("%s", ">>> Water killer ===================");
            // int interval = GetBuzeerIntervalWarning();
            // if (interval < 800) {
            //     LOG_ERR("Interval msec error [ %.1f > 800 ]", interval);
            //     interval = DEF_CONF_BEEP_WARNING_INTERVAL * 1000;
            // }
            M5BeepC(4000, 500, 300, BEEP_FORERVER);  
            break;
        }
    }
}

int M5BuzzCount(void)
{
    return bz_old_event - BUZZER_EVENT_WATER_ALERT1 + 1;
}

void M5LCDPrintf(int line, String txt)
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    M5.update();

    if (line == M5LCD_LINE1) {
        M5.Lcd.setCursor(5, 8, 2);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setRotation(3);
        M5.Lcd.fillScreen(WHITE);
        M5.Lcd.setTextColor(BLACK, WHITE);
    }
    else if (line == M5LCD_LINE2) {
        M5.Lcd.setCursor(5, 55, 2);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setRotation(3);
        M5.Lcd.setTextColor(BLACK, WHITE);
    }

    M5.Lcd.println(txt);
    #endif
}

void M5LCDShowSignal(int16_t * buffer)
{
    #if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    int y;
    static uint16_t oldy[160];

    for (int n = 0; n < 160; n++) {
        y = buffer[n] * DEF_CONF_GAIN_FACTOR;
        y = map(y, INT16_MIN, INT16_MAX, 10, 70);
        M5.Lcd.drawPixel(n, oldy[n], WHITE);
        M5.Lcd.drawPixel(n, y, BLACK);
        oldy[n] = y;
    }
    #endif
}
