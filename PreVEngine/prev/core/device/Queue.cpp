#include "Queue.h"

namespace prev::core::device {
Queue::Queue(GfxQueue q, uint32_t f, uint32_t idx, GfxQueueFlags flgs)
    : handle{ q }
    , family{ f }
    , index{ idx }
    , flags{ flgs }
{
}

Queue::operator GfxQueue() const
{
    return handle;
}

GfxResult Queue::Submit(const GfxSubmitDescriptor* submitDesc) const
{
    std::scoped_lock lock{ mutex };
    return gfxQueueSubmit(handle, submitDesc);
}

GfxResult Queue::WaitIdle() const
{
    std::scoped_lock lock{ mutex };
    return gfxQueueWaitIdle(handle);
}
} // namespace prev::core::device