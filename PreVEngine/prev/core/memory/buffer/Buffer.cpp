#include "Buffer.h"

#include <stdexcept>
#include <string>

namespace prev::core::memory::buffer {
Buffer::Buffer(Allocator& allocator)
    : m_allocator(allocator)
    , m_allocation(nullptr)
    , m_buffer(VK_NULL_HANDLE)
    , m_count(0)
    , m_stride(0)
{
}

Buffer::~Buffer()
{
    Clear();
}

void Buffer::Clear()
{
    vkQueueWaitIdle(*m_allocator.GetQueue());

    if (m_buffer) {
        m_allocator.DestroyBuffer(m_buffer, m_allocation);
    }

    m_buffer = VK_NULL_HANDLE;
    m_count = 0;
    m_stride = 0;
}

void Buffer::Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlags usage, const VmaMemoryUsage memtype, void** mapped)
{
    Clear();

    m_allocator.CreateBuffer(data, count * stride, usage, memtype, m_buffer, m_allocation, mapped);

    if (!m_buffer) {
        throw std::runtime_error("Could not allocate buffer: size = " + std::to_string(count * stride));
    }

    m_count = count;
    m_stride = stride;
}

uint32_t Buffer::GetCount() const
{
    return m_count;
}

uint32_t Buffer::GetSize() const
{
    return m_count * m_stride;
}

Buffer::operator VkBuffer() const
{
    return m_buffer;
}
} // namespace prev::core::memory::buffer