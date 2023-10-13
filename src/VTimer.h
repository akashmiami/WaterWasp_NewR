#ifndef __VTIMER_H
#define __VTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_TIMER_COUNT                            8

typedef void (*VTimerCallback_t)(void *arg);

void VTimerInit(void);

void VTimerStart(uint32_t msec, VTimerCallback_t callback, void *arg);

void VTimerCancel(VTimerCallback_t callback);

bool VTimerIsRunning(VTimerCallback_t callback);


#ifdef __cplusplus
}
#endif

#endif //__VTIMER_H
