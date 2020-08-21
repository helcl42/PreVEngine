#include "ComputeProvider.h"

namespace prev::scene {
void ComputeProvider::Set(const std::shared_ptr<prev::core::Queue>& queue, const std::shared_ptr<prev::core::memory::Allocator>& alloc)
{
    m_computeQueue = queue;
    m_computeAllocator = alloc;
}

void ComputeProvider::Reset()
{
    m_computeQueue = nullptr;
    m_computeAllocator = nullptr;
}

bool ComputeProvider::IsAvailable() const
{
    return m_computeQueue != nullptr;
}

std::shared_ptr<prev::core::Queue> ComputeProvider::GetQueue() const
{
    return m_computeQueue;
}

std::shared_ptr<prev::core::memory::Allocator> ComputeProvider::GetAllocator() const
{
    return m_computeAllocator;
}
} // namespace prev::scene