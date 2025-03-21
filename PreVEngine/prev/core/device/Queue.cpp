#include "Queue.h"

namespace prev::core::device {
Queue::Queue(VkQueue q, uint32_t f, uint32_t idx, VkQueueFlags flgs, VkSurfaceKHR surf) noexcept
    : handle{ q }
    , family{ f }
    , index{ idx }
    , flags{ flgs }
    , surface{ surf }
    , mutex{ std::make_unique<std::mutex>() }
{
}

Queue::operator VkQueue() const
{
    return handle;
}

VkResult Queue::Submit(uint32_t submitCount, const VkSubmitInfo* submitInfo, VkFence fence) const
{
    std::scoped_lock lock{ *mutex };

    return vkQueueSubmit(handle, submitCount, submitInfo, fence);
}

VkResult Queue::Present(const VkPresentInfoKHR* presentInfo) const
{
    std::scoped_lock lock{ *mutex };

    return vkQueuePresentKHR(handle, presentInfo);
}

VkResult Queue::WaitIdle() const
{
    std::scoped_lock lock{ *mutex };

    return vkQueueWaitIdle(handle);
}
} // namespace prev::core::device