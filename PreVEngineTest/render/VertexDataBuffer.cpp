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

void VertexDataBuffer::Add(const float data)
{
    Add(&data, sizeof(float));
}

void VertexDataBuffer::Add(const glm::vec2& data)
{
    Add(&data, sizeof(glm::vec2));
}

void VertexDataBuffer::Add(const glm::vec3& data)
{
    Add(&data, sizeof(glm::vec3));
}

void VertexDataBuffer::Add(const glm::vec4& data)
{
    Add(&data, sizeof(glm::vec4));
}

void VertexDataBuffer::Add(const glm::mat3& data)
{
    Add(&data, sizeof(glm::mat3));
}

void VertexDataBuffer::Add(const glm::mat4& data)
{
    Add(&data, sizeof(glm::mat4));
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