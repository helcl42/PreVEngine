#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include "Buffer.h"

namespace prev::render::buffer {
class VertexBuffer : public Buffer {
public:
    using Buffer::Buffer;

public:
    virtual void Data(const void* data, const uint32_t count, const uint32_t stride);
};

class HostVisibleVertexBuffer final : public VertexBuffer {
public:
    HostVisibleVertexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCoun);

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride) override;

private:
    uint32_t m_maxCount;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif