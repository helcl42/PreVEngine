#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "Common.h"

#include <assert.h>
#include <stdio.h>

enum class ConsoleColor {
    RESET,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE, // normal colors
    FAINT,
    BRED,
    BGREEN,
    BYELLOW,
    BBLUE,
    BMAGENTA,
    BCYAN,
    BRIGHT // bright colors
};

void SetConsoleTextColor(const ConsoleColor color);

#define print(COLOR, ...)                         \
    {                                             \
        SetConsoleTextColor(COLOR);               \
        printf(__VA_ARGS__);                      \
        SetConsoleTextColor(ConsoleColor::RESET); \
    }

#ifdef TARGET_PLATFORM_ANDROID
#include <android/log.h>
#include <jni.h>
#define LOG_TAG "PreVEngine"
#define _LOG(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define _LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define _LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define _LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define _LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define _LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define _LOG(...)            \
    {                        \
        printf(__VA_ARGS__); \
        printf("\n");        \
    }
#define _LOGV(...)                           \
    {                                        \
        print(ConsoleColor::CYAN, "PERF: "); \
        printf(__VA_ARGS__);                 \
        printf("\n");                        \
    }
#define _LOGD(...)                            \
    {                                         \
        print(ConsoleColor::BLUE, "DEBUG: "); \
        printf(__VA_ARGS__);                  \
        printf("\n");                         \
    }
#define _LOGI(...)                            \
    {                                         \
        print(ConsoleColor::GREEN, "INFO: "); \
        printf(__VA_ARGS__);                  \
        printf("\n");                         \
    }
#define _LOGW(...)                                \
    {                                             \
        print(ConsoleColor::YELLOW, "WARNING: "); \
        printf(__VA_ARGS__);                      \
        printf("\n");                             \
    }
#define _LOGE(...)                           \
    {                                        \
        print(ConsoleColor::RED, "ERROR: "); \
        printf(__VA_ARGS__);                 \
        printf("\n");                        \
    }
#endif

#ifdef ENABLE_LOGGING
#define LOG(...) _LOG(__VA_ARGS__)
#define LOGV(...) _LOGV(__VA_ARGS__)
#define LOGD(...) _LOGD(__VA_ARGS__)
#define LOGI(...) _LOGI(__VA_ARGS__)
#define LOGW(...) _LOGW(__VA_ARGS__)
#define LOGE(...) _LOGE(__VA_ARGS__)
#define ASSERT(EXPRESSION, ...)                    \
    {                                              \
        if (!EXPRESSION) {                         \
            _LOGE(__VA_ARGS__);                    \
            printf("%s:%d\n", __FILE__, __LINE__); \
            PAUSE;                                 \
            exit(0);                               \
        }                                          \
    }
#else
#define LOG(...) \
    {            \
    }
#define LOGV(...) \
    {             \
    }
#define LOGD(...) \
    {             \
    }
#define LOGI(...) \
    {             \
    }
#define LOGW(...) \
    {             \
    }
#define LOGE(...) \
    {             \
    }
#define ASSERT(EXPRESSION, ...) \
    {                           \
    }
#endif

#endif // !__LOGGER_H__
