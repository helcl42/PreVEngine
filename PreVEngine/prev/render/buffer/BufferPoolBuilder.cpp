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

std::unique_ptr<BufferPool> BufferPoolBuilder::Build() const
{
    Validate();

    const uint64_t alignedItemSize{ prev::util::math::RoundUp(m_stride, m_alignment) };
    const uint64_t totalSize{ alignedItemSize * m_count };

    auto buffer = BufferBuilder(m_device, m_queue)
                      .SetUsageFlags(m_usageFlags)
                      .SetMemoryProperties(m_memoryProperties)
                      .SetSize(totalSize)
                      .SetAlignment(m_alignment)
                      .Build();

    GfxDevice gfxDevice = static_cast<GfxDevice>(m_device);
    GfxQueue gfxQueue = static_cast<GfxQueue>(m_queue);
    GfxBuffer gfxBuffer = static_cast<GfxBuffer>(*buffer);
    const bool hostMapped = (m_memoryProperties & GFX_MEMORY_PROPERTY_HOST_VISIBLE) != 0;

    std::vector<Buffer> slices;
    slices.reserve(m_count);
    for (uint64_t i = 0; i < m_count; ++i) {
        slices.emplace_back(Buffer(gfxDevice, gfxQueue, gfxBuffer, hostMapped, alignedItemSize, i * alignedItemSize, false));
    }

    return std::unique_ptr<BufferPool>(new BufferPool(std::move(buffer), std::move(slices)));
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