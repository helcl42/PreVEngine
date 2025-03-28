#include "VertexBuffer.h"

namespace prev::render::buffer {
void VertexBuffer::Data(const void* data, const uint32_t count, const uint32_t stride)
{
    Buffer::Data(data, count, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, prev::core::memory::MemoryType::DEVICE_LOCAL);
}

HostMappedVertexBuffer::HostMappedVertexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount)
    : VertexBuffer(allocator)
    , m_maxCount(maxCount)
{
}

void HostMappedVertexBuffer::Data(const void* data, const uint32_t count, const uint32_t stride)
{
    if (!m_mapped) {
        Buffer::Data(nullptr, m_maxCount, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED);
    }

    const uint32_t finalCount{ std::min(m_maxCount, count) };
    memcpy(m_mapped, data, finalCount * stride);

    m_count = finalCount;
}
} // namespace prev::render::buffer