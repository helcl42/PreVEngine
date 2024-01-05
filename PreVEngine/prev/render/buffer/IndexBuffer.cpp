#include "IndexBuffer.h"

namespace prev::render::buffer {
void IndexBuffer::Data(const uint16_t* data, const uint32_t count)
{
    Buffer::Data(data, count, sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, prev::core::memory::MemoryType::DEVICE_LOCAL);
    m_indexType = VK_INDEX_TYPE_UINT16;
}

void IndexBuffer::Data(const uint32_t* data, const uint32_t count)
{
    Buffer::Data(data, count, sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, prev::core::memory::MemoryType::DEVICE_LOCAL);
    m_indexType = VK_INDEX_TYPE_UINT32;
}

VkIndexType IndexBuffer::GetIndexType() const
{
    return m_indexType;
}

HostMappedIndexBuffer::HostMappedIndexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount)
    : IndexBuffer(allocator)
    , m_maxCount(maxCount)
    , m_mapped(nullptr)
{
}

void HostMappedIndexBuffer::Data(const uint16_t* data, const uint32_t count)
{
    if (!m_mapped) {
        Buffer::Data(nullptr, m_maxCount, sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED, &m_mapped);
    }

    uint32_t finalCount{ std::min(m_maxCount, count) };
    memcpy(m_mapped, data, finalCount * sizeof(uint16_t));

    m_count = finalCount;
    m_indexType = VK_INDEX_TYPE_UINT16;
}

void HostMappedIndexBuffer::Data(const uint32_t* data, const uint32_t count)
{
    if (!m_mapped) {
        Buffer::Data(nullptr, m_maxCount, sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED, &m_mapped);
    }

    uint32_t finalCount{ std::min(m_maxCount, count) };
    memcpy(m_mapped, data, finalCount * sizeof(uint32_t));

    m_count = finalCount;
    m_indexType = VK_INDEX_TYPE_UINT32;
}
} // namespace prev::render::buffer