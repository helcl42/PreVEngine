#include "VertexBuffer.h"

namespace prev::render::buffer {
void VertexBuffer::Data(const void* data, const uint32_t count, const uint32_t stride)
{
    Buffer::Data(data, count, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

HostVisibleVertexBuffer::HostVisibleVertexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount, const uint32_t stride)
    : Buffer(allocator)
    , m_maxCount(maxCount)
    , m_stride(stride)
    , m_mapped(nullptr)
{
}

void HostVisibleVertexBuffer::Data(const void* data, const uint32_t count)
{
    if (!m_mapped) {
        Buffer::Data(nullptr, m_maxCount, m_stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);
    }

    uint32_t finalCount{ std::min(m_maxCount, count) };
    memcpy(m_mapped, data, finalCount * m_stride);
}
} // namespace prev::render::buffer