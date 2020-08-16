#ifndef __DEVICES_H__
#define __DEVICES_H__

#include "PhysicalDevice.h"

#include "../Queue.h"

#include <memory>

namespace prev::core::device {
class Device {
public:
    Device(PhysicalDevice& gpu);

    ~Device();

public:
    std::shared_ptr<Queue> AddQueue(VkQueueFlags flags, VkSurfaceKHR surface = 0);

    PhysicalDevice& GetGPU();

    void Print() const;

public:
    operator VkDevice();

private:
    uint32_t FamilyQueueCount(uint32_t family) const;

    void Create();

    void Destroy();

private:
    VkDevice m_handle;

    PhysicalDevice m_gpu;

    std::vector<std::shared_ptr<Queue> > m_queues;
};
} // namespace prev::core::device

#endif
