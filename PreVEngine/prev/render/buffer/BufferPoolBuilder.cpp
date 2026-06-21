#include "BufferPoolBuilder.h"

#include "BufferBuilder.h"

#include "../../util/MathUtils.h"

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

BufferPoolBuilder& BufferPoolBuilder::SetMemoryProperties(const GfxMemoryPropertyFlags memoryProperties)
{
    m_memoryProperties = memoryProperties;
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

BufferPoolBuilder& BufferPoolBuilder::SetChunkSize(const uint32_t chunkSize)
{
    m_chunkSize = chunkSize;
    return *this;
}

std::unique_ptr<BufferPool> BufferPoolBuilder::Build() const
{
    Validate();

    if (m_count == 0) {
        throw std::runtime_error("Could not create buffer pool ring with count 0 (call SetCount)");
    }

    const uint64_t alignedItemSize{ prev::util::math::RoundUp(m_stride, m_alignment) };
    const uint64_t totalSize{ alignedItemSize * m_count };

    auto buffer = BufferBuilder(m_device, m_queue)
                      .SetUsageFlags(m_usageFlags)
                      .SetMemoryProperties(m_memoryProperties)
                      .SetSize(totalSize)
                      .SetAlignment(m_alignment)
                      .Build();

    std::vector<Buffer> slices;
    slices.reserve(m_count);
    for (uint64_t i = 0; i < m_count; ++i) {
        slices.push_back(buffer->Slice(i * alignedItemSize, alignedItemSize));
    }

    return std::unique_ptr<BufferPool>(new BufferPool(std::move(buffer), std::move(slices)));
}

std::unique_ptr<FrameScopedBufferPool> BufferPoolBuilder::BuildFrameScoped() const
{
    Validate();

    if (m_chunkSize == 0) {
        throw std::runtime_error("Could not create frame-scoped buffer pool with chunk size 0 (call SetChunkSize)");
    }

    // No backing is allocated up front — the first Next() allocates a chunk of m_chunkSize slices.
    const uint64_t alignedItemSize{ prev::util::math::RoundUp(m_stride, m_alignment) };

    return std::unique_ptr<FrameScopedBufferPool>(new FrameScopedBufferPool(m_device, m_queue, m_usageFlags,
        m_memoryProperties, alignedItemSize, m_alignment, m_chunkSize));
}

void BufferPoolBuilder::Validate() const
{
    if (m_usageFlags == 0) {
        throw std::runtime_error("Could not create buffer pool with usage flags 0");
    }

    if (m_stride == 0) {
        throw std::runtime_error("Could not create buffer pool with stride 0");
    }
}

} // namespace prev::render::buffer