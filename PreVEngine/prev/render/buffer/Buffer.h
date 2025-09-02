#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "../../core/memory/Allocator.h"

#include <stdexcept>

namespace prev::render::buffer {
class Buffer final {
public:
    Buffer(prev::core::memory::Allocator& allocator, VkBuffer buffer, VmaAllocation allocation, prev::core::memory::MemoryType memoryType, uint64_t size, uint64_t offset, void* mappedPtr);

    ~Buffer();

public:
    void Write(const void* data, const uint64_t size, const uint64_t offset = 0);

    void Clear();

public:
    uint64_t GetSize() const;

    uint64_t GetOffset() const;

    void* GetMappedPtr() const;

    template <typename T>
    T& GetMapped()
    {
        static_assert(std::is_trivially_copyable<T>::value, "Mapped type must be trivially copyable.");

        if (m_memoryType != prev::core::memory::MemoryType::HOST_MAPPED) {
            throw std::runtime_error("Could not get mapped data from a non HOST_MAPPED buffer");
        }

        if (sizeof(T) > m_size) {
            throw std::runtime_error("Could not get mapped data since mapped data type size (" + std::to_string(sizeof(T)) + ") exceeds the buffer size (" + std::to_string(m_size) + ").");
        }

        return *reinterpret_cast<T*>(static_cast<uint8_t*>(m_mappedPtr) + m_offset);
    }

    template <typename T>
    void Write(const T& data)
    {
        Write(&data, sizeof(T), m_offset);
    }

public:
    operator VkBuffer() const;

protected:
    prev::core::memory::Allocator& m_allocator;

    VkBuffer m_buffer;

    VmaAllocation m_allocation;

    prev::core::memory::MemoryType m_memoryType;

    uint64_t m_size;

    uint64_t m_offset;

    void* m_mappedPtr;
};
} // namespace prev::render::buffer

#endif // !__BUFFER_H__
