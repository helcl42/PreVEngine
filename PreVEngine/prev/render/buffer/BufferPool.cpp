#include "BufferPool.h"

namespace prev::render::buffer {
BufferPool::BufferPool(std::vector<std::unique_ptr<Buffer>>&& buffers)
    : m_buffers{ std::move(buffers) }
    , m_index{ prev::util::CircularIndex<uint32_t>(static_cast<uint32_t>(m_buffers.size())) }
{
}

Buffer& BufferPool::GetCurrent() const
{
    return *m_buffers[m_index];
}

void BufferPool::MoveToNext()
{
    ++m_index;
}
} // namespace prev::render::buffer
