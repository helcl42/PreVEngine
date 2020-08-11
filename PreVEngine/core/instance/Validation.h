/*
*  #define ENABLE_VALIDATION 1          // Enables Vulkan Validation
*  #define ENABLE_LOGGING    1          // Enables LOG* print messages
*/

#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#if defined(__linux__) && !defined(__ANDROID__) // Linux (desktop only)
#define __LINUX__ 1
#endif

#if defined(__ANDROID__) && !defined(NDEBUG)
#define ENABLE_VALIDATION 1
#define ENABLE_LOGGING 1
//#define NDEBUG               //cuts 4kb off apk size
#endif

#ifdef _WIN32
#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define cTICK "\xFB" /* On Windows, use Square-root as tick mark */
#define PAUSE system("pause")
#elif __ANDROID__
#include <native.h>
#define cTICK "\u2713"
#define PAUSE
#elif __LINUX__
#include <xkbcommon/xkbcommon.h>
#define cTICK "\u2713"
#define PAUSE
#endif

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

#ifdef ANDROID
#include <android/log.h>
#include <jni.h>
#define LOG_TAG "PreVEngine"
#define _LOG(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define _LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define _LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define _LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define _LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define _LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
//#define printf(...)  __android_log_print(ANDROID_LOG_INFO   ,LOG_TAG,__VA_ARGS__)
#else
#define _LOG(...)            \
    {                        \
        printf(__VA_ARGS__); \
    }
#define _LOGV(...)                            \
    {                                         \
        print(ConsoleColor::CYAN, "PERF: "); \
        printf(__VA_ARGS__);                  \
    }
#define _LOGD(...)                            \
    {                                         \
        print(ConsoleColor::BLUE, "DEBUG: "); \
        printf(__VA_ARGS__);                  \
    }
#define _LOGI(...)                             \
    {                                          \
        print(ConsoleColor::GREEN, "INFO: "); \
        printf(__VA_ARGS__);                   \
    }
#define _LOGW(...)                                \
    {                                             \
        print(ConsoleColor::YELLOW, "WARNING: "); \
        printf(__VA_ARGS__);                      \
    }
#define _LOGE(...)                           \
    {                                        \
        print(ConsoleColor::RED, "ERROR: "); \
        printf(__VA_ARGS__);                 \
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

#include <assert.h>
#include <stdio.h>

//=========================================== Vulkan Wrapper ===========================================
//  By default, all Vulkan functions call the loader trampoline-code, which then calls the ICD or layers.
//  Alternatively, vulkan_wrapper.h can be used to replace all Vulkan functions with a dispatch-table,
//  which skips the loader, and calls the ICD directly, thereby improving performance.
//  Android has no loader, and always uses vulkan_wrapper.h.
//  For more details, see /source/loader/LoaderAndLayerInterface.md in the VS or LVL repo.
//
//  WARNING: If you enable USE_VULKAN_WRAPPER, make sure vulkan.h is NEVER #included before vulkan_wrapper.h
//
//#define USE_VULKAN_WRAPPER

#ifdef USE_VULKAN_WRAPPER
#include <vulkan_wrapper.h> // PC: Build dispatch table, so we can skip loader trampoline-code
#else
#include <vulkan/vulkan.h> // Android: This must be included AFTER native.h
#endif

void ShowVkResult(VkResult err);
const char* VkResultStr(const VkResult err);

#ifdef NDEBUG
#define VKERRCHECK(VKFN) \
    {                    \
        (void)VKFN;      \
    }
#else
#define VKERRCHECK(VKFN)                           \
    {                                              \
        VkResult VKRESULT = VKFN;                  \
        ShowVkResult(VKRESULT);                    \
        assert(VKRESULT >= 0);                     \
        if (VKRESULT)                              \
            printf("%s:%d\n", __FILE__, __LINE__); \
    }
#endif

namespace PreVEngine {
class DebugReport {
private:
    VkInstance m_instance;

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    PFN_vkCreateDebugReportCallbackEXT m_vkCreateDebugCallbackEXT;

    PFN_vkDestroyDebugReportCallbackEXT m_vkDestroyDebugCallbackEXT;

    VkDebugReportCallbackEXT m_debugCallback;

    VkDebugReportFlagsEXT m_flags;
#else
    PFN_vkCreateDebugUtilsMessengerEXT m_vkCreateDebugCallbackEXT;

    PFN_vkDestroyDebugUtilsMessengerEXT m_vkDestroyDebugCallbackEXT;

    VkDebugUtilsMessengerEXT m_debugCallback;

    VkDebugUtilsMessageSeverityFlagsEXT m_flags;

#endif
public:
    DebugReport();

public:
    void Init(VkInstance inst);

    void Destroy();
};
} // namespace PreVEngine

#endif
