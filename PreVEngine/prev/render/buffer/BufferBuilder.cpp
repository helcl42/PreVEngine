#include "BufferBuilder.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

#include <stdexcept>

namespace prev::render::buffer {
BufferBuilder::BufferBuilder(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

BufferBuilder& BufferBuilder::SetUsageFlags(const VkBufferUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

BufferBuilder& BufferBuilder::SetMemoryType(const prev::core::memory::MemoryType memoryType)
{
    m_memoryType = memoryType;
    return *this;
}

BufferBuilder& BufferBuilder::SetSize(const uint64_t size)
{
    m_size = size;
    return *this;
}

BufferBuilder& BufferBuilder::SetAlignment(const uint64_t alignment)
{
    m_alignment = alignment;
    return *this;
}

BufferBuilder& BufferBuilder::SetData(const void* data, const uint64_t size)
{
    m_data = data;
    m_dataSize = size;
    return *this;
}

std::unique_ptr<Buffer> BufferBuilder::Build() const
{
    Valiadate();

    const uint64_t alignedSize{ prev::util::math::RoundUp(m_size, m_alignment) };

    void* mappedData{};
    void** mappedDataPtr{ m_memoryType == prev::core::memory::MemoryType::HOST_MAPPED ? &mappedData : nullptr };

    VmaAllocation allocation{ nullptr };
    VkBuffer buffer{ VK_NULL_HANDLE };
    m_allocator.CreateBuffer(m_data, m_dataSize, alignedSize, m_usageFlags, m_memoryType, buffer, allocation, mappedDataPtr);
    if (!buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(alignedSize) + " bytes");
    }

    return std::unique_ptr<Buffer>(new Buffer(m_allocator, buffer, allocation, m_memoryType, alignedSize, 0, mappedData));
}

void BufferBuilder::Valiadate() const
{
    if (m_memoryType == prev::core::memory::MemoryType::UNDEFINED) {
        throw std::runtime_error("Could not create buffer with UNDEFINED memory type");
    }

    if (m_usageFlags == 0) {
        throw std::runtime_error("Could not create buffer with usage flags 0");
    }

    if (m_size == 0) {
        throw std::runtime_error("Could not create buffer with size 0");
    }
}
} // namespace prev::render::buffer