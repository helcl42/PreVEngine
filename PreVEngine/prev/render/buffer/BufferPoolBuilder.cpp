#include "BufferPoolBuilder.h"

#include "BufferBuilder.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <stdexcept>

namespace prev::render::buffer {
BufferPoolBuilder::BufferPoolBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
{
}

BufferPoolBuilder& BufferPoolBuilder::SetUsageFlags(const GfxBufferUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

BufferPoolBuilder& BufferPoolBuilder::SetHostMapped(const bool hostMapped)
{
    m_hostMapped = hostMapped;
    return *this;
}

BufferPoolBuilder& BufferPoolBuilder::SetCount(const uint64_t count)
{
    m_count = count;
    return *this;
}

BufferPoolBuilder& BufferPoolBuilder::SetStride(const uint64_t stride)
{
    m_stride = stride;
    return *this;
}

BufferPoolBuilder& BufferPoolBuilder::SetAlignment(const uint64_t alignment)
{
    m_alignment = alignment;
    return *this;
}

std::unique_ptr<BufferPool> BufferPoolBuilder::Build() const
{
    Validate();

    const uint64_t alignedItemSize{ prev::util::math::RoundUp(m_stride, m_alignment) };

    std::vector<std::unique_ptr<Buffer>> buffers;
    buffers.reserve(m_count);
    for (uint64_t i = 0; i < m_count; ++i) {
        auto buf = BufferBuilder(m_device, m_queue)
                       .SetUsageFlags(m_usageFlags)
                       .SetHostMapped(m_hostMapped)
                       .SetSize(alignedItemSize)
                       .SetAlignment(m_alignment)
                       .Build();
        buffers.push_back(std::move(buf));
    }

    return std::unique_ptr<BufferPool>(new BufferPool(std::move(buffers)));
}

void BufferPoolBuilder::Validate() const
{
    if (m_usageFlags == 0) {
        throw std::runtime_error("Could not create buffer pool with usage flags 0");
    }

    if (m_count == 0) {
        throw std::runtime_error("Could not create buffer pool with count 0");
    }

    if (m_stride == 0) {
        throw std::runtime_error("Could not create buffer pool with stride 0");
    }
}

} // namespace prev::render::buffer