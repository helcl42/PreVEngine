#ifndef __BUFFER_BUILDER_H__
#define __BUFFER_BUILDER_H__

#include "Buffer.h"

#include <memory>

namespace prev::render::buffer {
class BufferBuilder final {
public:
    BufferBuilder(prev::core::memory::Allocator& allocator);

    ~BufferBuilder() = default;

public:
    BufferBuilder& SetUsageFlags(const VkBufferUsageFlags usageFlags);

    BufferBuilder& SetMemoryType(const prev::core::memory::MemoryType memoryType);

    BufferBuilder& SetSize(const uint64_t size);

    BufferBuilder& SetAlignment(const uint64_t alignment);

    BufferBuilder& SetData(const void* data, const uint64_t size);

    std::unique_ptr<Buffer> Build() const;

private:
    void Valiadte() const;

private:
    prev::core::memory::Allocator& m_allocator;

    VkBufferUsageFlags m_usageFlags{};

    prev::core::memory::MemoryType m_memoryType{ prev::core::memory::MemoryType::UNDEFINED };

    uint64_t m_size{ 0 };

    uint64_t m_alignment{ 8 };

    const void* m_data{ nullptr };

    uint64_t m_dataSize{};
};
} // namespace prev::render::buffer

#endif // !__BUFFER_BUILDER_H__