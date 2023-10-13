/*
 *  @file: Portal.cpp
 *
 *  Created on:
 */

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AppConfig.h"
#include "Log.h"
#include "Queue.h"
#include "WebServer.h"
#include "uri/UriBraces.h"
#include "uri/UriRegex.h"
#include "WiFi.h"
#include "TimeSync.h"
#include "Portal.h"
#include "Utils.h"

/******************************************************************************/
/*                              TYPES and DEFINITIONS                         */
/******************************************************************************/

enum
{
    UPDATE_CONNECTION_OK = 0,
    UPDATE_FAILED_ATTEMPT = 1,
    UPDATE_USER_DISCONNECT = 2,
    UPDATE_LOST_CONNECTION = 3,
    UPDATE_WAIT_CONNECTION = 4,
};

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

extern const uint8_t index_html_start[] asm("_binary_src_webpage_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_src_webpage_index_html_end");

extern const uint8_t logs_html_start[] asm("_binary_src_webpage_logs_html_start");
extern const uint8_t logs_html_end[] asm("_binary_src_webpage_logs_html_end");

extern const uint8_t network_js_start[] asm("_binary_src_webpage_network_js_start");
extern const uint8_t network_js_end[] asm("_binary_src_webpage_network_js_end");

extern const uint8_t style_css_start[] asm("_binary_src_webpage_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_src_webpage_style_css_end");

static WebServer Server(80);
static TaskHandle_t PortalTaskHandle = NULL;
static int reason = UPDATE_CONNECTION_OK;
static char ssid[64];
static char passwd[64];
static char email[128];
static char timezone[48];

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void PortalFileIndex()
{
    Server.send(200, "text/html", (const char *)index_html_start);
}

void PortalFileStyle()
{
    Server.send(200, "text/css", (const char *)style_css_start);
}

void PortalFileNetwork()
{
    Server.send(200, "text/javascript", (const char *)network_js_start);
}

void PortalFileLogs()
{
    Server.send(200, "text/html", (const char *)logs_html_start);
}

void PortalWaterData()
{
    String response = "{";
    char tmp[256];
    char strjs[256];
    record_t *list_record = (record_t *)malloc(16 * sizeof(record_t));

    if (ConfigGetInt(CONF_DEVICE_REGISTER))
    {
        response += String("\"register\":true,");

        int numb_records = 0;
        int index = 0;
        memset(list_record, 0, 16 * sizeof(record_t));
        numb_records = RecordGet(list_record, 16);
        response += String("\"record\":[");
        for (int i = 0; i < numb_records; i++)
        {
            sprintf(tmp, "{\"date\":\"%s\",\"time\":\"%s\",\"ttime\":\"%s\",\"tgal\":\"%s\"}",
                    list_record[i].date,
                    list_record[i].time,
                    list_record[i].tot_time,
                    list_record[i].tot_gal);
            response += String(tmp);
            if (i < numb_records - 1)
            {
                response += String(",");
            }
        }
        response += String("],");
    }
    else
    {
        response += String("\"register\":false,");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        response += String("\"WiFi\":true");
    }
    else
    {
        response += String("\"WiFi\":false");
    }

    memset(tmp, 0, sizeof(tmp));
    if (ConfigGetStr(CONF_TOKEN, tmp) && strlen(tmp) > 0)
    {
        sprintf(strjs, ",\"utoken\":\"%s\"", tmp);
        response += String(strjs);

        PortalGetConfigEmail(tmp);
        sprintf(strjs, ",\"uemail\":\"%s\"", tmp);
        response += String(strjs);

        ConfigGetStr(CONF_USER_ID, tmp);
        sprintf(strjs, ",\"uid\":\"%s\"", tmp);
        response += String(strjs);

        ConfigGetStr(CONF_BASE_URI, tmp);
        sprintf(strjs, ",\"url\":\"%s\"", tmp);
        response += String(strjs);
    }

    response += "}";

    Server.send(200, "application/json", response);

    free(list_record);
}

void getLogs()
{
#if ENABLE_WEB_LOG
    esp_err_t err;
    char log_line[LOG_LINE_MAX];
    size_t log_length;
    String logs = "";

    while (1)
    {
        memset(log_line, 0, sizeof(log_line));
        err = xqueue_get(llog_queue, (uint8_t *)log_line, sizeof(log_line), &log_length);
        if (err != ESP_OK)
        {
            break;
        }
        logs += String(log_line) + "\n";
    }

    Server.send(200, "text/plain", logs);
#else
    Server.send(200, "text/plain", "Log disable!!!");
#endif
}

void scanAP()
{
    String ap_list = "[";
    char oneap_str[128];

    int n = WiFi.scanComplete();
    if (n > 0)
    {
        n = (n > 24) ? 24 : n;
        for (int i = 0; i < n; ++i)
        {
            sprintf(oneap_str, "{\"ssid\":\"%s\",\"chan\":%d,\"rssi\":%d,\"auth\":%d}",
                    WiFi.SSID(i).c_str(),
                    WiFi.channel(i),
                    WiFi.RSSI(i),
                    WiFi.encryptionType(i));
            ap_list += String(oneap_str);
            if (i < n - 1)
            {
                ap_list += ",";
            }
        }

        n = WiFi.scanNetworks(true);
        LOG_DBG("WiFi.scanNetworks %s", n == WIFI_SCAN_RUNNING ? "start" : "error");
    }
    else if (n == WIFI_SCAN_RUNNING)
    {
        LOG_DBG("%s", "Wait...");
    }
    else
    {
        n = WiFi.scanNetworks(true);
        LOG_DBG("WiFi.scanNetworks %s", n == WIFI_SCAN_RUNNING ? "start" : "error");
    }

    ap_list += "]";
    Server.send(200, "application/json", ap_list);
}

void deviceReboot()
{
    Server.send(200);
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_restart();
}

void disconnectAP()
{
    Server.send(200);
    vTaskDelay(pdMS_TO_TICKS(100));
    WiFi.disconnect();
}

void statusAP()
{
    char ap_status[128];

    if (WiFi.status() != WL_CONNECTED)
    {
        sprintf(ap_status, "{\"ssid\":\"%s\",\"urc\":%d}", ssid, reason);
    }
    else {
        sprintf(ap_status, "{\"ssid\":\"%s\",\"ip\":\"%s\",\"netmask\":\"%s\",\"gw\":\"%s\",\"urc\":%d}",
                WiFi.SSID().c_str(),
                WiFi.localIP().toString().c_str(),
                WiFi.subnetMask().toString().c_str(),
                WiFi.gatewayIP().toString().c_str(),
                reason);
    }

    Server.send(200, "application/json", String(ap_status));
}

uint8_t hexToBin(char c, uint8_t radix)
{
    if (c <= 0xF)
    {
        return c;
    }
    else if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        if (radix == 10)
            return 0xFF;

        return c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        if (radix == 10)
            return 0xFF;

        return c - 'A' + 10;
    }
    else
    {
        return 0xFF;
    }
}

static void hexStringToString(const char *s, String &rs)
{
    char hex[3] = {0, 0, 0};
    int n;
    char c[2];

    for (int i = 0; i < strlen(s); i += 2)
    {
        n = hexToBin(s[i], 16) << 4;
        n += hexToBin(s[i + 1], 16);
        sprintf(c, "%c", n);

        rs += String(c);
    }
}

void connectAP()
{
    String strs[4];
    int count = 0;
    String tcommand = Server.pathArg(0);
    String command = "";

    reason = UPDATE_WAIT_CONNECTION;
    hexStringToString(tcommand.c_str(), command);
    memset(ssid, 0, sizeof(ssid));
    memset(passwd, 0, sizeof(passwd));
    memset(email, 0, sizeof(email));
    memset(timezone, 0, sizeof(timezone));
    while (command.length() > 0 && count < 4)
    {
        int index = command.indexOf('&');
        if (index == -1)
        {
            strs[count++] = command;
            break;
        }
        else
        {
            strs[count++] = command.substring(0, index);
            command = command.substring(index + 1);
        }
    }

    strs[0].replace("ssid=", "");
    strs[1].replace("pwd=", "");
    strs[2].replace("email=", "");
    strs[3].replace("tz=", "");

    snprintf(ssid, sizeof(ssid), "%s", strs[0].c_str());
    snprintf(passwd, sizeof(passwd), "%s", strs[1].c_str());
    snprintf(email, sizeof(email), "%s", strs[2].c_str());
    snprintf(timezone, sizeof(timezone), "%s", strs[3].c_str());

    LOG_INFO("Parse ssid %s, pwd %s, email %s, tz %s", ssid, passwd, email, timezone);
    if (strlen(passwd) < 8 || strlen(email) < 3)
    {
        Server.send(400);
    }
    else
    {
        Server.send(200);

        int tmout = 0;

        WiFi.disconnect();
        WiFi.begin(ssid, passwd);
        while (WiFi.status() != WL_CONNECTED)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            LOG_DBG("%d. Wait ...", tmout);
            if (tmout++ > 8)
            {
                reason = UPDATE_FAILED_ATTEMPT;
                return;
            }
        }
        LOG_DBG("%s", ">> Backup wifi credentials");
        reason = UPDATE_CONNECTION_OK;
        ConfigSet(CONF_SSID, ssid);
        ConfigSet(CONF_PASSWD, passwd);
        ConfigSet(CONF_EMAIL, email);
        ConfigSet(CONF_TZ, timezone);

        const timezone_t *tz = PortalGetConfigTZ();
        if (tz != NULL)
        {
            TimeSyncSetTz(tz->tz);
        }
    }
}

void PortalHandle(void *arg)
{
    String ext_ssid = WiFi.macAddress();
    ext_ssid.replace(":", "");
    String softap_ssid = String(SOFT_AP_SSID) + String("_") + ext_ssid.substring(8);
    String host_name = String(HOST_NAME) + String("-") + ext_ssid.substring(8);

    WiFi.setHostname(host_name.c_str());
    if (PortalGetConfigWIFI(ssid, passwd) && strlen(passwd) >= 8 && strlen(ssid) > 0)
    {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        WiFi.begin(ssid, passwd);
        LOG_INFO("Connect to AP %s", ssid);
    }
    else
    {
        IPAddress ip(SOFT_AP_IP_ADDR);
        IPAddress gw(SOFT_AP_GATEWAY_IP_ADDR);
        IPAddress sub(255, 255, 255, 0);

        WiFi.mode(WIFI_AP_STA);
        WiFi.disconnect();
        WiFi.softAPConfig(ip, gw, sub);

        /* Find best channel */
        int n = WiFi.scanNetworks();
        int rssi_max[15];
        int rssi_min = 0;
        for (int i = 0; i < 15; i++)
        {
            rssi_max[i] = -255;
        }
        for (int i = 0; i < n; ++i)
        {
            if (rssi_max[WiFi.channel(i)] < WiFi.RSSI(i))
            {
                rssi_max[WiFi.channel(i)] = WiFi.RSSI(i);
            }
        }
        n = 13;
        for (int i = 1; i <= 13; i++)
        {
            if (rssi_min < rssi_max[i])
            {
                rssi_min = rssi_max[i];
                n = i;
            }
        }
        LOG_INFO("Start softAP %s", softap_ssid.c_str());
        WiFi.softAP(softap_ssid.c_str(), NULL, n);
    }

    Server.begin();

    while (1)
    {
        Server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

void PortalInit(void)
{
    Server.on("/", PortalFileIndex);
    Server.on("/waterdata", PortalWaterData);
    Server.on("/logs", PortalFileLogs);
    Server.on("/getlogs", getLogs);
    Server.on("/admin", PortalFileLogs);
    Server.on("/style.css", PortalFileStyle);
    Server.on("/network.js", PortalFileNetwork);
    Server.on("/ap.json", scanAP);
    Server.on("/status.json", statusAP);
    Server.on(UriBraces("/connect/{}"), connectAP);
    Server.on("/reboot", deviceReboot);

    xTaskCreatePinnedToCore(PortalHandle,      /* Task function. */
                            "Portal",          /* name of task. */
                            5120,              /* Stack size of task */
                            NULL,              /* parameter of the task */
                            1,                 /* priority of the task */
                            &PortalTaskHandle, /* Task handle to keep track of created task */
                            1);                /* pin task to core 0 */
}

void PortalExit(void)
{
    if (PortalTaskHandle != NULL)
    {
        vTaskDelete(PortalTaskHandle);
        PortalTaskHandle = NULL;
    }
}

bool PortalExistConfig()
{
    char passwd[64];

    memset(passwd, 0, sizeof(passwd));
    if (ConfigGetStr(CONF_PASSWD, passwd))
    {
        if (strlen(passwd) >= 8)
        {
            return true;
        }
    }

    return false;
}

const timezone_t *PortalGetConfigTZ()
{
    char tzone[64];
    const timezone_t *tz;

    if (ConfigGetStr(CONF_TZ, tzone))
    {
        if (strlen(tzone) > 0)
        {
            tz = GetListTZ();
            for (int i = 0; tz[i].zone != NULL; i++)
            {
                if (strcmp(tzone, tz[i].zone) == 0)
                {
                    return &tz[i];
                }
            }
        }
    }

    return NULL;
}

bool PortalGetConfigEmail(char *email)
{

    if (ConfigGetStr(CONF_EMAIL, email))
    {
        return true;
    }

    return false;
}

bool PortalGetConfigWIFI(char *ssid, char *passwd)
{
    if (ConfigGetStr(CONF_SSID, ssid) &&
        ConfigGetStr(CONF_PASSWD, passwd))
    {
        return true;
    }

    return false;
}
