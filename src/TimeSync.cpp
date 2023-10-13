/*
 *  time_sync.c
 *
 *  Created on: Nov 15, 2021
 */

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <time.h>
#include <sys/time.h>
#include <esp_sntp.h>
#include <esp_attr.h>

#include "appconfig.h"
#include "log.h"
#include "TimeSync.h"
#include "VTimer.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
#define EPOCH_TIME_BEGINNING         1609502400 /* 12:00:00 1/1/2021 */
#define OFFSET_YEAR                  1900
#define OFFSET_MONTH                 1
#define OFFSET_DATE                  0
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static uint32_t current_epoch_time = EPOCH_TIME_BEGINNING;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void TimeSyncNotificationCb(struct timeval *tv);

/******************************************************************************/

/**
 * @brief  Time sync callback
 */
static void TimeSyncNotificationCb(struct timeval *tv)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    /* Check time is syns, 1609502400 12:00:00 1/1/2021 */
    if (now > EPOCH_TIME_BEGINNING)
    {
        current_epoch_time = now;
        localtime_r(&now, &timeinfo);
    }
    LOG_DBG("Sync time, epoch %lu", current_epoch_time);
}

/**
 * @brief  Time sync task
 */
static void TimeSyncHandle(void* arg)
{
    /* Increase epoch time */
    current_epoch_time++;
    VTimerStart(1000, TimeSyncHandle, NULL);
}

/******************************************************************************/

/**
 * @brief  set timezone
 */
void TimeSyncSetTz(const char* tz)
{
    setenv("TZ", tz, 1);
    tzset();
    LOG_DBG("Set timezone %s", tz);
}

/**
 * @brief  get epoch time
 */
uint32_t TimeSyncGetEpoch(void)
{
    return current_epoch_time;
}

const char *TimeSyncGetStrDateTime() {
    const int mul_tick = 1000 / configTICK_RATE_HZ;
    time_t      now = current_epoch_time;
    struct tm   ts;
    static char buf[64];
    uint32_t msec;
    int index;

    /* Format time, "yyyy-mm-dd hh:mm:ss.xxx" */
    ts = *localtime(&now);
    msec = mul_tick * xTaskGetTickCount() % 1000;
    index = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
    sprintf(&buf[index], ".%03d", msec);

    return buf;
}

/**
 * @brief  init sntp for sync time
 */
void TimeSyncInit(void)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(TimeSyncNotificationCb);
    sntp_init();
    sntp_set_sync_interval(SYNC_TIME_INTERVAL);

    VTimerStart(1000, TimeSyncHandle, NULL);
}

void GetDateTime(char *datetime) {
    time_t now = current_epoch_time;
    struct tm ts = *localtime(&now);
 
    sprintf(datetime, "%04d/%02d/%02d %02d:%02d:%02d", 
                      ts.tm_year + OFFSET_YEAR,
                      ts.tm_mday + OFFSET_DATE,
                      ts.tm_mon + OFFSET_MONTH,
                      ts.tm_hour,
                      ts.tm_min,
                      ts.tm_sec);
}

void GetDate(char *date) {
    time_t now = current_epoch_time;
    struct tm ts = *localtime(&now);

    sprintf(date, "%02d/%02d/%04d", 
                      ts.tm_mon + OFFSET_MONTH,
                      ts.tm_mday + OFFSET_DATE,
                      ts.tm_year + OFFSET_YEAR);
}

void GetTime(char *mtime) {
    time_t now = current_epoch_time;
    struct tm ts = *localtime(&now);

    sprintf(mtime, "%02d:%02d:%02d", 
                      ts.tm_hour,
                      ts.tm_min,
                      ts.tm_sec);
}
