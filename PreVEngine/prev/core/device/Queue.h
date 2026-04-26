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
    Queue(GfxQueue q, uint32_t f, uint32_t idx, GfxQueueFlags flgs);

    operator GfxQueue() const;

    GfxResult Submit(const GfxSubmitDescriptor* submitDesc) const;

    GfxResult WaitIdle() const;

    GfxQueue handle{};

    uint32_t family{}; // queue family

    uint32_t index{}; // queue index

    GfxQueueFlags flags{}; // Graphics / Compute / Transfer / Sparse / Protected

    mutable std::mutex mutex;
};
} // namespace prev::core::device

#endif