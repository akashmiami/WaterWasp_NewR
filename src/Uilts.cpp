/*
 *  @file: Utils.cpp
 *
 *  Created on:
 */

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <vector>
#include "nvs_flash.h"
#include "nvs.h"
#include "FS.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"
#include "AppConfig.h"
#include "Log.h"
#include "Utils.h"
/******************************************************************************/
/*                              TYPES and DEFINITIONS                         */
/******************************************************************************/
#define MAX_CACHE_RECORD                           10
#define TEMPLATE_RECORD                            "{\"id\":%d,\"ddat\":\"%s\",\"dtim\":\"%s\",\"ttim\":\"%s\",\"tgal\":\"%s\"}"

typedef struct __field_map {
    int id;
    vtype_t type;
    const char *name; 
} field_map_t;

typedef std::vector<record_t> vrecord_t;
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static const field_map_t list_config[] = {
    { CONF_SSID,                    VTYPE_STR,    "ssid"      },
    { CONF_PASSWD,                  VTYPE_STR,    "pwd"       },
    { CONF_EMAIL,                   VTYPE_STR,    "email"     },
    { CONF_TZ,                      VTYPE_STR,    "tz"        },
    { CONF_DEVICE_REGISTER,         VTYPE_INT,    "register"  },
    { CONF_USER_ID,                 VTYPE_STR,    "user_id"   },
    { CONF_DEVICE_ID,               VTYPE_STR,    "device_id" },
    { CONF_TOKEN,                   VTYPE_STR,    "token"     },
    { CONF_BASE_URI,                VTYPE_STR,    "base_uri"  },
    { CONF_AVG_GPM,                 VTYPE_DOUBE,  "agpm"      },
    { CONF_SCORE_LIMIT,             VTYPE_INT,    "slim"      },
    { CONF_DET_THRES,               VTYPE_DOUBE,  "dthr"      },
    { CONF_DET_TCNT,                VTYPE_INT,    "dtct"      },
    { CONF_DET_TABL_SIZE,           VTYPE_INT,    "dert"      },
    { CONF_DETCNT_MAX,              VTYPE_INT,    "dmax"      },
    { CONF_DETCNT_MIN,              VTYPE_INT,    "dmin"      },
    { CONF_MAX_GALLONS_WARNING,     VTYPE_DOUBE,  "mgaw"      },
    { CONF_BEEP_WARNING_INTERVAL,   VTYPE_DOUBE,  "bint"      },
    { CONF_MODEL,                   VTYPE_INT,    "cmodel"    },
    { CONF_MIC_VOL_GAIN,            VTYPE_INT,    "mivg"      },
    { CONF_BUZZ_EN,                 VTYPE_INT,    "enbuz"     },
    { CONF_FILTER_EN,               VTYPE_INT,    "fltr"      },
    { CONF_WATER_ALERT1_TIME,       VTYPE_INT,    "wal1"      },
    { CONF_WATER_ALERT2_TIME,       VTYPE_INT,    "wal2"      },
    { CONF_WATER_ALERT3_TIME,       VTYPE_INT,    "wal3"      },
    { CONF_WATER_ALERT4_TIME,       VTYPE_INT,    "wal4"      },
    { CONF_WATER_KILLER_TIME,       VTYPE_INT,    "walk"      },
    { CONF_WATER_KILLER_RESET_TIME, VTYPE_INT,    "wakr"      },
    { CONF_WATER_AVG_RESET_TIME,    VTYPE_INT,    "awrt"      },
    { CONF_IFTTT_URL_ON,            VTYPE_STR,    "onift"     },
    { CONF_IFTTT_URL_OFF,           VTYPE_STR,    "ofift"     },
    { CONF_IFTTT_EN,                VTYPE_INT,    "enift"     },
    { 0,                            VTYPE_NONE,   NULL        }
};

static const timezone_t list_tz[] = {
    { "Europe_London",        "GMT0BST,M3.5.0/1,M10.5.0" },
    { "Europe_Berlin",        "CET-1CEST,M3.5.0,M10.5.0/3" },
    { "Europe_Helsinki",      "EET-2EEST,M3.5.0/3,M10.5.0/4" },
    { "Europe_Moscow",        "MSK-3" },
    { "Asia_Dubai",           "<+04>-4" },
    { "Asia_Karachi",         "PKT-5" },
    { "Asia_Dhaka",           "<+06>-6" },
    { "Asia_Jakarta",         "WIB-7" },
    { "Asia_Manila",          "PST-8" },
    { "Asia_Tokyo",           "JST-9" },
    { "Asia_Ho_Chi_Minh",     "<+07>-7"},
    { "Australia_Brisbane",   "AEST-10" },
    { "Pacific_Noumea",       "<+11>-11" },
    { "Pacific_Auckland",     "NZST-12NZDT,M9.5.0,M4.1.0/3" },
    { "Atlantic_Azores",      "<-01>1<+00>,M3.5.0/0,M10.5.0/1" },
    { "America_Noronha",      "<-02>2" },
    { "America_Araguaina",    "<-03>3" },
    { "America_Blanc-Sablon", "	AST4" },
    { "America_New_York",     "EST5EDT,M3.2.0,M11.1.0" },
    { "America_Chicago",      "CST6CDT,M3.2.0,M11.1.0" },
    { "America_Denver",       "MST7MDT,M3.2.0,M11.1.0" },
    { "America_Los_Angeles",  "PST8PDT,M3.2.0,M11.1.0" },
    { "America_Anchorage",    "AKST9AKDT,M3.2.0,M11.1.0" },
    { "Pacific_Honolulu",     "HST10" },
    { "Asia_Ho_Chi_Minh",     "<+07>-7" },
    { NULL, NULL}
};

static const char *record_file = FILE_PATH_RECORD;
static const char *config_file = FILE_PATH_CONFIG;
static const char *position_file = FILE_PATH_POSITION;
static const char *gallons_file = FILE_PATH_TOTAL_GALLONS;

vrecord_t vrecord;
int numb_records = 0;
int position = 0;
double total_gallons = 0.0f;
bool ota_available = false;
command_id_t pending_cmd = COMMAND_NONE;
ota_cmd_t ota_cmd;
StaticJsonDocument<MAX_CONFIG_LENGTH> jsconfig;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void ListAllFiles() {
    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return;
    }
  
    int count = 0;
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
        count++;
        LOG_DBG("%02d. File: %s", count, file.name());
        file = root.openNextFile();
    }

    if (!count) {
        LOG_INFO("%s", "Spiffs no file found!!");
    }
}

void RecordInit() {
    char line[MAX_RECORD_ITEM_LENGTH];
    StaticJsonDocument<MAX_RECORD_ITEM_LENGTH> jsrecord;
    record_t record;

    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open(record_file);
    if (!file) {
        LOG_ERR("%s", "Failed to open record file for reading");
        return;
    }

    while (file.available()) {
        memset(line, 0, sizeof(line));
        file.readBytesUntil('\n', line, sizeof(line));
        if (!deserializeJson(jsrecord, line)) {
            record.id = jsrecord["id"].as<int>();
            strcpy(record.date, jsrecord["ddat"].as<const char*>());
            strcpy(record.time, jsrecord["dtim"].as<const char*>());
            strcpy(record.tot_time, jsrecord["ttim"].as<const char*>());
            strcpy(record.tot_gal, jsrecord["tgal"].as<const char*>());
            numb_records++;
            vrecord.insert(vrecord.begin(), record);
            if (vrecord.size() > MAX_CACHE_RECORD) {
                vrecord.pop_back();
            }
        }
    }

    file.close();

    // Reload position log
    File psfile = SPIFFS.open(position_file);
    if (!psfile) {
        LOG_ERR("%s", "Failed to open position file for reading");
        return;
    }

    if (psfile.available()) {
        memset(line, 0, sizeof(line));
        psfile.readBytes(line, sizeof(line));
        position = atoi(line);
        LOG_INFO("Record position %d", position);
    }
    psfile.close();
  
    // Reload gallons total
    File glfile = SPIFFS.open(gallons_file);
    if (!glfile) {
        LOG_ERR("%s", "Failed to open gallons file for reading");
        return;
    }

    if (glfile.available()) {
        memset(line, 0, sizeof(line));
        glfile.readBytes(line, sizeof(line));
        total_gallons = atof(line);
        LOG_INFO("Total gallons %.2f", total_gallons);
    }
    glfile.close();
}

void BackupTotalGallons(double tgal) {
    char sp[16];

    File file = SPIFFS.open(gallons_file, FILE_WRITE);
    if (!file) {
        LOG_DBG("%s", "Failed to open gallons file for writing");
        return;
    }

    sprintf(sp, "%.2f", tgal);
    if (!file.print(sp)){
        LOG_ERR("%s", "Failed to backup gallons");
    }

    file.close();
}

/**
 * @fn     RecordAppend
 * @brief  append the record to a file
 */
void RecordAppend(record_t *record) {
    char strrecord[MAX_RECORD_ITEM_LENGTH];
    record_t rc;

    memcpy(&rc, record, sizeof(rc));
    vrecord.insert(vrecord.begin(), rc);
    if (vrecord.size() > MAX_CACHE_RECORD) {
        vrecord.pop_back();
    }

    // Increase total gallons
    total_gallons += atof(record->tot_gal);
    LOG_DBG("Total gallons %.2f", total_gallons);
    BackupTotalGallons(total_gallons);

    if (numb_records > MAX_RECORDS_STORE) {
        RecordDeleteAll();
        numb_records = 0;
        File file = SPIFFS.open(record_file, FILE_APPEND);
        if (!file || file.isDirectory()) {
            LOG_ERR("Failed to open file for writing %s", record_file);
        }

        for (auto it = vrecord.rbegin(); it != vrecord.rend(); ++it) {
            memset(strrecord, 0, sizeof(strrecord));
            sprintf(strrecord, TEMPLATE_RECORD "\n", it->id, it->date, it->time, it->tot_time, it->tot_gal);
            numb_records++;
            if (!file.print(strrecord)){
                LOG_ERR("%s", "Failed to append record");
            }

            if (numb_records == MAX_CACHE_RECORD) {
                LOG_DBG("Append ==== %s", strrecord);
            }
        }

        file.close();
    }
    else {
        File file = SPIFFS.open(record_file, FILE_APPEND);
        if (!file || file.isDirectory()) {
            LOG_ERR("Failed to open file for writing %s", record_file);
        }

        memset(strrecord, 0, sizeof(strrecord));
        sprintf(strrecord, TEMPLATE_RECORD "\n", rc.id, rc.date, rc.time, rc.tot_time, rc.tot_gal);
        numb_records++;
        if (!file.print(strrecord)){
            LOG_ERR("%s", "Failed to append record");
        }

        LOG_DBG("Append ==== %s", strrecord);
        file.close();
    }
}

void RecordDeleteAll() {
    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return;
    }

    if (!SPIFFS.remove(record_file)) {
        LOG_ERR("%s", "Failed to delete all record");
    }
}

int RecordGetStr(char *strrecord) {
    int count = 0;
    int last_position = 0;
    DynamicJsonDocument jsall(MAX_RECORD_ALL_LENGTH);
    char strtmp[MAX_RECORD_ALL_LENGTH];

    for (auto it = vrecord.rbegin(); it != vrecord.rend(); ++it) {
        if (it->id <= position) {
            continue;
        }
        last_position = it->id;
        DynamicJsonDocument jsrecord(MAX_RECORD_ITEM_LENGTH);
        jsrecord["id"] = it->id;
        jsrecord["ddat"] = it->date;
        jsrecord["dtim"] = it->time;
        jsrecord["ttim"] = it->tot_time;
        jsrecord["tgal"] = it->tot_gal;
        jsall[count] = jsrecord;
        count++;
    }

    serializeJson(jsall, strtmp);
    strcpy(strrecord, strtmp);
 
    return last_position;
}

void RecordSetPosition(int p) {
    char sp[12];
    position = p;

    File file = SPIFFS.open(position_file, FILE_WRITE);
    if (!file) {
        LOG_DBG("%s", "Failed to open position file for writing");
        return;
    }

    sprintf(sp, "%d", position);
    if (!file.print(sp)){
        LOG_ERR("%s", "Failed to backup position");
    }

    file.close();
}

int RecordGet(record_t *record, int maxrecord) {
    int count = 0;

    for (auto it = vrecord.begin(); it != vrecord.end(); ++it) {
       record[count].id = it->id;
       strcpy(record[count].date, it->date);
       strcpy(record[count].time, it->time);
       strcpy(record[count].tot_time, it->tot_time);
       strcpy(record[count].tot_gal, it->tot_gal);
       count++;
       if (count >= maxrecord) {
           break;
       }
    }

    return count;
}

bool RecordGetStrById(int id, char *strrecord) {
    char line[MAX_RECORD_ITEM_LENGTH];
    StaticJsonDocument<MAX_RECORD_ITEM_LENGTH> jsitem;
    char strtmp[MAX_RECORD_ALL_LENGTH];
    bool found = false;

    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return false;
    }

    File file = SPIFFS.open(record_file);
    if (!file) {
        LOG_ERR("%s", "Failed to open file for reading");
        return false;
    }

    while (file.available()) {
        memset(line, 0, sizeof(line));
        file.readBytesUntil('\n', line, sizeof(line));
        if (!deserializeJson(jsitem, line)) {
            if (jsitem["id"] == id) {
                serializeJson(jsitem, strtmp);
                strcpy(strrecord, strtmp);
                found = true;
                break;
            }
        }
    }

    file.close();
    
    return found;
}

static const char *getNameById(const field_map_t *list, int id) {
    for (int i = 0; list[i].name != NULL; i++) {
        if (id == list[i].id) {
            return list[i].name;
        }
    }

    return NULL;
}

void ConfigInit(void) {
    char strtmp[MAX_CONFIG_LENGTH];
    bool write_default = false;

    ListAllFiles();
    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open(config_file);
    if (!file) {
        LOG_ERR("%s", "Failed to open file for reading");
        return;
    }
    
    memset(strtmp, 0, sizeof(strtmp));
    if (file.available()) {
        DeserializationError error = deserializeJson(jsconfig, file);
        if (error) {
            write_default = true;
        }
    }
    else {
        write_default = true;
    }

    if (write_default) {
        LOG_INFO("%s", "Write default config");
        jsconfig[getNameById(list_config, CONF_SSID)]                    = "";
        jsconfig[getNameById(list_config, CONF_PASSWD)]                  = "";
        jsconfig[getNameById(list_config, CONF_EMAIL)]                   = "";
        jsconfig[getNameById(list_config, CONF_TZ)]                      = "";
        jsconfig[getNameById(list_config, CONF_DEVICE_REGISTER)]         = 0;
        jsconfig[getNameById(list_config, CONF_BASE_URI)]                = CLOUD_BASE_URI;
        jsconfig[getNameById(list_config, CONF_TOKEN)]                   = "";
        jsconfig[getNameById(list_config, CONF_USER_ID)]                 = "";
        jsconfig[getNameById(list_config, CONF_DEVICE_ID)]               = "";
        jsconfig[getNameById(list_config, CONF_AVG_GPM)]                 = DEF_CONF_AVG_GPM;
        jsconfig[getNameById(list_config, CONF_SCORE_LIMIT)]             = DEF_CONF_SCORE_LIMIT;
        jsconfig[getNameById(list_config, CONF_DET_THRES)]               = DEF_CONF_DET_THRES;
        jsconfig[getNameById(list_config, CONF_DET_TCNT)]                = DEF_CONF_DET_TCNT;
        jsconfig[getNameById(list_config, CONF_DET_TABL_SIZE)]           = DEF_CONF_DET_TABL_SIZE;
        jsconfig[getNameById(list_config, CONF_DETCNT_MAX)]              = DEF_CONF_DETCNT_MAX;
        jsconfig[getNameById(list_config, CONF_DETCNT_MIN)]              = DEF_CONF_DETCNT_MIN;
        jsconfig[getNameById(list_config, CONF_MAX_GALLONS_WARNING)]     = DEF_CONF_MAX_GALLONS_WARNING;
        jsconfig[getNameById(list_config, CONF_BEEP_WARNING_INTERVAL)]   = DEF_CONF_BEEP_WARNING_INTERVAL;
        jsconfig[getNameById(list_config, CONF_MODEL)]                   = DEF_CONF_DET_MODEL;
        jsconfig[getNameById(list_config, CONF_MIC_VOL_GAIN)]            = DEF_CONF_MIC_VOL_GAIN;
        jsconfig[getNameById(list_config, CONF_BUZZ_EN)]                 = DEF_CONF_BUZZ_ENABLE;
        jsconfig[getNameById(list_config, CONF_FILTER_EN)]               = DEF_CONF_FILTER_ENABLE;
        jsconfig[getNameById(list_config, CONF_WATER_ALERT1_TIME)]       = DEF_CONF_WATER_ALERT1_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_ALERT2_TIME)]       = DEF_CONF_WATER_ALERT2_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_ALERT3_TIME)]       = DEF_CONF_WATER_ALERT3_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_ALERT4_TIME)]       = DEF_CONF_WATER_ALERT4_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_KILLER_TIME)]       = DEF_CONF_WATER_KILLER_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_KILLER_RESET_TIME)] = DEF_CONF_WATER_KILLER_RESET_TIME;
        jsconfig[getNameById(list_config, CONF_WATER_AVG_RESET_TIME)]    = DEF_CONF_WATER_AVG_RESET_TIME;
        jsconfig[getNameById(list_config, CONF_IFTTT_EN)]                = 0;
        jsconfig[getNameById(list_config, CONF_IFTTT_URL_ON)]            = "";
        jsconfig[getNameById(list_config, CONF_IFTTT_URL_OFF)]           = "";
    }

    file.close();
    serializeJson(jsconfig, strtmp);
    LOG_INFO("Load config ==== %s", strtmp);
}

void ConfigShow() {
    char *strtmp = (char *)pvPortMalloc(MAX_CONFIG_LENGTH);
    if (strcmp == NULL) {
        LOG_ERR("%s", "No mem for config show");
        return;
    }

    serializeJson(jsconfig, strtmp, MAX_CONFIG_LENGTH);
    LOG_INFO("==== Config ==== %s", strtmp);
    vPortFree(strtmp);  
}

void ConfigBackup() {
    File file = SPIFFS.open(config_file, FILE_WRITE);
    if (!file) {
        LOG_DBG("%s", "Failed to open config file for writing");
        return ;
    }
    serializeJson(jsconfig, file);
    file.close();
}

bool ConfigSet(config_id_t id, char *value) {
    char strtmp[MAX_CONFIG_LENGTH];
    const char *item_name = getNameById(list_config, id);

    if (!item_name) {
        return false;
    }

    jsconfig[item_name] = value;
    ConfigBackup();

    return true;
}

bool ConfigSetInt(config_id_t id, int value) {
    const field_map_t *config_info = NULL;

    if (id >= NUMBER_OF_CONFIG) {
        return false;
    }

    config_info = &list_config[id];
    if (config_info->type != VTYPE_INT) {
        return false;
    }

    jsconfig[config_info->name] = value;
    ConfigBackup();

    return true;
}

bool ConfigSetDouble(config_id_t id, double value) {
    const field_map_t *config_info = NULL;

    if (id >= NUMBER_OF_CONFIG) {
        return false;
    }

    config_info = &list_config[id];
    if (config_info->type != VTYPE_DOUBE) {
        return false;
    }

    jsconfig[config_info->name] = value;
    ConfigBackup();

    return true;
}

bool ConfigSetJsStr(char *value) {
    bool is_set = false;
    DynamicJsonDocument jstmp(MAX_CONFIG_LENGTH);

    DeserializationError error = deserializeJson(jstmp, value);
    if (error) {
        LOG_ERR("%s", "Config error format!");
        return false;
    }

    if (jstmp.containsKey("cmd") && strcmp(jstmp["cmd"].as<char *>(), "updatewaterwasp") == 0) {
        LOG_INFO("%s", "==>> Command OTA");
        pending_cmd = COMMAND_OTA;
        memset(&ota_cmd, 0, sizeof(ota_cmd_t));
        if (jstmp.containsKey("download_address")) {
            snprintf(ota_cmd.url, sizeof(ota_cmd.url), "%s/%s", jstmp["download_address"].as<char *>(), FIRMWARE_FILE_NAME);
        }

        if (jstmp.containsKey("version")) {
            snprintf(ota_cmd.version, sizeof(ota_cmd.version), "%s", jstmp["version"].as<char *>());
        }
    }

    if (jstmp.containsKey("cmd") && strcmp(jstmp["cmd"].as<char *>(), "logfiles") == 0) {
        pending_cmd = COMMAND_GETLOG;
    }
    
    for (int i = 0; list_config[i].name != NULL; i++) {
        if (!jstmp.containsKey(list_config[i].name)) {
            continue;
        }

        switch (list_config[i].type) {
        case VTYPE_INT: {
            int ival = 0;
            ival = atoi(jstmp[list_config[i].name].as<char *>());
            LOG_INFO(">> Set %s = %d", list_config[i].name, ival);
            jsconfig[list_config[i].name] = ival;
            is_set = true;
            break;
        }

        case VTYPE_DOUBE: {
            double dval = 0.0;
            dval = atof(jstmp[list_config[i].name].as<char *>());
            LOG_INFO(">> Set %s = %.2f", list_config[i].name, dval);
            jsconfig[list_config[i].name] = dval;
            is_set = true;
            break;
        }

        case VTYPE_STR: {
            jsconfig[list_config[i].name] = String(jstmp[list_config[i].name].as<char *>());
            LOG_INFO(">> Set %s = %s", list_config[i].name, jstmp[list_config[i].name].as<char *>());
            is_set = true;
            break;
        }
        
        default:
            LOG_INFO(">> Unhandle set %s", list_config[i].name);
            break;
        }
    }
 
    if (is_set) {
        ConfigBackup();
    }

    return is_set | (pending_cmd != COMMAND_NONE);
}

command_id_t GetPendingCommand(void)
{
    command_id_t cmd = pending_cmd;

    pending_cmd = COMMAND_NONE;

    return cmd;
}

ota_cmd_t *GetOTACmd(void) {
    return &ota_cmd;
}

bool isNewerVersion(const char *version) {
    float ver = atof(version);
    float current = atof(FIRMWARE_VERSION);

    if (ver > current) {
        return true;
    }

    return false;
}

int ConfigGetInt(config_id_t id) {
    int val = 0;
    const field_map_t *config_info = NULL;

    if (id >= NUMBER_OF_CONFIG) {
        return val;
    }

    config_info = &list_config[id];
    if (!jsconfig.containsKey(config_info->name)) {
        return val;
    }

    if (config_info->type == VTYPE_INT) {
        return jsconfig[config_info->name].as<int>();
    }

    return val;
}

double ConfigGetDouble(config_id_t id) {
    double val = 0.0;
    const field_map_t *config_info = NULL;

    if (id >= NUMBER_OF_CONFIG) {
        return val;
    }

    config_info = &list_config[id];
    if (!jsconfig.containsKey(config_info->name)) {
        return val;
    }

    if (config_info->type == VTYPE_DOUBE) {
        return jsconfig[config_info->name].as<double>();
    }

    return val;
}

bool ConfigGetStr(config_id_t id, char *value) {
    const char *item_name = getNameById(list_config, id);

    if (!item_name) {
        return false;
    }

    if (!jsconfig.containsKey(item_name)) {
        return false;
    }

    strcpy(value, jsconfig[item_name]);

    return true;
}

double GetTotalGallons(void) {
    return total_gallons;
}

bool isEnbBuzzer() {
    int en = ConfigGetInt(CONF_BUZZ_EN);

    return en != 0 ? true : false;
}

int GetBuzeerIntervalWarning() {
    return (int)(ConfigGetDouble(CONF_BEEP_WARNING_INTERVAL) * 1000);
}

void EraseData(void) {
    if (!SPIFFS.begin(true)) {
        LOG_ERR("%s", "An Error has occurred while mounting SPIFFS");
        return;
    }

    SPIFFS.remove(record_file);
    SPIFFS.remove(config_file);
    SPIFFS.remove(position_file);
    SPIFFS.remove(gallons_file);
    nvs_flash_erase();
}

const timezone_t *GetListTZ(void) {
    return list_tz;
}
