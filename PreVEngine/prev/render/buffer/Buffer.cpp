#include "Buffer.h"

#include <stdexcept>
#include <string>

namespace prev::render::buffer {
Buffer::Buffer(prev::core::memory::Allocator& allocator, VkBuffer buffer, VmaAllocation allocation, prev::core::memory::MemoryType memoryType, uint64_t size, uint64_t offset, void* mappedPtr)
    : m_allocator{ allocator }
    , m_buffer{ buffer }
    , m_allocation{ allocation }
    , m_memoryType{ memoryType }
    , m_size{ size }
    , m_offset{ offset }
    , m_mappedPtr{ mappedPtr }
{
}

Buffer::~Buffer()
{
    if (m_buffer && m_allocation) {
        m_allocator.GetQueue().WaitIdle();

        m_allocator.DestroyBuffer(m_buffer, m_allocation);
    }
}

void Buffer::Write(const void* data, const uint64_t size, const uint64_t offset)
{
    if (m_memoryType != prev::core::memory::MemoryType::HOST_MAPPED) {
        throw std::runtime_error("Could not write a non HOST_MAPPED buffer");
    }

    if (size > m_size) {
        throw std::runtime_error("Attempted to write " + std::to_string(size) + " bytes, which is out of range for the buffer of size " + std::to_string(m_size) + ".");
    }

    uint8_t* dst = static_cast<uint8_t*>(m_mappedPtr) + offset;
    if (data) {
        std::memcpy(dst, data, size);
    } else {
        std::memset(dst, 0, size);
    }
}

void Buffer::Clear()
{
    if (m_memoryType != prev::core::memory::MemoryType::HOST_MAPPED) {
        throw std::runtime_error("Could not clear a non HOST_MAPPED buffer");
    }

    std::memset(m_mappedPtr, 0, m_size);
}

uint64_t Buffer::GetSize() const
{
    return m_size;
}

uint64_t Buffer::GetOffset() const
{
    return m_offset;
}

void* Buffer::GetMappedPtr() const
{
    return m_mappedPtr;
}

Buffer::operator VkBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer