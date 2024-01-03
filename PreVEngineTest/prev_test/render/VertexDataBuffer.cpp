#include "VertexDataBuffer.h"

namespace prev_test::render {
VertexDataBuffer::VertexDataBuffer(const size_t desiredSizeInBytes)
{
    m_buffer.reserve(desiredSizeInBytes);
}

void VertexDataBuffer::Add(const void* data, const unsigned int size)
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
} // namespace prev_test::render