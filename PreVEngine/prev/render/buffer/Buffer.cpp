#include "Buffer.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace prev::render::buffer {
Buffer::Buffer(GfxDevice device, GfxQueue queue, GfxBuffer buffer, bool hostMapped, uint64_t size, void* mappedPtr)
    : m_device{ device }
    , m_queue{ queue }
    , m_buffer{ buffer }
    , m_hostMapped{ hostMapped }
    , m_size{ size }
    , m_mappedPtr{ mappedPtr }
{
}

Buffer::~Buffer()
{
    if (m_buffer) {
        gfxQueueWaitIdle(m_queue);
        if (m_mappedPtr) {
            gfxBufferUnmap(m_buffer);
        }
        gfxBufferDestroy(m_buffer);
    }
}

void Buffer::Write(const void* data, const uint64_t size, const uint64_t offset)
{
    if (size > m_size) {
        throw std::runtime_error("Attempted to write " + std::to_string(size) + " bytes, which is out of range for the buffer of size " + std::to_string(m_size) + ".");
    }

    if (m_mappedPtr) {
        uint8_t* dst = static_cast<uint8_t*>(m_mappedPtr) + offset;
        if (data) {
            std::memcpy(dst, data, size);
        } else {
            std::memset(dst, 0, size);
        }
        gfxBufferFlushMappedRange(m_buffer, offset, size);
    } else {
        if (data) {
            gfxQueueWriteBuffer(m_queue, m_buffer, offset, data, size);
        }
    }
}

void Buffer::Clear()
{
    if (m_mappedPtr) {
        std::memset(m_mappedPtr, 0, m_size);
        gfxBufferFlushMappedRange(m_buffer, 0, m_size);
    } else {
        std::vector<uint8_t> zeros(m_size, 0);
        gfxQueueWriteBuffer(m_queue, m_buffer, 0, zeros.data(), m_size);
    }
}

uint64_t Buffer::GetSize() const
{
    return m_size;
}

void* Buffer::GetMappedPtr() const
{
    return m_mappedPtr;
}

Buffer::operator GfxBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer