#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <prev/core/instance/Validation.h>

#include <memory>

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

    VkCommandPool CreateCommandPool() const;

    operator VkQueue() const;

    VkDevice device;

    uint32_t family; // queue family

    uint32_t index; // queue index

    VkQueueFlags flags; // Graphics / Compute / Transfer / Sparse / Protected

    VkSurfaceKHR surface; // VK_NULL_HANDLE if queue can not present

    VkQueue handle;
};
} // namespace prev::core::device

#endif