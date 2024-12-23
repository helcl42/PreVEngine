#include "PhysicalDevice.h"

#include "../../util/VkUtils.h"

namespace prev::core::device {
PhysicalDevice::PhysicalDevice()
    : m_handle{ VK_NULL_HANDLE }
    , m_availableProperties{}
    , m_availableFeatures{}
    , m_extensions{}
{
}

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice gpu, const std::vector<std::string>& extensions)
    : m_handle{ gpu }
    , m_extensions{ gpu }
{
    m_availableProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, {}, {} };
    m_availableFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, {}, {} };
    m_enabledFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, {}, {} };

    vkGetPhysicalDeviceFeatures2(gpu, &m_availableFeatures);
    vkGetPhysicalDeviceProperties2(gpu, &m_availableProperties);

    // enable features here -> might be overridden by an inherited class ??
    if (m_availableFeatures.features.samplerAnisotropy) {
        m_enabledFeatures.features.samplerAnisotropy = VK_TRUE;
    }
    if (m_availableFeatures.features.depthClamp) {
        m_enabledFeatures.features.depthClamp = VK_TRUE;
    }
    if (m_availableFeatures.features.shaderClipDistance) {
        m_enabledFeatures.features.shaderClipDistance = VK_TRUE;
    }
    if (m_availableFeatures.features.fillModeNonSolid) {
        m_enabledFeatures.features.fillModeNonSolid = VK_TRUE;
    }
    if (m_availableFeatures.features.geometryShader) {
        m_enabledFeatures.features.geometryShader = VK_TRUE;
    }
    if (m_availableFeatures.features.tessellationShader) {
        m_enabledFeatures.features.tessellationShader = VK_TRUE;
    }
    if (m_availableFeatures.features.sampleRateShading) {
        m_enabledFeatures.features.sampleRateShading = VK_TRUE;
    }

#ifdef ENABLE_XR
    if (m_availableFeatures.features.multiViewport) {
        m_enabledFeatures.features.multiViewport = VK_TRUE;
    }

    m_physicalDeviceMultiviewFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR, {} };
    m_physicalDeviceMultiviewFeatures.multiview = VK_TRUE;

    m_enabledFeatures.pNext = &m_physicalDeviceMultiviewFeatures;
#endif

    uint32_t familyCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
    m_queueFamilies.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, m_queueFamilies.data());

    m_extensions.Pick(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
    m_extensions.Pick("VK_KHR_portability_subset");
#endif
#if defined(ENABLE_XR)
    m_extensions.Pick(VK_KHR_MULTIVIEW_EXTENSION_NAME);
    m_extensions.Pick(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME); // VK_KHR_external_memory_fd
    m_extensions.Pick(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME); // VK_EXT_fragment_density_map
#endif
    for(const auto& ext : extensions) {
        m_extensions.Pick(ext);
    }
}

PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
{
    *this = other;
}

PhysicalDevice& PhysicalDevice::operator=(const PhysicalDevice& other)
{
    if(this != &other) {
        m_handle = other.m_handle;
        m_availableProperties = other.m_availableProperties;
        m_availableFeatures = other.m_availableFeatures;
        m_queueFamilies = other.m_queueFamilies;
        m_extensions = other.m_extensions;
        m_enabledFeatures = other.m_enabledFeatures;
        m_physicalDeviceMultiviewFeatures = other.m_physicalDeviceMultiviewFeatures;

        // The copy + move stuff needs to be implemented due to this line :/
        m_enabledFeatures.pNext = &m_physicalDeviceMultiviewFeatures;
    }
    return *this;
}

PhysicalDevice::PhysicalDevice(PhysicalDevice&& other)
{
    *this = other;
    other = {};
}

PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other)
{
    if(this != &other) {
        *this = other;
        other = {};
    }
    return *this;
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
    return m_availableProperties.properties;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::GetAvailableFeatures() const
{
    return m_availableFeatures.features;
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
    return m_enabledFeatures.features;
}

const VkPhysicalDeviceFeatures2& PhysicalDevice::GetEnabledFeatures2() const
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

    const auto vendor{ prev::util::vk::VendorIdToString(m_availableProperties.properties.vendorID) };
    const auto& gpuProps{ GetProperties() };
    LOGI("\t%s %s %s", deviceTypeToString(gpuProps.deviceType).c_str(), vendor.c_str(), gpuProps.deviceName);

    if (showQueues) {
        const auto queueFamilies{ GetQueueFamilies() };
        for (size_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& queueProps{ queueFamilies[i] };
            LOGI("\t\tQueue-family: %zd count: %2d flags: [ %s]", i, queueProps.queueCount, prev::util::vk::QueueFlagsToString(queueProps.queueFlags).c_str());
        }
    }
}
} // namespace prev::core::device
