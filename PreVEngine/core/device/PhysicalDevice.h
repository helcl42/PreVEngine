#ifndef __PHYSICAL_DEVICE_H__
#define __PHYSICAL_DEVICE_H__

#include "../instance/Validation.h"

#include "DeviceExtensions.h"

#include <string>
#include <vector>

namespace prev {
class PhysicalDevice {
public:
    PhysicalDevice();

    PhysicalDevice(VkPhysicalDevice gpu);

    virtual ~PhysicalDevice() = default;

public:
    std::string GetVendorName() const;

    int FindQueueFamily(VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE) const; // Returns a QueueFamlyIndex, or -1 if none found.

    std::vector<VkSurfaceFormatKHR> SurfaceFormats(VkSurfaceKHR surface) const; // Returns list of supported surface formats.

    VkFormat FindSurfaceFormat(VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats = { VK_FORMAT_B8G8R8A8_UNORM, // Returns first supported format from given list, or VK_FORMAT_UNDEFINED if no match was found.
                                                         VK_FORMAT_R8G8B8A8_UNORM }) const;

    VkFormat FindDepthFormat(const std::vector<VkFormat>& preferredFormats = {
                                 VK_FORMAT_D32_SFLOAT, // Returns first supported depth format from list,
                                 VK_FORMAT_D32_SFLOAT_S8_UINT, // or VK_FORMAT_UNDEFINED if no match was found.
                                 VK_FORMAT_D24_UNORM_S8_UINT,
                                 VK_FORMAT_D16_UNORM_S8_UINT,
                                 VK_FORMAT_D16_UNORM }) const;

public:
    const VkPhysicalDeviceProperties& GetProperties() const;

    const VkPhysicalDeviceFeatures& GetAvailableFeatures() const;

    const std::vector<VkQueueFamilyProperties> GetQueueFamilies() const;

    DeviceExtensions& GetExtensions();

    VkPhysicalDeviceFeatures& GetEnabledFeatures();

public:
    operator VkPhysicalDevice() const;

private:
    VkPhysicalDevice m_handle;

    VkPhysicalDeviceProperties m_availableProperties; // properties and limits

    VkPhysicalDeviceFeatures m_availableFeatures; // list of available features

    std::vector<VkQueueFamilyProperties> m_queueFamilies; // array of queue families

    DeviceExtensions m_extensions; // picklist: select extensions to load (Defaults to "VK_KHR_swapchain" only.)

    VkPhysicalDeviceFeatures m_enabledFeatures = {}; // Set required features.   TODO: finish this.
};
} // namespace prev

#endif