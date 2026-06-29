#include "Buffer.h"
#include "OwnedGfxBuffer.h"

#include "../../core/DeferredResourceDestroyer.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace prev::render::buffer {
Buffer::Buffer(GfxDevice device, GfxQueue queue, const CreateInfo& createInfo)
    : m_device{ device }
    , m_queue{ queue }
    , m_buffer{ createInfo.buffer }
    , m_hostMapped{ createInfo.hostMapped }
    , m_size{ createInfo.size }
    , m_offset{ createInfo.offset }
    , m_owning{ createInfo.owning }
    , m_deferredResourceDestroyer{ createInfo.deferredResourceDestroyer }
    , m_destroyExecutionMode{ createInfo.destroyExecutionMode }
    , m_state{ createInfo.stateFlag ? createInfo.stateFlag : std::make_shared<std::atomic<prev::core::ResourceState>>(prev::core::ResourceState::Ready) }
{
    assert(m_deferredResourceDestroyer && "Buffer requires a deferred resource destroyer");
}

Buffer::~Buffer()
{
    ReleaseBuffer();
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_device{ other.m_device }
    , m_queue{ other.m_queue }
    , m_buffer{ other.m_buffer }
    , m_hostMapped{ other.m_hostMapped }
    , m_size{ other.m_size }
    , m_offset{ other.m_offset }
    , m_owning{ other.m_owning }
    , m_deferredResourceDestroyer{ other.m_deferredResourceDestroyer }
    , m_destroyExecutionMode{ other.m_destroyExecutionMode }
    , m_state{ other.m_state }
{
    other.m_buffer = {};
    other.m_owning = false;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if (this != &other) {
        ReleaseBuffer();
        m_device = other.m_device;
        m_queue = other.m_queue;
        m_buffer = other.m_buffer;
        m_hostMapped = other.m_hostMapped;
        m_size = other.m_size;
        m_offset = other.m_offset;
        m_owning = other.m_owning;
        m_deferredResourceDestroyer = other.m_deferredResourceDestroyer;
        m_destroyExecutionMode = other.m_destroyExecutionMode;
        m_state = other.m_state;
        other.m_buffer = {};
        other.m_owning = false;
    }
    return *this;
}

bool Buffer::IsDeferred() const
{
    if (m_destroyExecutionMode == ExecutionMode::Immediate) {
        return false;
    }
    return m_deferredResourceDestroyer->IsActive();
}

prev::core::ResourceState Buffer::GetState() const
{
    return m_state->load();
}

void Buffer::ReleaseBuffer()
{
    if (!m_owning || !m_buffer) {
        return;
    }
    // Cancel a still-pending async upload (optimization; the uploader's record is handle-only, not a UAF).
    if (m_state && m_state->load() == prev::core::ResourceState::Creating) {
        m_state->store(prev::core::ResourceState::Destroying);
    }
    if (IsDeferred()) {
        // Defer destruction past any in-flight work; no queue stall.
        m_deferredResourceDestroyer->Destroy(std::make_unique<OwnedGfxBuffer>(m_buffer));
    } else {
        gfxQueueWaitIdle(m_queue);
        gfxBufferDestroy(m_buffer);
    }
    m_buffer = {};
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

Buffer Buffer::Slice(uint64_t offset, uint64_t size) const
{
    // Non-owning view sharing this buffer's GfxBuffer; offset is relative to this buffer's own
    // offset so slicing composes. The view never destroys the GPU buffer (owning = false).
    CreateInfo info{};
    info.buffer = m_buffer;
    info.hostMapped = m_hostMapped;
    info.size = size;
    info.offset = m_offset + offset;
    info.owning = false;
    info.deferredResourceDestroyer = m_deferredResourceDestroyer;
    info.destroyExecutionMode = m_destroyExecutionMode;
    info.stateFlag = m_state; // the view shares the parent's lifecycle state

    return Buffer(m_device, m_queue, info);
}

Buffer::operator GfxBuffer() const
{
    return m_buffer;
}
} // namespace prev::render::buffer