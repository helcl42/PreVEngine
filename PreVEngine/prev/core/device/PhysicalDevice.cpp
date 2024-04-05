#include "PhysicalDevice.h"

#include "../../util/VkUtils.h"

namespace prev::core::device {
PhysicalDevice::PhysicalDevice()
    : m_handle(VK_NULL_HANDLE)
    , m_availableProperties()
    , m_availableFeatures()
    , m_extensions()
{
}

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice gpu)
    : m_handle(gpu)
    , m_extensions(gpu)
{
    vkGetPhysicalDeviceFeatures(gpu, &m_availableFeatures);
    vkGetPhysicalDeviceProperties(gpu, &m_availableProperties);

    // enable features here -> might be overriden by an inherited class ??
    if (m_availableFeatures.samplerAnisotropy) {
        m_enabledFeatures.samplerAnisotropy = VK_TRUE;
    }
    if (m_availableFeatures.depthClamp) {
        m_enabledFeatures.depthClamp = VK_TRUE;
    }
    if (m_availableFeatures.shaderClipDistance) {
        m_enabledFeatures.shaderClipDistance = VK_TRUE;
    }
    if (m_availableFeatures.fillModeNonSolid) {
        m_enabledFeatures.fillModeNonSolid = VK_TRUE;
    }
    if (m_availableFeatures.geometryShader) {
        m_enabledFeatures.geometryShader = VK_TRUE;
    }
    if (m_availableFeatures.sampleRateShading) {
        m_enabledFeatures.sampleRateShading = VK_TRUE;
    }

    uint32_t familyCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
    m_queueFamilies.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, m_queueFamilies.data());

    m_extensions.Pick(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
    m_extensions.Pick("VK_KHR_portability_subset");
#endif
}

// Find queue-family with requred flags, and can present to given surface. (if provided)
// Returns the QueueFamily index, or -1 if not found.
int32_t PhysicalDevice::FindQueueFamily(const VkQueueFlags flags, const VkQueueFlags unwantedFlags, const VkSurfaceKHR surface) const
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_queueFamilies.size()); ++i) {
        const auto& queueFamily{ m_queueFamilies[i] };
        if ((queueFamily.queueFlags & flags) != flags || (queueFamily.queueFlags & unwantedFlags) != 0) {
            continue;
        }

        if (surface != VK_NULL_HANDLE) {
            VkBool32 canPresent;
            VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, static_cast<uint32_t>(i), surface, &canPresent));
            if (canPresent == VK_FALSE) {
                continue;
            }
        }

        return static_cast<int32_t>(i);
    }
    return -1;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::SurfaceFormats(const VkSurfaceKHR surface) const
{
    uint32_t count{ 0 };
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, nullptr);

    ASSERT(!!count, "No supported surface formats found.");

    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, formats.data());

    return formats;
}

//--Returns the first supported surface color format from the preferredFormats list, or VK_FORMAT_UNDEFINED if no match found.
VkFormat PhysicalDevice::FindSurfaceFormat(const VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats) const
{
    const auto formats{ SurfaceFormats(surface) }; // get list of supported surface formats
    for (const auto& preferedFormat : preferredFormats) {
        for (const auto& format : formats) {
            if (format.format == preferedFormat) {
                return format.format;
            }
        }
    }

    // return formats[0].format;  //first supported format
    return VK_FORMAT_UNDEFINED;
}

VkFormat PhysicalDevice::FindDepthFormat(const std::vector<VkFormat>& preferredFormats) const
{
    for (const auto& preferedFormat : preferredFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(m_handle, preferedFormat, &formatProps);

        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return preferedFormat;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

const VkPhysicalDeviceProperties& PhysicalDevice::GetProperties() const
{
    return m_availableProperties;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::GetAvailableFeatures() const
{
    return m_availableFeatures;
}

const std::vector<VkQueueFamilyProperties>& PhysicalDevice::GetQueueFamilies() const
{
    return m_queueFamilies;
}

const DeviceExtensions& PhysicalDevice::GetExtensions() const
{
    return m_extensions;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::GetEnabledFeatures() const
{
    return m_enabledFeatures;
}

PhysicalDevice::operator VkPhysicalDevice() const
{
    return m_handle;
}

void PhysicalDevice::Print(const bool showQueues) const
{
    auto deviceTypeToString = [](const VkPhysicalDeviceType type) -> std::string {
        static const std::string devTypea[] = {
            "OTHER",
            "INTEGRATED",
            "DISCRETE",
            "VIRTUAL",
            "CPU"
        };
        return devTypea[type];
    };

    const auto vendor{ prev::util::vk::VendorIdToString(m_availableProperties.vendorID) };
    const auto& gpuProps{ GetProperties() };
    LOGI("\t%s %s %s\n", deviceTypeToString(gpuProps.deviceType).c_str(), vendor.c_str(), gpuProps.deviceName);

    if (showQueues) {
        const auto queueFamilies{ GetQueueFamilies() };
        for (size_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& queueProps{ queueFamilies[i] };
            LOGI("\t\tQueue-family: %zd count: %2d flags: [ %s]\n", i, queueProps.queueCount, prev::util::vk::QueueFlagsToString(queueProps.queueFlags).c_str());
        }
    }
}
} // namespace prev::core::device
