#ifndef __DEVICE_FACTORY_H__
#define __DEVICE_FACTORY_H__

#include "PhysicalDevice.h"

#include <memory>

namespace prev::core::device {
class Device;
struct QueueMetadata;

class DeviceFactory {
public:
    DeviceFactory() = default;

    ~DeviceFactory() = default;

public:
    std::shared_ptr<Device> Create(const std::shared_ptr<PhysicalDevice>& gpu, VkSurfaceKHR surface) const;

private:
    uint32_t FamilyQueueCount(const std::vector<QueueMetadata>& queues, const uint32_t family) const;

    bool FindQueue(const std::shared_ptr<PhysicalDevice>& gpu, VkQueueFlags flags, VkQueueFlags unwantedFlags, VkSurfaceKHR surface, const std::vector<QueueMetadata>& addedQueues, QueueMetadata& outQueueMeta) const;
};
} // namespace prev::core::device

#endif // !__DEVICE_FACTORY_H__
