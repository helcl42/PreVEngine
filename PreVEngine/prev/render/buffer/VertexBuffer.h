#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include "Buffer.h"

namespace prev::render::buffer {
class VertexBuffer final : public Buffer {
public:
    using Buffer::Buffer;

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride);
};

class HostVisibleVertexBuffer final : public Buffer {
public:
    HostVisibleVertexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount, const uint32_t stride);

public:
    void Data(const void* data, const uint32_t count);

private:
    uint32_t m_maxCount;

    uint32_t m_stride;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif