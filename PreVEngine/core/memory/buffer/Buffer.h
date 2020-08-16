#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../Allocator.h"

namespace prev::core::memory::buffer {
class Buffer {
public:
    Buffer(Allocator& allocator);

    virtual ~Buffer();

public:
    void Clear();

    void Data(const void* data, const uint32_t count, const uint32_t stride, const VkBufferUsageFlags usage, const VmaMemoryUsage memtype = VMA_MEMORY_USAGE_GPU_ONLY, void** mapped = nullptr);

public:
    uint32_t GetCount() const;

    uint32_t GetSize() const;

public:
    operator VkBuffer() const;

protected:
    Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkBuffer m_buffer;

    uint32_t m_count;

    uint32_t m_stride;
};
} // namespace prev::core::memory::buffer

#endif // !__BUFFER_H__
