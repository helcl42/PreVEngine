#ifndef __BUFFER_POOL_BUILDER_H__
#define __BUFFER_POOL_BUILDER_H__

#include "BufferPool.h"

#include <memory>

namespace prev::render::buffer {
class BufferPoolBuilder final {
public:
    BufferPoolBuilder(prev::core::memory::Allocator& allocator);

    ~BufferPoolBuilder() = default;

public:
    BufferPoolBuilder& SetUsageFlags(const VkBufferUsageFlags usageFlags);

    BufferPoolBuilder& SetMemoryType(const prev::core::memory::MemoryType memoryType);

    BufferPoolBuilder& SetCount(const uint64_t count);

    BufferPoolBuilder& SetStride(const uint64_t stride);

    BufferPoolBuilder& SetAlignment(const uint64_t alignment);

    std::unique_ptr<BufferPool> Build() const;

private:
    void Valiadate() const;

private:
    prev::core::memory::Allocator& m_allocator;

    VkBufferUsageFlags m_usageFlags{};

    prev::core::memory::MemoryType m_memoryType{ prev::core::memory::MemoryType::UNDEFINED };

    uint64_t m_count{ 0 };

    uint64_t m_stride{ 0 };

    uint64_t m_alignment{ 8 };
};
} // namespace prev::render::buffer

#endif // !__BUFFER_POOL_BUILDER_H__
