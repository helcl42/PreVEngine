#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "device/PhysicalDevice.h"
#include "instance/Validation.h"

#include <memory>

namespace prev::core {
struct Queue {
    Queue(VkQueue h, uint32_t f, uint32_t idx, VkQueueFlags flgs, VkSurfaceKHR surf, VkDevice dvc, prev::core::device::PhysicalDevice& physDevice);

    VkCommandPool CreateCommandPool() const;

    operator VkQueue() const;

    VkQueue handle;

    uint32_t family; // queue family

    uint32_t index; // queue index

    VkQueueFlags flags; // Graphics / Compute / Transfer / Sparse / Protected

    VkSurfaceKHR surface; // VK_NULL_HANDLE if queue can not present

    VkDevice device; // (used by Swapchain)

    prev::core::device::PhysicalDevice gpu; // (used by Swapchain)
};
} // namespace prev::core

#endif