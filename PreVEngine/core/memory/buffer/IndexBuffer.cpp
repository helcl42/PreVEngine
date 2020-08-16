#include "IndexBuffer.h"

namespace prev::core::memory::buffer {
void IndexBuffer::Data(const uint16_t* data, const uint32_t count)
{
    Buffer::Data(data, count, sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    m_indexType = VK_INDEX_TYPE_UINT16;
}

void IndexBuffer::Data(const uint32_t* data, const uint32_t count)
{
    Buffer::Data(data, count, sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    m_indexType = VK_INDEX_TYPE_UINT32;
}

VkIndexType IndexBuffer::GetIndexType() const
{
    return m_indexType;
}
} // namespace prev::core::memory::buffer