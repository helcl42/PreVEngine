#ifndef __DEVICES_H__
#define __DEVICES_H__

#include "PhysicalDevice.h"
#include "Queue.h"

#include <map>
#include <memory>
#include <optional>
#include <set>

namespace prev::core::device {
struct QueueMetadata {
    uint32_t family; // queue family

    uint32_t index; // queue index

    VkQueueFlags flags; // Graphics / Compute / Transfer / Sparse / Protected

    VkSurfaceKHR surface; // VK_NULL_HANDLE if queue can not present

    friend bool operator<(const QueueMetadata& a, const QueueMetadata& b);
};

struct QueuesMetadata {
    void Add(const std::vector<QueueType>& queueTypes, const std::vector<QueueMetadata>& metadatas);

    std::vector<uint32_t> GetQueueFamiies() const;

    uint32_t GetQueueFamilyCount(const uint32_t family) const;

    std::vector<QueueMetadata> GetAllQueues() const;

    bool HasAny(const QueueType queueType) const;

    std::map<QueueType, std::set<QueueMetadata>> queueGroups;
};

class Device {
public:
    Device(const std::shared_ptr<PhysicalDevice>& gpu, const QueuesMetadata& queuesMetadata);

    ~Device();

public:
    std::shared_ptr<Queue> GetQueue(const QueueType queueType, const uint32_t index = 0) const;

    std::vector<std::shared_ptr<Queue>> GetQueues(const QueueType queueType) const;

    std::map<QueueType, std::vector<std::shared_ptr<Queue>>> GetAllQueues() const;

    std::vector<QueueType> GetAllQueueTypes() const;

    std::shared_ptr<PhysicalDevice> GetGPU() const;

    void Print() const;

public:
    operator VkDevice() const;

private:
    std::shared_ptr<PhysicalDevice> m_gpu;

    VkDevice m_handle;

    std::map<QueueType, std::vector<std::shared_ptr<Queue>>> m_queues;
};
} // namespace prev::core::device

#endif
