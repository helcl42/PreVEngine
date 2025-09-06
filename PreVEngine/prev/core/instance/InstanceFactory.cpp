#include "InstanceFactory.h"

#include "Extensions.h"
#include "InstanceFactory.h"
#include "Layers.h"
#include "Validation.h"

#include "../../common/Logger.h"

#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
#include <vulkan/vulkan_metal.h>
#endif

#include <stdexcept>

namespace prev::core::instance {
namespace {
    Layers CreateLayers(const bool enableValidation, const std::vector<std::string>& extLayers)
    {
        Layers layers;
        if (enableValidation) {
            layers.Pick("VK_LAYER_KHRONOS_validation");
        }

        for (const auto& layer : extLayers) {
            layers.Pick(layer);
        }
        return layers;
    }

    Extensions CreateExtensions(const bool enableValidation, const std::vector<std::string>& extExtensions)
    {
        Extensions extensions;
        extensions.Pick(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        extensions.Pick(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME); // VK_KHR_external_memory_fd
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
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
        extensions.Pick(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
        extensions.Pick(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        extensions.Pick(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        if (enableValidation) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
            extensions.Pick(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#else
            extensions.Pick(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // in Debug mode, Enable Validation
#endif
        }

        for (const auto& ext : extExtensions) {
            extensions.Pick(ext);
        }
        return extensions;
    }

    VkApplicationInfo CreateApplicationInfo(const std::string& appName)
    {
        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = 1;
        appInfo.pEngineName = "PreVEngine";
        appInfo.engineVersion = 1;
        appInfo.apiVersion = VK_API_VERSION_1_1;
        return appInfo;
    }

    VkInstanceCreateInfo CreateInstanceCreateInfo(const Layers& layers, const Extensions& extensions, const VkApplicationInfo& appInfo)
    {
        VkInstanceCreateInfo instanceInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
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
        return instanceInfo;
    }

    VkInstance CreateInstance(const VkInstanceCreateInfo& createInfo)
    {
        VkInstance vkInstance;
        VKERRCHECK(vkCreateInstance(&createInfo, nullptr, &vkInstance));
        if (!vkInstance) {
            throw std::runtime_error("Could not create VK instance.");
        }

#ifdef ENABLE_VK_LOADER
        volkLoadInstance(vkInstance);
#endif
        return vkInstance;
    }

    std::unique_ptr<ValidationReporter> CreateValidationReporter(const VkInstance instance, const Extensions& extensions)
    {
        std::unique_ptr<ValidationReporter> validationReporter{};
        if (extensions.IsPicked(VK_EXT_DEBUG_REPORT_EXTENSION_NAME) || extensions.IsPicked(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            validationReporter = std::make_unique<ValidationReporter>(instance);
        }
        return validationReporter;
    }
} // namespace

std::unique_ptr<Instance> InstanceFactory::Create(const bool enableValidation, const std::string& appName, const std::vector<std::string>& extLayers, const std::vector<std::string>& extExtensions) const
{
    const auto layers{ CreateLayers(enableValidation, extLayers) };
    layers.Print();

    const auto extensions{ CreateExtensions(enableValidation, extExtensions) };
    extensions.Print();

    const auto appInfo{ CreateApplicationInfo(appName) };
    const auto instanceCreateInfo{ CreateInstanceCreateInfo(layers, extensions, appInfo) };

    const auto vkInstance{ CreateInstance(instanceCreateInfo) };
    auto validationReporter{ CreateValidationReporter(vkInstance, extensions) };

    auto instance{ std::make_unique<Instance>(vkInstance, std::move(validationReporter)) };

    LOGI("Vulkan Instance created");

    return instance;
}
} // namespace prev::core::instance