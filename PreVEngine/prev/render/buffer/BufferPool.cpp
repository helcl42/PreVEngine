#include "BufferPool.h"

namespace prev::render::buffer {
BufferPool::BufferPool(std::unique_ptr<Buffer> buffer, std::vector<Buffer>&& slices)
    : m_buffer{ std::move(buffer) }
    , m_slices{ std::move(slices) }
    , m_index{ prev::util::CircularIndex<uint32_t>(static_cast<uint32_t>(m_slices.size())) }
{
}

Buffer& BufferPool::Next()
{
    Buffer& slice{ m_slices[m_index] };
    ++m_index;
    return slice;
}
} // namespace prev::render::buffer
