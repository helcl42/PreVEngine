#include "Buffer.h"

#include <stdexcept>
#include <string>

namespace prev::render::buffer {
Buffer::Buffer(prev::core::memory::Allocator& allocator)
    : m_allocator(allocator)
    , m_allocation(nullptr)
    , m_buffer(VK_NULL_HANDLE)
    , m_count(0)
    , m_stride(0)
    , m_offset(0)
    , m_mapped(nullptr)
{
}

Buffer::~Buffer()
{
    Clear();
}

void Buffer::Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlags usage, const prev::core::memory::MemoryType memoryType)
{
    Clear();

    void** mapped{ memoryType == prev::core::memory::MemoryType::HOST_MAPPED ? &m_mapped : nullptr };

    m_allocator.CreateBuffer(data, count * stride, usage, memoryType, m_buffer, m_allocation, mapped);

    if (!m_buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(count * stride));
    }

    m_count = count;
    m_stride = stride;
    m_offset = 0;
}

void Buffer::Clear()
{
    if (m_buffer && m_allocation) {
        m_allocator.GetQueue().WaitIdle();

        m_allocator.DestroyBuffer(m_buffer, m_allocation);
    }

    m_buffer = VK_NULL_HANDLE;
    m_count = 0;
    m_stride = 0;
    m_offset = 0;
}

uint32_t Buffer::GetCount() const
{
    return m_count;
}

uint32_t Buffer::GetSize() const
{
    return m_count * m_stride;
}

uint32_t Buffer::GetOffset() const
{
    return m_offset;
}

void* Buffer::GetMappedPtr() const
{
    return m_mapped;
}

Buffer::operator VkBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer