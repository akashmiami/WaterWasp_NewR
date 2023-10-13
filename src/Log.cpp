#include "AppConfig.h"
#include "Queue.h"
#include "TimeSync.h"
#include "Log.h"

#define LOG_OUTPUT(...) printf(__VA_ARGS__)
#define LOC_SIZE 24

#if BOARD_USING == BOARD_M5TIMER_CAM
#define MAX_LOG_QUEUE_SIZE 16
#else
#define MAX_LOG_QUEUE_SIZE 8
#endif /* */

void *llog_queue = NULL;

void llog_init()
{
#if ENABLE_WEB_LOG
    llog_queue = xqueue_init(MAX_LOG_QUEUE_SIZE);
#endif
}

void llog(uint8_t newline, uint8_t level, char levelstr, const char *file, uint32_t line, const char *format, ...)
{
    static char log_line[LOG_LINE_MAX];
    int index = 0;
    va_list args;

    if (level <= (LOG_LEVEL))
    {
        if (newline)
        {
            char loc_str[LOC_SIZE];
            int loc_sz;
            loc_sz = snprintf(loc_str, sizeof(loc_str), "%s:%d", file, line);
            for (int i = loc_sz; i < LOC_SIZE - 1; i++)
            {
                loc_str[i] = ' ';
            }
            loc_str[LOC_SIZE - 1] = '\0';

#if 1
            index = snprintf(log_line, sizeof(log_line), "[%c][%s] ", levelstr, loc_str);
#else
            index = snprintf(log_line, sizeof(log_line), "[%c][%s][%s] ", levelstr, TimeSyncGetStrDateTime(), loc_str);
#endif
        }
    }

    va_start(args, format);
    index += vsprintf(&log_line[index], format, args);
    va_end(args);

    LOG_OUTPUT(log_line);
    LOG_OUTPUT("\r\n");

#if ENABLE_WEB_LOG
    xqueue_put(llog_queue, (uint8_t *)log_line, index);
#endif
}