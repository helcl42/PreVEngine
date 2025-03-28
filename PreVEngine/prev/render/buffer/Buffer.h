#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../../core/memory/Allocator.h"

namespace prev::render::buffer {
class Buffer {
public:
    Buffer(prev::core::memory::Allocator& allocator);

    virtual ~Buffer();

public:
    void Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlags usage, const prev::core::memory::MemoryType memoryType = prev::core::memory::MemoryType::DEVICE_LOCAL);

    void Clear();

public:
    uint32_t GetCount() const;

    uint32_t GetSize() const;

    uint32_t GetOffset() const;

    void* GetMappedPtr() const;

public:
    operator VkBuffer() const;

protected:
    prev::core::memory::Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkBuffer m_buffer;

    uint32_t m_count;

    uint32_t m_stride;

    uint32_t m_offset;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif // !__BUFFER_H__
