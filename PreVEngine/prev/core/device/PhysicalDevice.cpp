#include "PhysicalDevice.h"

#include "../../util/MathUtils.h"
#include "../../util/VkUtils.h"

namespace prev::core::device {
namespace {
    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(const VkPhysicalDevice gpu)
    {
        uint32_t count{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, queueFamilyProperties.data());
        return queueFamilyProperties;
    }

    static constexpr VkFormat DEFAULT_COLOR_FORMAT{ VK_FORMAT_R8G8B8A8_SNORM };

    static constexpr VkFormat DEFAULT_DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };
} // namespace

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice gpu, const std::vector<std::string>& extensions)
    : m_handle{ gpu }
    , m_extensions{ gpu }
    , m_queueFamilies{ GetQueueFamilyProperties(gpu) }
    , m_availableProperties{ prev::util::vk::CreateStruct<VkPhysicalDeviceProperties2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2) }
    , m_availableFeatures{ prev::util::vk::CreateStruct<VkPhysicalDeviceFeatures2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) }
    , m_enabledFeatures{ prev::util::vk::CreateStruct<VkPhysicalDeviceFeatures2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) }
    , m_physicalDeviceMultiviewFeatures{ prev::util::vk::CreateStruct<VkPhysicalDeviceMultiviewFeatures>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR) }
{
    vkGetPhysicalDeviceFeatures2(gpu, &m_availableFeatures);
    vkGetPhysicalDeviceProperties2(gpu, &m_availableProperties);

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
    EnableMultiview();
#endif

    m_extensions.Pick(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_IOS_MVK)
    m_extensions.Pick("VK_KHR_portability_subset");
#endif
#ifdef ENABLE_XR
    m_extensions.Pick(VK_KHR_MULTIVIEW_EXTENSION_NAME);
    m_extensions.Pick(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME); // VK_KHR_external_memory_fd
    m_extensions.Pick(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME); // VK_EXT_fragment_density_map
#endif
    for (const auto& ext : extensions) {
        m_extensions.Pick(ext);
    }
}

PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
{
    *this = other;
}

PhysicalDevice& PhysicalDevice::operator=(const PhysicalDevice& other)
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_availableProperties = other.m_availableProperties;
        m_availableFeatures = other.m_availableFeatures;
        m_queueFamilies = other.m_queueFamilies;
        m_extensions = other.m_extensions;
        m_enabledFeatures = other.m_enabledFeatures;
        m_physicalDeviceMultiviewFeatures = other.m_physicalDeviceMultiviewFeatures;
#ifdef ENABLE_XR
        EnableMultiview();
#endif
    }
    return *this;
}

PhysicalDevice::PhysicalDevice(PhysicalDevice&& other)
{
    *this = other;
}

PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other)
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_availableProperties = other.m_availableProperties;
        m_availableFeatures = other.m_availableFeatures;
        m_queueFamilies = other.m_queueFamilies;
        m_extensions = other.m_extensions;
        m_enabledFeatures = other.m_enabledFeatures;
        m_physicalDeviceMultiviewFeatures = other.m_physicalDeviceMultiviewFeatures;
#ifdef ENABLE_XR
        EnableMultiview();
#endif
        other.m_handle = VK_NULL_HANDLE;
        other.m_availableProperties = {};
        other.m_availableFeatures = {};
        other.m_queueFamilies = {};
        other.m_extensions = {};
        other.m_enabledFeatures = {};
        other.m_physicalDeviceMultiviewFeatures = {};
    }
    return *this;
}

// Find queue-family with requred flags, and can present to given surface. (if provided)
// Returns the QueueFamily index, or -1 if not found.
int32_t PhysicalDevice::FindQueueFamily(const VkQueueFlags flags, const VkQueueFlags unwantedFlags, const VkSurfaceKHR surface) const
{
    for (size_t i = 0; i < m_queueFamilies.size(); ++i) {
        const auto& queueFamily{ m_queueFamilies[i] };
        if (!prev::util::math::HasAllFlagsSet(queueFamily.queueFlags, flags)) {
            continue;
        }

        if (prev::util::math::HasAnyFlagsSet(queueFamily.queueFlags, unwantedFlags)) {
            continue;
        }

        if (surface != VK_NULL_HANDLE) {
            VkBool32 canPresent{};
            VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, static_cast<uint32_t>(i), surface, &canPresent));
            if (canPresent == VK_FALSE) {
                continue;
            }
        }

        return static_cast<int32_t>(i);
    }
    return -1;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::GetSurfaceFormats(const VkSurfaceKHR surface) const
{
    if (surface == VK_NULL_HANDLE) {
        return {};
    }

    uint32_t count{ 0 };
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, formats.data());
    return formats;
}

// Returns the first supported surface color format from the preferredFormats list, or VK_FORMAT_UNDEFINED if no match found.
VkSurfaceFormatKHR PhysicalDevice::FindSurfaceFormat(const VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats) const
{
    const auto formats{ GetSurfaceFormats(surface) }; // get list of supported surface formats
    for (const auto& preferedFormat : preferredFormats) {
        for (const auto& format : formats) {
            if (format.format == preferedFormat) {
                return format;
            }
        }
    }
    return { DEFAULT_COLOR_FORMAT, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
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
    return DEFAULT_DEPTH_FORMAT;
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

void PhysicalDevice::Print() const
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

    const auto queueFamilies{ GetQueueFamilies() };
    for (size_t i = 0; i < queueFamilies.size(); ++i) {
        const auto& queueProps{ queueFamilies[i] };
        LOGI("\t\tQueue-family: %zu count: %2d flags: [ %s]", i, queueProps.queueCount, prev::util::vk::QueueFlagsToString(queueProps.queueFlags).c_str());
    }
}

PhysicalDevice::operator VkPhysicalDevice() const
{
    return m_handle;
}

void PhysicalDevice::EnableMultiview()
{
    m_physicalDeviceMultiviewFeatures = { prev::util::vk::CreateStruct<VkPhysicalDeviceMultiviewFeatures>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR) };
    m_physicalDeviceMultiviewFeatures.multiview = VK_TRUE;

    m_enabledFeatures.pNext = &m_physicalDeviceMultiviewFeatures;
}
} // namespace prev::core::device
