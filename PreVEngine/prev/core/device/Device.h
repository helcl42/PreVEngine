#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "PhysicalDevice.h"
#include "Queue.h"

#include <map>
#include <memory>
#include <vector>

namespace prev::core::device {
class Device {
public:
    Device(const PhysicalDevice& gpu, const VkDevice handle, std::map<QueueType, std::vector<std::unique_ptr<Queue>>>&& queues);

    ~Device();

public:
    void WaitIdle() const;

    bool HasQueue(const QueueType queueType, const uint32_t index = 0) const;

    const Queue& GetQueue(const QueueType queueType, const uint32_t index = 0) const;

    std::vector<QueueType> GetAllQueueTypes() const;

    uint32_t GetQueueTypeCount(const QueueType queueType) const;

    const PhysicalDevice& GetGPU() const;

    void Print() const;

public:
    operator VkDevice() const;

private:
    PhysicalDevice m_gpu{};

    VkDevice m_handle{};

    std::map<QueueType, std::vector<std::unique_ptr<Queue>>> m_queues;
};
} // namespace prev::core::device

#endif
