/*
 *  time_sync.h
 *
 *  Created on: Nov 15, 2021
 */

#ifndef __TIME_SYNC_H
#define __TIME_SYNC_H

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

/**
 * @brief  init sntp for sync time
 * @param  None
 * @return None
 */
void TimeSyncInit(void);

/**
 * @brief  set timezone
 * @param  tz:  timezone, reference https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv   
 * @return None
 */
void TimeSyncSetTz(const char *tz);

/**
 * @brief  get epoch time
 * @param  None
 * @return None
 */
uint32_t TimeSyncGetEpoch(void);

/**
 * @brief  get str time
 * @return yyyy-mm-dd hh:mm:ss.xxx
 */
const char *TimeSyncGetStrDateTime(void);

void GetDateTime(char *datetime);

void GetDate(char *date);

void GetTime(char *mtime);

#ifdef __cplusplus
}
#endif

#endif // __TIME_SYNC_H
