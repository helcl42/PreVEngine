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

    friend bool operator<(const QueueMetadata& a, const QueueMetadata& b)
    {
        if (a.family < b.family && a.index < b.index) {
            return true;
        }
        return false;
    }
};

struct QueueMetadataStorage {
    std::map<QueueType, std::set<QueueMetadata> > queueGroups;

    std::set<QueueMetadata> uniqueQueue;

    void Add(const std::initializer_list<QueueType>& queueTypes, const std::initializer_list<QueueMetadata>& metadatas)
    {
        for (const auto& qt : queueTypes) {
            auto& group{ queueGroups[qt] };
            group.insert(metadatas.begin(), metadatas.end());
        }

        uniqueQueue.insert(metadatas.begin(), metadatas.end());
    }

    std::set<uint32_t> GetDistinctQueueFamiies() const
    {
        std::set<uint32_t> queueFamilies;
        for (const auto& group : queueGroups) {
            for (const auto& item : group.second) {
                queueFamilies.insert(item.family);
            }
        }
        return queueFamilies;
    }

    uint32_t GetQueueFamilyCount(const uint32_t familly) const
    {
        uint32_t count{ 0 };
        for (const auto& item : uniqueQueue) {
            if (item.family == familly) {
                count++;
            }
        }
        return count;
    }

    std::vector<QueueMetadata> GetAllQueues() const
    {
        std::vector<QueueMetadata> result;
        for (const auto& group : queueGroups) {
            result.insert(result.end(), group.second.cbegin(), group.second.cend());
        }
        return result;
    }

    bool HasAny(const QueueType queueType) const
    {
        return queueGroups.find(queueType) != queueGroups.cend();
    }
};

class Device {
public:
    Device(const std::shared_ptr<PhysicalDevice>& gpu, const QueueMetadataStorage& queuesMetadata);

    ~Device();

public:
    std::shared_ptr<Queue> GetQueue(const QueueType queueType, const uint32_t index = 0) const;

    std::shared_ptr<PhysicalDevice> GetGPU() const;

    void Print() const;

public:
    operator VkDevice() const;

private:
    std::shared_ptr<PhysicalDevice> m_gpu;

    VkDevice m_handle;

    std::map<QueueType, std::vector<std::shared_ptr<Queue> > > m_queues;
};
} // namespace prev::core::device

#endif
