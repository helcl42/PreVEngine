#include "PhysicalDevice.h"

namespace PreVEngine {
PhysicalDevice::PhysicalDevice()
    : m_handle(VK_NULL_HANDLE)
    , m_availableProperties()
    , m_availableFeatures()
    , m_extensions()
{
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice gpu)
    : m_handle(gpu)
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

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
    m_queueFamilies.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, m_queueFamilies.data());

    m_extensions.Init(gpu);
    m_extensions.Pick(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

std::string PhysicalDevice::GetVendorName() const
{
    struct
    {
        const uint32_t id;
        const char* name;
    } vendors[] = {
        { 0x1002, "AMD" },
        { 0x10DE, "NVIDIA" },
        { 0x8086, "INTEL" },
        { 0x13B5, "ARM" },
        { 0x5143, "Qualcomm" },
        { 0x1010, "Imagination" }
    };

    for (auto vendor : vendors) {
        if (vendor.id == m_availableProperties.vendorID) {
            return vendor.name;
        }
    }
    return "UNKNOWN";
}

// Find queue-family with requred flags, and can present to given surface. (if provided)
// Returns the QueueFamily index, or -1 if not found.
int PhysicalDevice::FindQueueFamily(VkQueueFlags flags, VkSurfaceKHR surface) const
{
    for (size_t i = 0; i < m_queueFamilies.size(); i++) {
        if ((m_queueFamilies[i].queueFlags & flags) != flags) {
            continue;
        }

        if (surface != VK_NULL_HANDLE) {
            VkBool32 canPresent;
            VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, static_cast<uint32_t>(i), surface, &canPresent));
            if (canPresent == VK_FALSE) {
                continue;
            }
        }

        return static_cast<int>(i);
    }

    return -1;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::SurfaceFormats(VkSurfaceKHR surface) const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, nullptr);

    ASSERT(!!count, "No supported surface formats found.");

    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, formats.data());

    return formats;
}

//--Returns the first supported surface color format from the preferredFormats list, or VK_FORMAT_UNDEFINED if no match found.
VkFormat PhysicalDevice::FindSurfaceFormat(VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats) const
{
    auto formats = SurfaceFormats(surface); // get list of supported surface formats
    for (auto& pf : preferredFormats) {
        for (auto& f : formats) {
            if (f.format == pf) {
                return f.format;
            }
        }
    }

    //return formats[0].format;  //first supported format
    return VK_FORMAT_UNDEFINED;
}

VkFormat PhysicalDevice::FindDepthFormat(const std::vector<VkFormat>& preferredFormats) const
{
    for (auto& format : preferredFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(m_handle, format, &formatProps);

        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
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

const std::vector<VkQueueFamilyProperties> PhysicalDevice::GetQueueFamilies() const
{
    return m_queueFamilies;
}

DeviceExtensions& PhysicalDevice::GetExtensions()
{
    return m_extensions;
}

VkPhysicalDeviceFeatures& PhysicalDevice::GetEnabledFeatures()
{
    return m_enabledFeatures;
}

PhysicalDevice::operator VkPhysicalDevice() const
{
    return m_handle;
}
} // namespace PreVEngine