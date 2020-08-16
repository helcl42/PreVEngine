#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include "Buffer.h"

namespace prev::core::memory::buffer {
class VBO final : public Buffer {
public:
    using Buffer::Buffer;

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride);
};
} // namespace prev::core::memory::buffer

#endif