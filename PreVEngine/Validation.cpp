#include "Validation.h"

// !!!! vulkan_wrapper.h must be #included BEFORE vulkan.h !!!!

#ifdef VK_NO_PROTOTYPES

#ifdef __LINUX__
#include <vulkan_wrapper.cpp>
#endif
struct INITVULKAN {
    INITVULKAN()
    {
        bool success = (InitVulkan() == 1); // Returns true if this device supports Vulkan.
        printf("Initialize Vulkan: ");
        print(success ? ConsoleColor::GREEN : ConsoleColor::RED, success ? "SUCCESS\n" : "FAILED (Vulkan driver not found.)\n");
    }
} INITVULKAN; // Run this function BEFORE main.
#endif

void SetConsoleTextColor(const ConsoleColor color)
{
#ifdef _WIN32
    const char bgr[] = { 7, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15 }; // RGB-to-BGR
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, bgr[static_cast<uint32_t>(color)]);
#elif __LINUX__
    if (color == ConsoleColor::FAINT) {
        printf("\033[37m\033[2m"); // set faint white
        return;
    }
    printf("\033[%dm", (int(color) & 8) ? 1 : 0); // bright or normal
    if (bool(color)) {
        printf("\033[3%dm", int(color) & 7); // set text color
    }
#endif
}

#if !defined(NDEBUG) || defined(ENABLE_LOGGING) || defined(ENABLE_VALIDATION)
const char* VkResultStr(const VkResult err)
{
    switch (err) {
#define STR(r) \
    case r:    \
        return #r
        STR(VK_SUCCESS); // 0
        STR(VK_NOT_READY); // 1
        STR(VK_TIMEOUT); // 2
        STR(VK_EVENT_SET); // 3
        STR(VK_EVENT_RESET); // 4
        STR(VK_INCOMPLETE); // 5

        STR(VK_ERROR_OUT_OF_HOST_MEMORY); // -1
        STR(VK_ERROR_OUT_OF_DEVICE_MEMORY); // -2
        STR(VK_ERROR_INITIALIZATION_FAILED); // -3
        STR(VK_ERROR_DEVICE_LOST); // -4
        STR(VK_ERROR_MEMORY_MAP_FAILED); // -5
        STR(VK_ERROR_LAYER_NOT_PRESENT); // -6
        STR(VK_ERROR_EXTENSION_NOT_PRESENT); // -7
        STR(VK_ERROR_FEATURE_NOT_PRESENT); // -8
        STR(VK_ERROR_INCOMPATIBLE_DRIVER); // -9
        STR(VK_ERROR_TOO_MANY_OBJECTS); // -10
        STR(VK_ERROR_FORMAT_NOT_SUPPORTED); // -11
        STR(VK_ERROR_FRAGMENTED_POOL); // -12

        STR(VK_ERROR_SURFACE_LOST_KHR); // -1000000000
        STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR); // -1000000001
        STR(VK_SUBOPTIMAL_KHR); //  1000001003
        STR(VK_ERROR_OUT_OF_DATE_KHR); // -1000001004
        STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR); // -1000003001
        STR(VK_ERROR_VALIDATION_FAILED_EXT); // -1000011001
        STR(VK_ERROR_INVALID_SHADER_NV); // -1000012000

        STR(VK_ERROR_OUT_OF_POOL_MEMORY); // -1000069000
        STR(VK_ERROR_INVALID_EXTERNAL_HANDLE); // -1000072003
        STR(VK_ERROR_NOT_PERMITTED_EXT); // -1000174001
#undef STR
    default:
        return "UNKNOWN_RESULT";
    }
}

void ShowVkResult(const VkResult err)
{
    if (err > 0)
        _LOGW("%s ", VkResultStr(err)); // Print warning
    if (err < 0)
        _LOGE("%s ", VkResultStr(err)); // Print error
}
#else
void ShowVkResult(VkResult err)
{
}
#endif

namespace PreVEngine {
#ifdef ENABLE_VALIDATION

#ifdef VK_USE_PLATFORM_ANDROID_KHR
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData)
{
    if (objType == VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT && msgCode <= 1)
        return false;

    char buf[1024];
    snprintf(buf, sizeof(buf), "[%s] : %s\n", pLayerPrefix, pMsg);

    switch (msgFlags) {
    case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        _LOGI("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        _LOGW("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
        _LOGV("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        _LOGE("%s\n", buf);
        return VK_TRUE;
    case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
        _LOGD("%s", buf);
        return VK_FALSE;
    default:
        return VK_FALSE;
    }
}
#else
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    char buf[1024];
    snprintf(buf, sizeof(buf), "[%s] : %s\n", pCallbackData->pMessageIdName, pCallbackData->pMessage);

    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        _LOGD("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        _LOGI("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        _LOGW("%s", buf);
        return VK_FALSE;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        _LOGE("%s\n", buf);
        return VK_TRUE;
    default:
        return VK_FALSE;
    }
}
#endif

void DebugReport::Init(VkInstance inst)
{
    assert(!!inst);

    m_instance = inst;

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    m_vkCreateDebugCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT");
    m_vkDestroyDebugCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT");

    Destroy(); // Destroy old report before creating new one

    m_flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | // 1
        VK_DEBUG_REPORT_WARNING_BIT_EXT | // 2
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | // 4
        VK_DEBUG_REPORT_ERROR_BIT_EXT | // 8
        VK_DEBUG_REPORT_DEBUG_BIT_EXT | //16
        0;

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    createInfo.pNext = nullptr;
    createInfo.flags = m_flags;
    createInfo.pfnCallback = DebugCallback; // Callback function to call
    createInfo.pUserData = nullptr;
    VKERRCHECK(m_vkCreateDebugCallbackEXT(m_instance, &createInfo, nullptr, &m_debugCallback));
#else
    m_vkCreateDebugCallbackEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkCreateDebugUtilsMessengerEXT");
    m_vkDestroyDebugCallbackEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkDestroyDebugUtilsMessengerEXT");

    Destroy(); // Destroy old report before creating new one

    m_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
        | 0;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.flags = 0;
    createInfo.messageSeverity = m_flags;
    createInfo.pfnUserCallback = DebugCallback;
    VKERRCHECK(m_vkCreateDebugCallbackEXT(m_instance, &createInfo, nullptr, &m_debugCallback));
#endif
}

void DebugReport::Destroy()
{
    if (m_debugCallback) {
        m_vkDestroyDebugCallbackEXT(m_instance, m_debugCallback, nullptr);
    }
}

#else // No Validation

#endif // ENABLE_VALIDATION

DebugReport::DebugReport()
    : m_vkCreateDebugCallbackEXT(VK_NULL_HANDLE)
    , m_vkDestroyDebugCallbackEXT(VK_NULL_HANDLE)
    , m_debugCallback(VK_NULL_HANDLE)
    , m_instance(nullptr)
    , m_flags(0)
{
}
} // namespace PreVEngine
