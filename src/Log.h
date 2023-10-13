#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LLOG_LEVEL_NONE                            0 /*< No log */
#define LLOG_LEVEL_ERR                             1 /*< Errors */
#define LLOG_LEVEL_WARN                            2 /*< Warnings */
#define LLOG_LEVEL_INFO                            3 /*< Basic info */
#define LLOG_LEVEL_DBG                             4 /*< Detailled debug */

#define LOG_LINE_MAX                               2560
#define __FILE_NAME__                              (strrchr(__FILE__, '//') ? strrchr(__FILE__, '//') + 1 : __FILE__)

#define LOG(newline, level, levelstr, format, ...) llog(newline, level, levelstr, __FILE_NAME__, __LINE__, format, __VA_ARGS__)

#define LOG_PRINT(format, ...)                     LOG(1, 0,               'P', format, __VA_ARGS__)
#define LOG_ERR(format, ...)                       LOG(1, LLOG_LEVEL_ERR,  'E', format, __VA_ARGS__)
#define LOG_WARN(format, ...)                      LOG(1, LLOG_LEVEL_WARN, 'W', format, __VA_ARGS__)
#define LOG_INFO(format, ...)                      LOG(1, LLOG_LEVEL_INFO, 'I', format, __VA_ARGS__)
#define LOG_DBG(format, ...)                       LOG(1, LLOG_LEVEL_DBG,  'D', format, __VA_ARGS__)

extern void *llog_queue;

void llog_init();

void llog(uint8_t newline, uint8_t level, char levelstr, const char *file, uint32_t line, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H */
