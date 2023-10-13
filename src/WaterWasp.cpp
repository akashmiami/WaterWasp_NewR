/*
 *  @file: WaterWasp.ino
 *
 *  Created on:
 */

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "WiFi.h"
#include "ArduinoJson.h"
#include "Portal.h"
#include "AppConfig.h"
#include "Log.h"
#include "Queue.h"
#include "Detection.h"
#include "Portal.h"
#include "HttpAPI.h"
#include "HttpOTA.h"
#include "VTimer.h"
#include "Utils.h"
#include "TimeSync.h"
#include "M5Board.h"
#include "M5Peripheral.h"

/******************************************************************************/
/*                              TYPES and DEFINITIONS                         */
/******************************************************************************/

#define IFTTT_OFF 0
#define IFTTT_ON 1

#define IF_EXPRIED_RUN_SECTION(last, interval, section) \
    do                                                  \
    {                                                   \
        if (last + interval < mticks)                   \
        {                                               \
            last = mticks;                              \
            section                                     \
        }                                               \
    } while (0)

enum
{
    CLOUD_STATE_GET_USERID = 1,
    CLOUD_STATE_GET_TOKEN,
    CLOUD_STATE_CHECK_DEVICE_REGISTER,
    CLOUD_STATE_SEND_DATA
};

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static bool first_time = true;
static bool ifttt_off = true;

static unsigned long detected_start = 0;
static unsigned long detected_last = 0;
static unsigned long detected_startx = 0;
static bool is_water_killer_reset = false;
static bool ota_available = false;

unsigned long last_request = 0;
unsigned long last_warning = 0;
unsigned long last_send_heartbeat = 0;
unsigned long last_poll_config = 0;
unsigned long last_poll_command = 0;
unsigned long last_reconnect = 0;
unsigned long next_on_ifttt = 0;

uint32_t next_request = REQUEST_NORMAL;
uint32_t cloud_state = CLOUD_STATE_GET_USERID;
static bool device_register = false;

static char strbody[4096];
char user_email[EMAIL_MAX_LENGTH];
char user_id[USERID_MAX_LENGTH];
char user_token[TOKEN_MAX_LENGTH];

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void HandleCloudRequest();
static void HandleNewCommand();
static void HandlePollConfig();
static void HandleIFTTT(uint8_t state);
static void SendHeartBeat();
static void HandleReconnect(void);

void M5LCDShowHomeScreen()
{
#if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    M5LCDPrintf(M5LCD_LINE1, "~~~ WATER WASP ~~~");
    if (PortalExistConfig())
    {
        String ip_addr = "Unknown";
        if (WiFi.status() == WL_CONNECTED)
        {
            ip_addr = WiFi.localIP().toString();
        }
        M5LCDPrintf(M5LCD_LINE2, ip_addr);
    }
    else
    {
        LOG_INFO("=== AP mode, ip %s", SOFT_AP_IP_ADDR);
        M5LCDPrintf(M5LCD_LINE2, String("AP mode, ip ") + String(SOFT_AP_IP_ADDR));
    }
#endif /* (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS) */
}

void M5LCDShowWaterResult(float total_time, float total_galons)
{
    char txtshow[32];
    char date[32];
    char time[32];

    GetDate(date);
    GetTime(time);
    sprintf(txtshow, "%.2f gal in %.2f sec", total_galons, total_time);
    LOG_INFO("=========> Water result [%s] <=========", txtshow);
    M5LCDPrintf(M5LCD_LINE1, txtshow);
    M5LCDPrintf(M5LCD_LINE2, String(date) + String(" ") + String(time));
}

void setup()
{
    M5.begin();
    Serial.begin(115200);
    llog_init();
    LOG_INFO(">>> App start [=%s]!!!, free heap %ld", FIRMWARE_VERSION, xPortGetFreeHeapSize());

    ConfigInit();
    RecordInit();
    VTimerInit();
    M5PeripheralInit();
    BuzzerPlay(BUZZER_EVENT_APP_INIT);
    M5LCDShowHomeScreen();
    DetectionInit();
    PortalInit();
    TimeSyncInit();
    if (PortalExistConfig())
    {
        LedShow(LED_EVENT_CONFIGURED);

        const timezone_t *tz = PortalGetConfigTZ();
        if (tz != NULL)
        {
            TimeSyncSetTz(tz->tz);
        }
    }
    else
    {
        LedShow(LED_EVENT_NOT_CONFIGURED);
    }

#if 0
    ConfigSetInt(CONF_MODEL, MODEL_CNN);
    ConfigSetInt(CONF_MIC_VOL_GAIN, 2);
    ConfigSetInt(CONF_DET_TCNT, 6);
    ConfigSetDouble(CONF_DET_THRES, 0.56);
    ConfigSetInt(CONF_DET_TABL_SIZE, 16);
#endif
    // ConfigSetInt(CONF_MODEL, MODEL_SVM);
    // ConfigSetInt(CONF_MIC_VOL_GAIN, 1);
    // ConfigSetDouble(CONF_DET_THRES, 21000);
    // ConfigSetInt(CONF_DET_TCNT, 30);
    // ConfigSetInt(CONF_DET_TABL_SIZE, 200);
    // ConfigSetInt(CONF_WATER_ALERT1_TIME, 20);
    // ConfigSetInt(CONF_WATER_ALERT2_TIME, 40);
    // ConfigSetInt(CONF_WATER_ALERT3_TIME, 60);
    // ConfigSetInt(CONF_WATER_ALERT4_TIME, 80);
    // ConfigSetInt(CONF_WATER_KILLER_TIME, 120);
    // ConfigSetInt(CONF_WATER_AVG_RESET_TIME, 30);
}

void loop()
{
    unsigned long mticks = millis();
    bool is_connected = (WiFi.status() == WL_CONNECTED);
    bool is_run = false;
    bool result = false;
    float total_time;
    float total_galons;
    static bool fl = true;

    if (fl)
    {
        LOG_INFO("The loop run core %d, free heap %ld",
                 xPortGetCoreID(), xPortGetFreeHeapSize());
        fl = false;
    }

    M5.update();

    if (is_connected)
    {
        if (!is_run)
        {
            // get token / email / send log
            IF_EXPRIED_RUN_SECTION(last_request, next_request,
                                   is_run = true;
                                   HandleCloudRequest(););
        }

        if (!is_run)
        {
            // get new command
            IF_EXPRIED_RUN_SECTION(last_poll_command, POLL_NEW_COMMAND_INTERVAL,
                                   is_run = true;
                                   HandleNewCommand(););
        }

        if (!is_run)
        {
            // poll config
            IF_EXPRIED_RUN_SECTION(last_poll_config, POLL_CONFIG_INTERVAL,
                                   is_run = true;
                                   HandlePollConfig(););
        }

        if (!is_run)
        {
            // send heartbeat
            IF_EXPRIED_RUN_SECTION(last_send_heartbeat, HEARTBEAT_INTERVAL,
                                   is_run = true;
                                   SendHeartBeat(););
        }

        // send heartbeat when device regiterd and first time connected wifi
        if (first_time && device_register)
        {
            first_time = false;
            SendHeartBeat();
            HandleIFTTT(IFTTT_ON);
        }

        last_reconnect = mticks;
    }
    else
    {
        // Reconnect
        IF_EXPRIED_RUN_SECTION(last_reconnect, 15000,
                               is_run = true;
                               HandleReconnect(););
    }

#if (BOARD_USING == BOARD_M5STICKC || BOARD_USING == BOARD_M5STICKCPLUS)
    if (M5.BtnA.isPressed())
    {
        LOG_DBG("%s", "Button A pressed");
        if (detected_start != 0)
        {
            total_time = (float)(mticks - detected_start) / 1000; // msec to sec
            total_galons = total_time * ConfigGetDouble(CONF_AVG_GPM) / 60;
            M5LCDShowWaterResult(total_time, total_galons);
            detected_start = 0;
        }
    }

    if (M5.BtnB.pressedFor(BUTTON_RESET_FACTORY_HOLD_TIME))
    {
        LOG_DBG("Button hold %dmsec", BUTTON_RESET_FACTORY_HOLD_TIME);
        LOG_DBG("%s", "===>> Reset factory <<===");
        BuzzerPlay(BUZZER_EVENT_BTN_HOLS5s);
        EraseData();
        delay(1000);
        esp_restart();
    }
#elif (BOARD_USING == BOARD_M5ATOM || BOARD_USING == BOARD_ESP32CAM || BOARD_USING == BOARD_M5TIMER_CAM)
    if (detected_start != 0)
    {
        bool btn_pressed = M5.Btn.wasPressed() ? 1 : 0;
        if (btn_pressed || is_water_killer_reset)
        {
            // Stash record
            record_t record;
            time_t now;

            total_time = (float)(mticks - detected_start) / 1000; // msec to sec
            total_galons = total_time * ConfigGetDouble(CONF_AVG_GPM) / 60;
            // Build record and push to spiff
            memset(&record, 0, sizeof(record));
            time(&now);
            record.id = (int)now;
            sprintf(record.tot_gal, "%.2f", total_galons);
            sprintf(record.tot_time, "%.2f", total_time);
            GetDate(record.date);
            GetTime(record.time);
            RecordAppend(&record);
            // Show water result
            LedShow(LED_EVENT_WATER_NONE);
            BuzzerPlay(BUZZER_EVENT_NONE);
            if (btn_pressed)
            {
                LOG_INFO("%s", "Btn pressed ====> Clear water detected data ====>");
            }

            if (is_water_killer_reset)
            {
                LOG_INFO("Water killer reset time %ld ====> Clear water detected data ====>",
                         mticks - detected_startx);
            }

            HandleIFTTT(IFTTT_ON);
            detected_start = 0;
            detected_startx = 0;
            is_water_killer_reset = false;
        }
    }

    if (M5.Btn.pressedFor(BUTTON_RESET_FACTORY_HOLD_TIME))
    {
        LOG_DBG("Button hold %dmsec", BUTTON_RESET_FACTORY_HOLD_TIME);
        LOG_DBG("%s", "===>> Reset factory <<===");
        EraseData();
        delay(1000);
        esp_restart();
    }
#endif /* (BOARD_USING == ?) */

    if ((next_on_ifttt != 0) && (mticks > next_on_ifttt))
    {
        next_on_ifttt = 0;
        HandleIFTTT(IFTTT_ON);
    }

    if (IsWaterDetected())
    {
        detected_last = mticks;
    }

    if (detected_start == 0 && IsWaterDetected() == 1)
    {
        M5LCDPrintf(M5LCD_LINE1, "Water detected!");
        LOG_INFO("%s", "=========> Water detected! <=========");
        LedShow(LED_EVENT_WATER_DETECTED);
        detected_start = mticks;
        if (detected_startx == 0)
        {
            detected_startx = detected_start;
            LOG_INFO("%s", "Startx count >>>");
        }
    }
    else if (detected_start != 0)
    {
        total_time = (float)(mticks - detected_start) / 1000; // msec to sec
        total_galons = total_time * ConfigGetDouble(CONF_AVG_GPM) / 60;

        // Not detect water
        if (IsWaterDetected() == 0)
        {
            record_t record;
            time_t now;

            detected_start = 0;
            // Build record and push to spiff
            memset(&record, 0, sizeof(record));
            time(&now);
            record.id = (int)now;
            sprintf(record.tot_gal, "%.2f", total_galons);
            sprintf(record.tot_time, "%.2f", total_time);
            GetDate(record.date);
            GetTime(record.time);
            RecordAppend(&record);
            // Show water result
            M5LCDShowWaterResult(total_time, total_galons);
            LedShow(LED_EVENT_WATER_NONE);
            BuzzerPlay(BUZZER_EVENT_NONE);
        }
        else if (mticks - detected_start > 1000)
        {
            int tcount = (mticks - detected_startx) / 1000;
            int water_killer_reset = ConfigGetInt(CONF_WATER_KILLER_TIME) +
                                     ConfigGetInt(CONF_WATER_KILLER_RESET_TIME);
            if (tcount >= water_killer_reset)
            {
                is_water_killer_reset = true;
                LOG_INFO("!!! Water killer reset [tcount %d, %d]", tcount, water_killer_reset);
            }
            else if (tcount >= ConfigGetInt(CONF_WATER_KILLER_TIME))
            {
                M5LCDPrintf(M5LCD_LINE2, "U r a Water killer :(");
                LedShow(LED_EVENT_WATER_KILLER);
                BuzzerPlay(BUZZER_EVENT_WATER_KILLER);
                HandleIFTTT(IFTTT_OFF);
                next_on_ifttt = mticks + ConfigGetInt(CONF_WATER_KILLER_RESET_TIME) * 1000;
            }
            // else if (tcount >= ConfigGetInt(CONF_WATER_ALERT4_TIME))
            // {
            //     LedShow(LED_EVENT_WATER_ALERT4);
            //     BuzzerPlay(BUZZER_EVENT_WATER_ALERT4);
            // }
            else if (tcount >= ConfigGetInt(CONF_WATER_ALERT3_TIME))
            {
                LedShow(LED_EVENT_WATER_ALERT3);
                BuzzerPlay(BUZZER_EVENT_WATER_ALERT3);
            }
            else if (tcount >= ConfigGetInt(CONF_WATER_ALERT2_TIME))
            {
                LedShow(LED_EVENT_WATER_ALERT2);
                BuzzerPlay(BUZZER_EVENT_WATER_ALERT2);
            }
            else if (tcount >= ConfigGetInt(CONF_WATER_ALERT1_TIME))
            {
                LedShow(LED_EVENT_WATER_ALERT1);
                BuzzerPlay(BUZZER_EVENT_WATER_ALERT1);
            }
        }
    }
    else
    {
    }

    if (detected_last != 0 && detected_startx != 0 &&
        ((mticks - detected_last) > ConfigGetInt(CONF_WATER_AVG_RESET_TIME) * 1000))
    {
        // Last see water detected in X sec before
        // Clear detected_startx
        detected_startx = 0;
        LOG_INFO("!!! Water avg reset [%ld %ld %d]", mticks, detected_last,
                 ConfigGetInt(CONF_WATER_AVG_RESET_TIME) * 1000);
    }

    if (ota_available)
    {
        ota_cmd_t *cmd = GetOTACmd();
        char ssid[64];
        char passwd[64];
        int count = 0;

        ota_available = false;
        LOG_DBG("Firmware URL %s, version %s", cmd->url, cmd->version);
        if (isNewerVersion(cmd->version))
        {
            memset(ssid, 0, sizeof(ssid));
            memset(passwd, 0, sizeof(passwd));
            PortalGetConfigWIFI(ssid, passwd);
            LOG_INFO("The loop run core %d, free heap %ld", xPortGetCoreID(), xPortGetFreeHeapSize());
            PortalExit();
            DetectionExit();
            vTaskDelay(pdMS_TO_TICKS(1000));
            LOG_INFO("The loop run core %d, free heap %ld", xPortGetCoreID(), xPortGetFreeHeapSize());
            HttpOTAHandle(cmd->url);
            vTaskDelay(pdMS_TO_TICKS(100));
            esp_restart();
        }
        else
        {
            LOG_INFO("%s", "Firmware NOT newer!!!");
        }
    }

    if (Serial.available() > 0)
    {
        int rchar = Serial.read();
        if (rchar != -1)
        {
            LOG_DBG("RX == %c", rchar);
            if (rchar == 'C')
            {
                ConfigShow();
            }
            else if (rchar == 'H')
            {
                LOG_DBG("Free memory size == %ld", xPortGetFreeHeapSize());
            }
            else if (rchar == 'R')
            {
                esp_restart();
            }
            else if (rchar == 'F')
            {
                LOG_DBG("%s", "===>> Reset factory <<===");
                EraseData();
                delay(1000);
                esp_restart();
            }
        }
    }

    vTaskDelay(pdMS_TO_TICKS(120));
}

void HandleCloudRequest()
{
    switch (cloud_state)
    {
    case CLOUD_STATE_GET_USERID:
        memset(user_email, 0, sizeof(user_email));
        if (!PortalGetConfigEmail(user_email))
        {
            break;
        }

        memset(user_id, 0, sizeof(user_id));
        if (ConfigGetStr(CONF_USER_ID, user_id) && strlen(user_id) > 0)
        {
            LOG_DBG("User ID from spiffs:: %s", user_id);
            cloud_state = CLOUD_STATE_GET_TOKEN;
            break;
        }

        if (HttpAPIGetUserId(user_email, user_id))
        {
            ConfigSet(CONF_USER_ID, user_id);
            next_request = REQUEST_NORMAL;
            cloud_state = CLOUD_STATE_GET_TOKEN;
        }
        break;

    case CLOUD_STATE_GET_TOKEN:
    {
        memset(user_token, 0, sizeof(user_token));
        if (ConfigGetStr(CONF_TOKEN, user_token) && strlen(user_token) > 0)
        {
            LOG_DBG("User Token from spiffs:: %s", user_token);
            cloud_state = CLOUD_STATE_CHECK_DEVICE_REGISTER;
            break;
        }

        char datetime[32];
        StaticJsonDocument<512> jsbody;

        memset(strbody, 0, sizeof(strbody));
        memset(datetime, 0, sizeof(datetime));
        GetDateTime(datetime);
        jsbody["Email"] = String(user_email);
        jsbody["Mac"] = WiFi.macAddress();
        jsbody["IPAddr"] = WiFi.localIP().toString();
        jsbody["SSID"] = WiFi.SSID();
        jsbody["ConnType"] = "wlan0";
        jsbody["Status"] = "token";
        jsbody["Date"] = String(datetime);
        jsbody["DeviceType"] = "3";
        jsbody["OS"] = "m5stickc";

        serializeJson(jsbody, strbody);

        if (HttpAPIGetToken(strbody, user_token))
        {
            ConfigSet(CONF_TOKEN, user_token);
            next_request = REQUEST_CHECK_DEVICE_REGISTER;
            cloud_state = CLOUD_STATE_CHECK_DEVICE_REGISTER;
        }
        break;
    }

    case CLOUD_STATE_CHECK_DEVICE_REGISTER:
    {
        if (ConfigGetInt(CONF_DEVICE_REGISTER))
        {
            cloud_state = CLOUD_STATE_SEND_DATA;
            device_register = true;
            LOG_INFO("%s", "Device registered");
            break;
        }

        if (!HttpAPIPollCommand(user_email, user_token, strbody))
        {
            break;
        }

        DynamicJsonDocument jsbody(4096);
        if (deserializeJson(jsbody, strbody))
        {
            break;
        }

        if (jsbody.containsKey("cmd"))
        {
            if (jsbody["cmd"] == "baddevice")
            {
                LOG_INFO("%s", "Device not register!!!");
            }
            else
            {
                LOG_INFO("%s", "Device just registered");
                device_register = true;
                ConfigSetInt(CONF_DEVICE_REGISTER, 1);
                next_request = REQUEST_UPDATE_DATA;
                cloud_state = CLOUD_STATE_SEND_DATA;
            }
        }
        break;
    }

    case CLOUD_STATE_SEND_DATA:
    {
        DynamicJsonDocument jsbody(4096);
        char *strrecord = (char *)pvPortMalloc(MAX_RECORD_ALL_LENGTH);

        if (strrecord == NULL)
        {
            LOG_ERR("%s", "No mem for strrecord");
            break;
        }

        memset(strrecord, 0, MAX_RECORD_ALL_LENGTH);
        memset(strbody, 0, sizeof(strbody));
        int position = RecordGetStr(strrecord);
        if (position)
        {
            LOG_DBG("=== %s", strrecord);
            DynamicJsonDocument jsrecord(MAX_RECORD_ALL_LENGTH);
            deserializeJson(jsrecord, strrecord, MAX_RECORD_ALL_LENGTH);

            jsbody["Email"] = String(user_email);
            jsbody["Tkn"] = String(user_token);
            jsbody["Status"] = "water";
            jsbody["logs"] = jsrecord;

            serializeJson(jsbody, strbody);
            if (HttpAPISendData(strbody))
            {
            }
            RecordSetPosition(position);
        }
        else
        {
            LOG_DBG("%s", "Nothing data to send");
        }
        next_request = REQUEST_UPDATE_DATA;
        vPortFree(strrecord);
        break;
    }

    default:
        break;
    }
}

void SendHeartBeat()
{
    DynamicJsonDocument jsbody(2048);
    char date[32];
    char time[32];
    int32_t dBm = WiFi.RSSI();

    if (!device_register)
    {
        return;
    }

    GetDate(date);
    GetTime(time);
    jsbody["Email"] = String(user_email);
    jsbody["Tkn"] = String(user_token);
    jsbody["Status"] = "health";
    jsbody["Uptime"] = String(millis());
    jsbody["Mem"] = String(ESP.getFreeHeap());
    jsbody["Swap"] = "0";
    jsbody["SSID"] = WiFi.SSID();
    jsbody["IP"] = WiFi.localIP().toString();
    jsbody["MAC"] = WiFi.macAddress();
    jsbody["ConnType"] = "wlan0";
#if BOARD_USING == BOARD_ESP32
    jsbody["PiModel"] = "esp32";
#elif BOARD_USING == BOARD_M5ATOM
    jsbody["PiModel"] = "atomu";
#endif
    jsbody["Date"] = String(date) + String(" ") + String(time);
    jsbody["Signal"] = (dBm == 31 ? String(F("N/A")) : String(dBm));

    serializeJson(jsbody, strbody);
    if (HttpAPISendData(strbody))
    {
    }
}

void HandleNewCommand()
{
    if (!device_register)
    {
        return;
    }

    if (!HttpAPIPollCommand(user_email, user_token, strbody))
    {
        return;
    }

    // Parse and handle new command
    if (!ConfigSetJsStr(strbody))
    {
        return;
    }

    char date[32];
    char time[32];

    GetDate(date);
    GetTime(time);

    command_id_t cmd = GetPendingCommand();
    if (cmd == COMMAND_GETLOG)
    {
        size_t index = 0;
        size_t remain = 2048;
        size_t rlength;

        memset(strbody, 0, sizeof(strbody));
        while (1)
        {
            int err = xqueue_get(llog_queue, (uint8_t *)&strbody[index], remain, &rlength);
            if (err != ESP_OK)
            {
                break;
            }

            index += rlength;
            remain -= rlength;
            strbody[index++] = '\n';
            remain--;
        }

        // Respond to cloud
        DynamicJsonDocument jsbody(2560);

        jsbody["Email"] = String(user_email);
        jsbody["Tkn"] = String(user_token);
        jsbody["Status"] = "logs";
        jsbody["sys_logs"] = String(strbody);
        jsbody["SSID"] = WiFi.SSID();
        jsbody["IP"] = WiFi.localIP().toString();
        jsbody["MAC"] = WiFi.macAddress();
        jsbody["ConnType"] = "wlan0";
        jsbody["Date"] = String(date) + String(" ") + String(time);

        memset(strbody, 0, sizeof(strbody));
        serializeJson(jsbody, strbody);
    }
    else
    {
        // Respond to cloud
        StaticJsonDocument<1024> jsbody;

        jsbody["Email"] = String(user_email);
        jsbody["Tkn"] = String(user_token);
        jsbody["Status"] = "done";
        jsbody["MopFlag"] = "0";
        jsbody["SSID"] = WiFi.SSID();
        jsbody["IP"] = WiFi.localIP().toString();
        jsbody["MAC"] = WiFi.macAddress();
        jsbody["ConnType"] = "wlan0";
        jsbody["Date"] = String(date) + String(" ") + String(time);

        memset(strbody, 0, sizeof(strbody));
        serializeJson(jsbody, strbody);
    }

    if (cmd == COMMAND_OTA)
    {
        ota_available = true;
    }

    if (HttpAPISendData(strbody))
    {
    }
}

static void HandlePollConfig()
{
    if (!device_register)
    {
        return;
    }

    if (!HttpAPIPollConfig(user_email, user_token, strbody))
    {
        return;
    }
}

static void HandleIFTTT(uint8_t state)
{
    char ifttt_url[256];

    if (ConfigGetInt(CONF_IFTTT_EN) == 0)
    {
        return;
    }

    // LOG_INFO("IFTTT current is off %d, new state %d", ifttt_off);
    if (state == IFTTT_ON)
    {
        if (ConfigGetStr(CONF_IFTTT_URL_ON, ifttt_url) && ifttt_off)
        {
            if (HttpIFTTTget(ifttt_url))
            {
                ifttt_off = false;
            }
        }
    }

    if (state == IFTTT_OFF)
    {
        if (ConfigGetStr(CONF_IFTTT_URL_OFF, ifttt_url) && !ifttt_off)
        {
            if (HttpIFTTTget(ifttt_url))
            {
                ifttt_off = true;
            }
        }
    }
}

void HandleReconnect(void)
{
    char ssid[64];
    char passwd[64];

    memset(ssid, 0, sizeof(ssid));
    memset(passwd, 0, sizeof(passwd));
    if (PortalGetConfigWIFI(ssid, passwd))
    {
        if (strlen(passwd) > 0 && strlen(ssid) > 0)
        {
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            WiFi.begin(ssid, passwd);
            LOG_INFO("Connect to AP %s", ssid);
        }
    }
    else
    {
    }
}