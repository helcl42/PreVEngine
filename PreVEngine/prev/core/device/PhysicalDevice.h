#ifndef __PHYSICAL_DEVICE_H__
#define __PHYSICAL_DEVICE_H__

#include "../instance/Validation.h"

#include "DeviceExtensions.h"

#include <string>
#include <vector>

namespace prev::core::device {
class PhysicalDevice {
public:
    PhysicalDevice();

    PhysicalDevice(const VkPhysicalDevice gpu, const std::vector<std::string>& extensions = {});

    virtual ~PhysicalDevice() = default;

public:
    PhysicalDevice(const PhysicalDevice& other);

    PhysicalDevice& operator=(const PhysicalDevice& other);

    PhysicalDevice(PhysicalDevice&& other);

    PhysicalDevice& operator=(PhysicalDevice&& other);

public:
    int32_t FindQueueFamily(const VkQueueFlags flags, const VkQueueFlags unwantedFlags = 0, const VkSurfaceKHR surface = VK_NULL_HANDLE) const; // Returns a QueueFamlyIndex, or -1 if none found.

    std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const VkSurfaceKHR surface) const; // Returns list of supported surface formats.

    VkSurfaceFormatKHR FindSurfaceFormat(const VkSurfaceKHR surface, const std::vector<VkFormat>& preferredFormats = { /*VK_FORMAT_R8G8B8A8_SRGB,*/ VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM }) const; // Returns first supported format from given list, or VK_FORMAT_UNDEFINED if no match was found.

    VkFormat FindDepthFormat(const std::vector<VkFormat>& preferredFormats = {
                                 VK_FORMAT_D32_SFLOAT,
                                 VK_FORMAT_D32_SFLOAT_S8_UINT,
                                 VK_FORMAT_D24_UNORM_S8_UINT,
                                 VK_FORMAT_D16_UNORM_S8_UINT,
                                 VK_FORMAT_D16_UNORM }) const; // Returns first supported depth format from list, or VK_FORMAT_UNDEFINED if no match was found.

public:
    const VkPhysicalDeviceProperties& GetProperties() const;

    const VkPhysicalDeviceFeatures& GetAvailableFeatures() const;

    const std::vector<VkQueueFamilyProperties>& GetQueueFamilies() const;

    const DeviceExtensions& GetExtensions() const;

    const VkPhysicalDeviceFeatures& GetEnabledFeatures() const;

    const VkPhysicalDeviceFeatures2& GetEnabledFeatures2() const;

    void Print(const bool showQueues) const;

public:
    operator VkPhysicalDevice() const;

private:
    VkPhysicalDevice m_handle{};

    DeviceExtensions m_extensions{};

    VkPhysicalDeviceProperties2 m_availableProperties{}; // properties and limits

    VkPhysicalDeviceFeatures2 m_availableFeatures{}; // list of available features

    std::vector<VkQueueFamilyProperties> m_queueFamilies; // array of queue families

    VkPhysicalDeviceFeatures2 m_enabledFeatures{};

    VkPhysicalDeviceMultiviewFeatures m_physicalDeviceMultiviewFeatures{};
};
} // namespace prev::core::device

#endif