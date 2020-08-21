#ifndef __COMPUTE_PROVIDER_H__
#define __COMPUTE_PROVIDER_H__

#include "../common/pattern/Singleton.h"
#include "../core/Queue.h"
#include "../core/memory/Allocator.h"

#include <memory>

namespace prev::scene {
class ComputeProvider final : public prev::common::pattern::Singleton<ComputeProvider> {
public:
    ~ComputeProvider() = default;

public:
    void Set(const std::shared_ptr<prev::core::Queue>& queue, const std::shared_ptr<prev::core::memory::Allocator>& alloc);

    void Reset();

    bool IsAvailable() const;

    std::shared_ptr<prev::core::Queue> GetQueue() const;

    std::shared_ptr<prev::core::memory::Allocator> GetAllocator() const;

private:
    ComputeProvider() = default;

private:
    friend class prev::common::pattern::Singleton<ComputeProvider>;

private:
    std::shared_ptr<prev::core::Queue> m_computeQueue;

    std::shared_ptr<prev::core::memory::Allocator> m_computeAllocator;
};
} // namespace prev::scene

#endif