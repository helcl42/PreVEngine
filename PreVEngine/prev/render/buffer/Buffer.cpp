#include "Buffer.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace prev::render::buffer {
Buffer::Buffer(GfxDevice device, GfxQueue queue, GfxBuffer buffer, bool hostMapped, uint64_t size, uint64_t offset, bool owning)
    : m_device{ device }
    , m_queue{ queue }
    , m_buffer{ buffer }
    , m_hostMapped{ hostMapped }
    , m_size{ size }
    , m_offset{ offset }
    , m_owning{ owning }
{
}

Buffer::~Buffer()
{
    if (m_owning && m_buffer) {
        gfxQueueWaitIdle(m_queue);
        gfxBufferDestroy(m_buffer);
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_device{ other.m_device }
    , m_queue{ other.m_queue }
    , m_buffer{ other.m_buffer }
    , m_hostMapped{ other.m_hostMapped }
    , m_size{ other.m_size }
    , m_offset{ other.m_offset }
    , m_owning{ other.m_owning }
{
    other.m_buffer = {};
    other.m_owning = false;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if (this != &other) {
        if (m_owning && m_buffer) {
            gfxQueueWaitIdle(m_queue);
            gfxBufferDestroy(m_buffer);
        }
        m_device = other.m_device;
        m_queue = other.m_queue;
        m_buffer = other.m_buffer;
        m_hostMapped = other.m_hostMapped;
        m_size = other.m_size;
        m_offset = other.m_offset;
        m_owning = other.m_owning;
        other.m_buffer = {};
        other.m_owning = false;
    }
    return *this;
}

void Buffer::Write(const void* data, const uint64_t size, const uint64_t offset)
{
    if (size > m_size) {
        throw std::runtime_error("Attempted to write " + std::to_string(size) + " bytes, which is out of range for the buffer of size " + std::to_string(m_size) + ".");
    }

    if (data) {
        gfxQueueWriteBuffer(m_queue, m_buffer, m_offset + offset, data, size);
    } else {
        std::vector<uint8_t> zeros(size, 0);
        gfxQueueWriteBuffer(m_queue, m_buffer, m_offset + offset, zeros.data(), size);
    }
}

void Buffer::Clear()
{
    std::vector<uint8_t> zeros(m_size, 0);
    gfxQueueWriteBuffer(m_queue, m_buffer, m_offset, zeros.data(), m_size);
}

uint64_t Buffer::GetSize() const
{
    return m_size;
}

uint64_t Buffer::GetOffset() const
{
    return m_offset;
}

Buffer::operator GfxBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer