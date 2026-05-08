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
    Device(const PhysicalDevice& gpu, GfxDevice handle, std::map<QueueType, std::vector<std::unique_ptr<Queue>>>&& queues, std::vector<std::string> enabledExtensions = {});

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
    operator GfxDevice() const;

    bool HasExtension(const std::string& extension) const;

private:
    PhysicalDevice m_gpu;

    GfxDevice m_handle;

    std::map<QueueType, std::vector<std::unique_ptr<Queue>>> m_queues;

    std::vector<std::string> m_enabledExtensions;
};
} // namespace prev::core::device

#endif
