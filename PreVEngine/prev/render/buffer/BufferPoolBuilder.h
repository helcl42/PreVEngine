#ifndef __BUFFER_POOL_BUILDER_H__
#define __BUFFER_POOL_BUILDER_H__

#include "BufferPool.h"

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

    BufferPoolBuilder& SetHostMapped(bool hostMapped);

    BufferPoolBuilder& SetCount(const uint64_t count);

    BufferPoolBuilder& SetStride(const uint64_t stride);

    BufferPoolBuilder& SetAlignment(const uint64_t alignment);

    std::unique_ptr<BufferPool> Build() const;

private:
    void Validate() const;

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxBufferUsageFlags m_usageFlags{};

    bool m_hostMapped{ true };

    uint64_t m_count{ 0 };

    uint64_t m_stride{ 0 };

    uint64_t m_alignment{ 8 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_BUILDER_H__
