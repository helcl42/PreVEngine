#include "Queue.h"

namespace prev::core::device {
Queue::Queue(VkDevice dev, uint32_t f, uint32_t idx, VkQueueFlags flgs, VkSurfaceKHR surf)
    : device{ dev }
    , family{ f }
    , index{ idx }
    , flags{ flgs }
    , surface{ surf }
{
    vkGetDeviceQueue(device, family, index, &handle);
}

VkCommandPool Queue::CreateCommandPool() const
{
    VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    poolInfo.queueFamilyIndex = family;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool;
    VKERRCHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
    return commandPool;
}

Queue::operator VkQueue() const
{
    return handle;
}
} // namespace prev::core::device