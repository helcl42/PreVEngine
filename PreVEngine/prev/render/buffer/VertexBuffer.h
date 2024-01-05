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

class HostMappedVertexBuffer final : public VertexBuffer {
public:
    HostMappedVertexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount);

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride) override;

private:
    uint32_t m_maxCount;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif