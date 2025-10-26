#include "VertexDataBuffer.h"

namespace prev_test::render {
VertexDataBuffer::VertexDataBuffer(const size_t desiredSizeInBytes)
{
    m_buffer.reserve(desiredSizeInBytes);
}

void VertexDataBuffer::Add(const void* data, const size_t size)
{
    m_buffer.insert(m_buffer.end(), static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
}

void VertexDataBuffer::Reset()
{
    m_buffer.clear();
}

const uint8_t* VertexDataBuffer::GetData() const
{
    return m_buffer.data();
}

size_t VertexDataBuffer::GetSize() const
{
    return m_buffer.size();
}
} // namespace prev_test::render