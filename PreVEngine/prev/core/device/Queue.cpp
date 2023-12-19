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

Queue::operator VkQueue() const
{
    return handle;
}

VkResult Queue::Submit(uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const
{
    std::scoped_lock lock{ mutex };

    return vkQueueSubmit(handle, submitCount, submitInfo, fence);
}

VkResult Queue::WaitIdle() const
{
    std::scoped_lock lock{ mutex };

    return vkQueueWaitIdle(handle);
}
} // namespace prev::core::device