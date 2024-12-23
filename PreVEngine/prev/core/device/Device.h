#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "PhysicalDevice.h"
#include "Queue.h"

#include <map>
#include <memory>
#include <set>

namespace prev::core::device {
struct QueueMetadata {
    uint32_t family{}; // queue family

    uint32_t index{}; // queue index

    VkQueueFlags flags{}; // Graphics / Compute / Transfer / Sparse / Protected

    VkSurfaceKHR surface{}; // VK_NULL_HANDLE if queue can not present

    friend bool operator<(const QueueMetadata& a, const QueueMetadata& b);
};

struct QueuesMetadata {
    void Add(const std::vector<QueueType>& queueTypes, const QueueMetadata& queueMetadata);

    std::vector<uint32_t> GetQueueFamilies() const;

    uint32_t GetQueueFamilyCount(const uint32_t family) const;

    std::vector<QueueMetadata> GetAllQueues() const;

    bool HasAny(const QueueType queueType) const;

    std::map<QueueType, std::set<QueueMetadata>> queueGroups;
};

class Device {
public:
    Device(const PhysicalDevice& gpu, const QueuesMetadata& queuesMetadata);

    ~Device();

public:
    void WaitIdle() const;

    bool HasQueue(const QueueType queueType, const uint32_t index = 0) const;

    const Queue& GetQueue(const QueueType queueType, const uint32_t index = 0) const;

    const std::map<QueueType, std::vector<Queue>>& GetAllQueues() const;

    std::vector<QueueType> GetAllQueueTypes() const;

    const PhysicalDevice& GetGPU() const;

    void Print() const;

public:
    operator VkDevice() const;

private:
    PhysicalDevice m_gpu{};

    VkDevice m_handle{};

    std::map<QueueType, std::vector<Queue>> m_queues;
};
} // namespace prev::core::device

#endif
