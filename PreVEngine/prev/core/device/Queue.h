#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <prev/core/Core.h>

#include <memory>
#include <mutex>

namespace prev::core::device {
enum class QueueType {
    PRESENT,
    GRAPHICS,
    COMPUTE,
    TRANSFER,
    SPARSE,
    PROTECTED
};

struct Queue {
    Queue(VkDevice dev, uint32_t f, uint32_t idx, VkQueueFlags flgs, VkSurfaceKHR surf);

    operator VkQueue() const;

    VkResult Submit(uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const;

    VkResult Present(const VkPresentInfoKHR* presentInfo) const;

    VkResult WaitIdle() const;

    VkDevice device;

    uint32_t family; // queue family

    uint32_t index; // queue index

    VkQueueFlags flags; // Graphics / Compute / Transfer / Sparse / Protected

    VkSurfaceKHR surface; // VK_NULL_HANDLE if queue can not present

    VkQueue handle;

    mutable std::mutex mutex;
};
} // namespace prev::core::device

#endif