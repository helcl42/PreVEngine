#include "AllocatorProvider.h"

#include <stdexcept>

namespace prev::core {
std::shared_ptr<prev::core::memory::Allocator> AllocatorProvider::GetAllocator() const
{
    return m_allocator;
}

void AllocatorProvider::SetAllocator(const std::shared_ptr<prev::core::memory::Allocator>& alloc)
{
    m_allocator = alloc;
}
} // namespace prev::core