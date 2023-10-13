/*
 *  @file: Utils.h
 *
 *  Created on:
 */

/******************************************************************************/
#ifndef __UTILS_H
#define __UTILS_H
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
/******************************************************************************/
/*                              TYPES and DEFINITIONS                         */
/******************************************************************************/
#define PARAM_MAX_LENGTH                           64
#define TOKEN_MAX_LENGTH                           256
#define USERID_MAX_LENGTH                          256
#define URI_MAX_LENGTH                             256
#define EMAIL_MAX_LENGTH                           256
#define MAX_CONFIG_LENGTH                          2048
#define MAX_RECORD_ITEM_LENGTH                     128
#define MAX_RECORD_ALL_LENGTH                      2048

#define FILE_PATH_RECORD                          "/record.txt"
#define FILE_PATH_CONFIG                          "/config.txt"
#define FILE_PATH_POSITION                        "/position.txt"
#define FILE_PATH_TOTAL_GALLONS                   "/gallons.txt"

typedef enum __config_id {
    CONF_SSID,
    CONF_PASSWD,
    CONF_EMAIL,
    CONF_TZ,
    CONF_DEVICE_REGISTER, 
    CONF_USER_ID,
    CONF_DEVICE_ID,
    CONF_TOKEN,
    CONF_BASE_URI,
    CONF_AVG_GPM,
    CONF_SCORE_LIMIT,
    CONF_DET_THRES,
    CONF_DET_TCNT,
    CONF_DET_TABL_SIZE,
    CONF_DETCNT_MAX,
    CONF_DETCNT_MIN,
    CONF_MAX_GALLONS_WARNING,
    CONF_BEEP_WARNING_INTERVAL,
    CONF_MODEL,
    CONF_MIC_VOL_GAIN,
    CONF_BUZZ_EN,
    CONF_FILTER_EN,
    CONF_WATER_ALERT1_TIME,
    CONF_WATER_ALERT2_TIME,
    CONF_WATER_ALERT3_TIME, 
    CONF_WATER_ALERT4_TIME,
    CONF_WATER_KILLER_TIME,
    CONF_WATER_KILLER_RESET_TIME,
    CONF_WATER_AVG_RESET_TIME,
    CONF_IFTTT_URL_ON,
    CONF_IFTTT_URL_OFF,
    CONF_IFTTT_EN,
    
    NUMBER_OF_CONFIG
} config_id_t;

typedef enum __command_id {
    COMMAND_NONE,
    COMMAND_OTA,
    COMMAND_GETLOG,
} command_id_t;

typedef enum __value_type {
    VTYPE_NONE,
    VTYPE_STR,
    VTYPE_INT,
    VTYPE_DOUBE
} vtype_t;

typedef struct __attribute__((packed)) __record {
    int id;
    char date[12];
    char time[10];
    char tot_time[10];
    char tot_gal[10];
} record_t;

typedef struct {
    const char *zone;
    const char *tz;
} timezone_t;

typedef struct {
    char url[256];
    char version[16];
} ota_cmd_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void ListAllFiles(void);

void RecordAppend(record_t *record);

void RecordInit();

int RecordGetStr(char *strrecord);

int RecordGet(record_t *record, int maxrecord);

bool RecordGetStrById(int id, char *strrecord);

void RecordSetPosition(int p);

void RecordDeleteAll(void);

void ConfigInit(void);

bool ConfigSet(config_id_t id, char *value);

bool ConfigSetInt(config_id_t id, int value);

bool ConfigSetDouble(config_id_t id, double value);

bool ConfigSetJsStr(char *value);

int ConfigGetInt(config_id_t id);

double ConfigGetDouble(config_id_t id);

bool ConfigGetStr(config_id_t id, char *value);

bool isEnbBuzzer();

int GetBuzeerIntervalWarning();

void ConfigShow();

void EraseData(void);

const timezone_t *GetListTZ(void);

double GetTotalGallons(void);

ota_cmd_t *GetOTACmd(void);

bool isNewerVersion(const char *version);

command_id_t GetPendingCommand(void);

#endif /* __UTILS_H */
