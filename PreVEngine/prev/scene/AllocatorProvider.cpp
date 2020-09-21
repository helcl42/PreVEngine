#include "AllocatorProvider.h"

#include <stdexcept>

namespace prev::scene {
std::shared_ptr<prev::core::memory::Allocator> AllocatorProvider::GetAllocator() const
{
    return m_allocator;
}

void AllocatorProvider::SetAllocator(const std::shared_ptr<prev::core::memory::Allocator>& alloc)
{
    m_allocator = alloc;
}
} // namespace prev::scene