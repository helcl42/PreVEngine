#include "Instance.h"

#include "../../common/Logger.h"

#include <stdexcept>

#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
#include <vulkan/vulkan_metal.h>
#endif

namespace prev::core::instance {
Instance::Instance(const bool enableValidation, const char* appName, const char* engineName)
{
    Layers layers;
    if (enableValidation) {
        layers.Pick("VK_LAYER_KHRONOS_validation");
    }
    layers.Print();

    Extensions extensions;
    if (extensions.Pick(VK_KHR_SURFACE_EXTENSION_NAME)) {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        extensions.Pick(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
        extensions.Pick(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        extensions.Pick(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        extensions.Pick(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
        extensions.Pick(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MIR_KHR)
        extensions.Pick(VK_KHR_MIR_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        extensions.Pick(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        extensions.Pick(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        extensions.Pick(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        extensions.Pick(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    } else {
        LOGE("Failed to load VK_KHR_Surface");
    }

    if (enableValidation) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        extensions.Pick(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#else
        extensions.Pick(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // in Debug mode, Enable Validation
#endif
    }
    extensions.Print();

    Create(layers, extensions, appName, engineName);
}

Instance::Instance(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName)
{
    Create(layers, extensions, appName, engineName);
}

void Instance::Create(const Layers& layers, const Extensions& extensions, const char* appName, const char* engineName)
{
    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = 1;
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    appInfo.apiVersion = VK_API_VERSION_1_0;
#else
    appInfo.apiVersion = VK_API_VERSION_1_1;
#endif

    VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instanceInfo.pNext = nullptr;
#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
    instanceInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    instanceInfo.flags = 0;
#endif
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensions.GetPickCount();
    instanceInfo.ppEnabledExtensionNames = extensions.GetPickListRaw();
    instanceInfo.enabledLayerCount = layers.GetPickCount();
    instanceInfo.ppEnabledLayerNames = layers.GetPickListRaw();

    VKERRCHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance));
    if (!m_instance) {
        throw std::runtime_error("Could not create VK instance.");
    }

#ifdef ENABLE_VK_LOADER
    volkLoadInstance(m_instance);
#endif

    LOGI("Vulkan Instance created\n");

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    if (extensions.IsPicked(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        m_validationReporter.Init(m_instance); // If VK_EXT_debug_report is loaded, initialize it.
    }
#else
    if (extensions.IsPicked(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_validationReporter.Init(m_instance); // If VK_EXT_utils is loaded, initialize it.
    }
#endif
}

void Instance::Print() const
{
    LOGI("->Instance %s created.\n", (!!m_instance) ? "" : "NOT");
}

Instance::~Instance()
{
    m_validationReporter.Destroy(); // Must be called BEFORE vkDestroyInstance()

    vkDestroyInstance(m_instance, nullptr);
    LOGI("Vulkan Instance destroyed\n");
}

Instance::operator VkInstance() const
{
    return m_instance;
}
} // namespace prev::core::instance
