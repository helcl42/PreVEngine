#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include "Buffer.h"

namespace PreVEngine {
class VBO : public Buffer {
public:
    using Buffer::Buffer;

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride);
};
} // namespace PreVEngine

#endif