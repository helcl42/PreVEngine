#include "UniformBuffer.h"

namespace prev::core::memory::buffer {
UBO::UBO(VkBuffer buffer, void* data, const uint32_t offset, const uint32_t range)
    : m_buffer(buffer)
    , m_mapped(data)
    , m_offset(offset)
    , m_range(range)
{
}

void UBO::Update(const void* data)
{
    memcpy(static_cast<uint8_t*>(m_mapped) + m_offset, data, m_range);
}

uint32_t UBO::GetOffset() const
{
    return m_offset;
}

uint32_t UBO::GetRange() const
{
    return m_range;
}

UBO::operator VkBuffer() const
{
    return m_buffer;
}
} // namespace prev::core::memory::buffer