#include "Core.h"

#include "../common/Logger.h"

#ifdef ENABLE_VK_LOADER
#define VOLK_IMPLEMENTATION
#include <external/volk/volk.h>
#endif

struct VulkanInitializer {
    VulkanInitializer()
    {
#ifdef ENABLE_VK_LOADER
        const bool success{ volkInitialize() == VK_SUCCESS };
        if (success) {
            const auto version{ volkGetInstanceVersion() };
            LOGI("Vulkan API version %d.%d.%d available (Loaded).", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
        } else {
            LOGE("Vulkan API load failed.");
        }
#else
        auto version{ VK_API_VERSION_1_0 };
        auto FN_vkEnumerateInstanceVersion = PFN_vkEnumerateInstanceVersion(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
        if (vkEnumerateInstanceVersion) {
            vkEnumerateInstanceVersion(&version);
        }
        LOGI("Vulkan API version %d.%d.%d available.", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
#endif
    }
} VulkanInitializer{}; // Run this function BEFORE main.

#if !defined(NDEBUG) || defined(ENABLE_LOGGING)
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
    if (err > 0) {
        LOGW("%s ", VkResultStr(err)); // Print warning
    }
    if (err < 0) {
        LOGE("%s ", VkResultStr(err)); // Print error
    }
}
#else
void ShowVkResult(const VkResult err)
{
}
#endif
