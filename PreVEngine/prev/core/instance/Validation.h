#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#include "../Core.h"

namespace prev::core::instance {
class DebugReport {
public:
    DebugReport();

public:
    void Init(VkInstance inst);

    void Destroy();

private:
    VkInstance m_instance{};

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    PFN_vkCreateDebugReportCallbackEXT m_vkCreateDebugCallbackEXT{};

    PFN_vkDestroyDebugReportCallbackEXT m_vkDestroyDebugCallbackEXT{};

    VkDebugReportCallbackEXT m_debugCallback{};

    VkDebugReportFlagsEXT m_flags{};
#else
    PFN_vkCreateDebugUtilsMessengerEXT m_vkCreateDebugCallbackEXT{};

    PFN_vkDestroyDebugUtilsMessengerEXT m_vkDestroyDebugCallbackEXT{};

    VkDebugUtilsMessengerEXT m_debugCallback{};

    VkDebugUtilsMessageSeverityFlagsEXT m_flags{};

#endif
};
} // namespace prev::core::instance

#endif
