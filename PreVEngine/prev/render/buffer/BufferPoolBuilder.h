#ifndef __BUFFER_POOL_BUILDER_H__
#define __BUFFER_POOL_BUILDER_H__

#include "BufferPool.h"
#include "FrameScopedBufferPool.h"

#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <memory>

namespace prev::render::buffer {
class BufferPoolBuilder final {
public:
    BufferPoolBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue);

    ~BufferPoolBuilder() = default;

public:
    BufferPoolBuilder& SetUsageFlags(const GfxBufferUsageFlags usageFlags);

    BufferPoolBuilder& SetMemoryProperties(const GfxMemoryPropertyFlags memoryProperties);

    // Ring size for Build(); ignored by BuildFrameScoped().
    BufferPoolBuilder& SetCount(const uint64_t count);

    BufferPoolBuilder& SetStride(const uint64_t stride);

    BufferPoolBuilder& SetAlignment(const uint64_t alignment);

    // Allocation granularity for BuildFrameScoped(): slices carved per backing GPU buffer when a
    // frame's region grows. Not a capacity — the pool grows unboundedly on demand. Ignored by Build().
    BufferPoolBuilder& SetChunkSize(const uint32_t chunkSize);

    // Fixed-size ring of SetCount() slices from one backing buffer. Size it to exceed the slices
    // live at once (e.g. draws/frame * frames-in-flight).
    std::unique_ptr<BufferPool> Build() const;

    // Grow-on-demand pool, one region per frame-in-flight index. Sized by SetChunkSize(); nothing is
    // allocated until the first Next().
    std::unique_ptr<FrameScopedBufferPool> BuildFrameScoped() const;

private:
    void Validate() const;

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxBufferUsageFlags m_usageFlags{};

    GfxMemoryPropertyFlags m_memoryProperties{};

    uint64_t m_count{ 0 };

    uint64_t m_stride{ 0 };

    uint64_t m_alignment{ 8 };

    uint32_t m_chunkSize{ 0 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_BUILDER_H__
