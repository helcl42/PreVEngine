#ifndef __VERTEX_DATA_BUFFER_H__
#define __VERTEX_DATA_BUFFER_H__

#include <prev/common/Common.h>

#include <vector>

namespace prev_test::render {
class VertexDataBuffer final {
public:
    VertexDataBuffer() = default;

    VertexDataBuffer(const size_t desiredSizeInBytes);

    ~VertexDataBuffer() = default;

public:
    void Add(const void* data, const size_t size);

    template <typename Type>
    void Add(const Type& data)
    {
        Add(&data, sizeof(Type));
    }

    void Reset();

    const uint8_t* GetData() const;

    size_t GetSize() const;

private:
    std::vector<uint8_t> m_buffer;
};
} // namespace prev_test::render

#endif // !__VERTEX_DATA_BUFFER_H__
