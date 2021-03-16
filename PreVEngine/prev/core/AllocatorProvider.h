#ifndef __ALLOCATOR_PROVIDER_H__
#define __ALLOCATOR_PROVIDER_H__

#include "../common/pattern/Singleton.h"
#include "../core/memory/Allocator.h"

#include <memory>

namespace prev::core {
class AllocatorProvider final : public prev::common::pattern::Singleton<AllocatorProvider> {
public:
    ~AllocatorProvider() = default;

public:
    std::shared_ptr<prev::core::memory::Allocator> GetAllocator() const;

    void SetAllocator(const std::shared_ptr<prev::core::memory::Allocator>& alloc);

private:
    AllocatorProvider() = default;

private:
    friend class prev::common::pattern::Singleton<AllocatorProvider>;

private:
    std::shared_ptr<prev::core::memory::Allocator> m_allocator;
};
} // namespace prev::core

#endif // !__ALLOCATOR_PROVIDER_H__
