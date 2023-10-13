#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AppConfig.h"
#include "Log.h"
#include "VTimer.h"

#define TIMER_EXPIRED(timer) ((uint32_t)(xTaskGetTickCount() - timer.start) >= timer.time)

typedef struct __vtimerdata {
    uint32_t start;                // msec start
    uint32_t time;                 // msec timeout
    VTimerCallback_t callback;     // callback function
    void *params;                  // arg for callback
} vtimerdata_t;

static TaskHandle_t VTimerTaskHandle;
static vtimerdata_t vtimerlist[MAX_TIMER_COUNT] = { 0 };
static void vtimer_process(void *arg);

void VTimerInit(void) {
    LOG_DBG("%s", "VTimer init");
    xTaskCreatePinnedToCore(
        vtimer_process,          /* Task function. */
        "VTimer",                /* name of task. */
        2048,                    /* Stack size of task */
        NULL,                    /* parameter of the task */
        1,                       /* priority of the task */
        &VTimerTaskHandle,       /* Task handle to keep track of created task */
        1);                      /* pin task to core 1 */
}

void VTimerStart(uint32_t msec, VTimerCallback_t callback, void *arg) {
    uint32_t index;

    vTaskSuspendAll();
    for (index = 0ul; index < MAX_TIMER_COUNT; index++) {
        if ((vtimerlist[index].callback == NULL) || (vtimerlist[index].callback == callback)) {
            vtimerlist[index].start = xTaskGetTickCount();
            vtimerlist[index].callback = callback;
            vtimerlist[index].time = msec;
            vtimerlist[index].params = arg;

            for (index = index + 1u; index < MAX_TIMER_COUNT; index++) {
                if (vtimerlist[index].callback == callback) {
                    vtimerlist[index].callback = NULL;
                }
            }
            goto lb_exit;
        }
    }

lb_exit:
    xTaskResumeAll();
}

void VTimerCancel(VTimerCallback_t callback) {
    uint32_t index;

    vTaskSuspendAll();
    for (index = 0ul; index < MAX_TIMER_COUNT; index++) {
        if (vtimerlist[index].callback == callback) {
            vtimerlist[index].callback = NULL;
            break;
        }
    }
    xTaskResumeAll();
}

bool VTimerIsRunning(VTimerCallback_t callback) {
    uint32_t index;

    for (index = 0ul; index < MAX_TIMER_COUNT; index++) {
        if (vtimerlist[index].callback == callback) {
            return true;
        }
    }
    return false;
}

static void vtimer_process(void* arg) {
    uint32_t index;
    void *param = NULL;
    VTimerCallback_t callback = NULL;

    while (1) {
        for (index = 0ul; index < MAX_TIMER_COUNT; index++) {
            if ((vtimerlist[index].callback != NULL) && TIMER_EXPIRED(vtimerlist[index])) {
                callback = vtimerlist[index].callback;
                param = vtimerlist[index].params;
                VTimerCancel(callback);
                callback(param);
                //vtimerlist[index].callback = NULL;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
