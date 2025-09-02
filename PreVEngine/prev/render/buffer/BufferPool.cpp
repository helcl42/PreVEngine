#include "BufferPool.h"

namespace prev::render::buffer {
BufferPool::BufferPool(prev::core::memory::Allocator& allocator, VkBuffer buffer, VmaAllocation allocation, std::vector<std::unique_ptr<Buffer>>&& buffers)
    : m_allocator{ allocator }
    , m_buffer{ buffer }
    , m_allocation{ allocation }
    , m_buffers{ std::move(buffers) }
    , m_index{ prev::util::CircularIndex<uint32_t>(m_buffers.size()) }
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
