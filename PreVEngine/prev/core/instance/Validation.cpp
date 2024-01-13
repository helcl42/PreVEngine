#include "Validation.h"

#include "../../common/Logger.h"

namespace prev::core::instance {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData)
{
    if (objType == VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT && msgCode <= 1) {
        return false;
    }

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
        VK_DEBUG_REPORT_DEBUG_BIT_EXT | // 16
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

DebugReport::DebugReport()
    : m_instance(nullptr)
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    , m_vkCreateDebugCallbackEXT(VK_NULL_HANDLE)
    , m_vkDestroyDebugCallbackEXT(VK_NULL_HANDLE)
    , m_debugCallback(VK_NULL_HANDLE)
    , m_flags(0)
#else
    , m_vkCreateDebugCallbackEXT(VK_NULL_HANDLE)
    , m_vkDestroyDebugCallbackEXT(VK_NULL_HANDLE)
    , m_debugCallback(VK_NULL_HANDLE)
    , m_flags(0)
#endif
{
}
} // namespace prev::core::instance
