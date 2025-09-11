#include "BufferPoolBuilder.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <stdexcept>

namespace prev::render::buffer {
BufferPoolBuilder::BufferPoolBuilder(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

BufferPoolBuilder& BufferPoolBuilder::SetUsageFlags(const VkBufferUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

BufferPoolBuilder& BufferPoolBuilder::SetMemoryType(const prev::core::memory::MemoryType memoryType)
{
    m_memoryType = memoryType;
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
    Valiadate();

    const uint64_t alignedItemSize{ prev::util::math::RoundUp(m_stride, m_alignment) };
    const uint64_t alignedFullSize{ alignedItemSize * m_count };

    void* mappedData{};
    void** mappedDataPtr{ m_memoryType == prev::core::memory::MemoryType::HOST_MAPPED ? &mappedData : nullptr };

    VmaAllocation wmaAllocation{ nullptr };
    VkBuffer vkBuffer{ VK_NULL_HANDLE };
    m_allocator.CreateBuffer(nullptr, alignedFullSize, alignedFullSize, m_usageFlags, m_memoryType, vkBuffer, wmaAllocation, mappedDataPtr);
    if (!vkBuffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(alignedFullSize) + " bytes");
    }

    std::vector<std::unique_ptr<Buffer>> buffers;
    buffers.resize(m_count);
    for (uint64_t i = 0; i < m_count; ++i) {
        const uint64_t offset{ i * alignedItemSize };
        // void* mappedPtr{ m_memoryType == prev::core::memory::MemoryType::HOST_MAPPED ? static_cast<uint8_t*>(mappedData) : nullptr };
        VmaAllocation allocation{ i == 0 ? wmaAllocation : nullptr }; // first instance in the pool clears the allocation
        buffers[i] = std::unique_ptr<Buffer>(new Buffer(m_allocator, vkBuffer, allocation, m_memoryType, alignedItemSize, offset, mappedData));
    }

    return std::unique_ptr<BufferPool>(new BufferPool(m_allocator, vkBuffer, wmaAllocation, std::move(buffers)));
}

void BufferPoolBuilder::Valiadate() const
{
    if (m_memoryType == prev::core::memory::MemoryType::UNDEFINED) {
        throw std::runtime_error("Could not create buffer pool with UNDEFINED memory type");
    }

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