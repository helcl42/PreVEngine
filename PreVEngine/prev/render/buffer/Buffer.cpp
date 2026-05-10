#include "Buffer.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace prev::render::buffer {
Buffer::Buffer(GfxDevice device, GfxQueue queue, GfxBuffer buffer, bool hostMapped, uint64_t size)
    : m_device{ device }
    , m_queue{ queue }
    , m_buffer{ buffer }
    , m_hostMapped{ hostMapped }
    , m_size{ size }
{
}

Buffer::~Buffer()
{
    if (m_buffer) {
        gfxQueueWaitIdle(m_queue);
        gfxBufferDestroy(m_buffer);
    }
}

void Buffer::Write(const void* data, const uint64_t size, const uint64_t offset)
{
    if (size > m_size) {
        throw std::runtime_error("Attempted to write " + std::to_string(size) + " bytes, which is out of range for the buffer of size " + std::to_string(m_size) + ".");
    }

    if (data) {
        gfxQueueWriteBuffer(m_queue, m_buffer, offset, data, size);
    } else {
        std::vector<uint8_t> zeros(size, 0);
        gfxQueueWriteBuffer(m_queue, m_buffer, offset, zeros.data(), size);
    }
}

void Buffer::Clear()
{
    std::vector<uint8_t> zeros(m_size, 0);
    gfxQueueWriteBuffer(m_queue, m_buffer, 0, zeros.data(), m_size);
}

uint64_t Buffer::GetSize() const
{
    return m_size;
}

Buffer::operator GfxBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer