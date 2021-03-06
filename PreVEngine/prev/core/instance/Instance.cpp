﻿#include "Instance.h"

namespace prev::core::instance {
Instance::Instance(const bool enableValidation, const char* appName, const char* engineName)
{
    Layers layers;
#ifdef ENABLE_VALIDATION
    if (enableValidation) {
        layers.Pick("VK_LAYER_KHRONOS_validation");
    }
    layers.Print();
#endif
    Extensions extensions;
    if (extensions.Pick(VK_KHR_SURFACE_EXTENSION_NAME)) {
#ifdef VK_USE_PLATFORM_WIN32_KHR
        extensions.Pick(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_ANDROID_KHR
        extensions.Pick(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_XCB_KHR
        extensions.Pick(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_XLIB_KHR
        extensions.Pick(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_WAYLAND_KHR
        extensions.Pick(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif VK_USE_PLATFORM_MIR_KHR
        extensions.Pick(VK_KHR_MIR_SURFACE_EXTENSION_NAME);
#endif
    } else {
        LOGE("Failed to load VK_KHR_Surface");
    }

#ifdef ENABLE_VALIDATION
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    extensions.Pick(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#else
    extensions.Pick(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // in Debug mode, Enable Validation
#endif
    extensions.Print();
#endif
    assert(extensions.PickCount() >= 2);

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
    appInfo.apiVersion = VK_API_VERSION_1_2;
#endif

    VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensions.PickCount();
    instanceInfo.ppEnabledExtensionNames = extensions.GetPickList();
    instanceInfo.enabledLayerCount = layers.PickCount();
    instanceInfo.ppEnabledLayerNames = layers.GetPickList();

    VKERRCHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance));

    LOGI("Vulkan Instance created\n");

#ifdef ENABLE_VALIDATION
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    if (extensions.IsPicked(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        m_debugReport.Init(m_instance); // If VK_EXT_debug_report is loaded, initialize it.
    }
#else
    if (extensions.IsPicked(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_debugReport.Init(m_instance); // If VK_EXT_utils is loaded, initialize it.
    }
#endif
#endif
}

void Instance::Print() const
{
    printf("->Instance %s created.\n", (!!m_instance) ? "" : "NOT");
}

DebugReport& Instance::GetDebugReport()
{
    return m_debugReport;
}

Instance::~Instance()
{
#ifdef ENABLE_VALIDATION
    m_debugReport.Destroy(); // Must be called BEFORE vkDestroyInstance()
#endif
    vkDestroyInstance(m_instance, nullptr);
    LOGI("Vulkan Instance destroyed\n");
}

Instance::operator VkInstance() const
{
    return m_instance;
}
} // namespace prev::core::instance