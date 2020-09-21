#ifndef __VERTEX_DATA_BUFFER_H__
#define __VERTEX_DATA_BUFFER_H__

#include <common/Common.h>

#include <vector>

namespace prev_test::render {
class VertexDataBuffer final {
public:
    VertexDataBuffer() = default;

    VertexDataBuffer(const size_t desiredSizeInBytes);

    ~VertexDataBuffer() = default;

public:
    void Add(const void* data, const unsigned int size);

    void Add(const float data);

    void Add(const glm::vec2& data);

    void Add(const glm::vec3& data);

    void Add(const glm::vec4& data);

    void Add(const glm::mat3& data);

    void Add(const glm::mat4& data);

    void Reset();

    const uint8_t* GetData() const;

private:
    std::vector<uint8_t> m_buffer;
};
} // namespace prev_test::render

#endif // !__VERTEX_DATA_BUFFER_H__
