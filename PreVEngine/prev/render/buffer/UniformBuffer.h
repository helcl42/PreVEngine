#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

namespace prev::render::buffer {
template <typename Type>
class UnifomBuffer final : public Buffer {
public:
    UnifomBuffer(prev::core::memory::Allocator& allocator, const uint32_t alignment = 32)
        : Buffer(allocator)
    {
        m_count = 1;
        m_stride = prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(Type)), alignment);
    }

    UnifomBuffer(prev::core::memory::Allocator& allocator, VkBuffer buffer, void* mappedPtr, const uint32_t offset, const uint32_t size)
        : Buffer(allocator)
    {
        m_buffer = buffer;
        m_count = 1;
        m_stride = size;
        m_offset = offset;
        m_mapped = mappedPtr;
    }

    ~UnifomBuffer() = default;

public:
    void Data(const Type& data)
    {
        if (!m_mapped) {
            Buffer::Data(nullptr, m_count, m_stride, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED);
        }

        memcpy(m_mapped, &data, sizeof(Type));
    }
};

template <typename Type>
class UniformRingBuffer final : public Buffer {
public:
    UniformRingBuffer(prev::core::memory::Allocator& allocator)
        : Buffer(allocator)
    {
    }

    ~UniformRingBuffer() = default;

public:
    void UpdateCapacity(const uint32_t capacity, const uint32_t alignment = 32)
    {
        Clear();

        m_buffers.clear();

        m_index = prev::util::CircularIndex<uint32_t>{ capacity };

        const uint32_t itemSize{ prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(Type)), alignment) };
        Buffer::Data(nullptr, capacity, itemSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED);

        for (uint32_t i = 0; i < capacity; ++i) {
            const auto offset{ i * itemSize };
            auto ubo = std::make_shared<UnifomBuffer<Type>>(m_allocator, m_buffer, static_cast<uint8_t*>(m_mapped) + offset, offset, itemSize);
            m_buffers.emplace_back(ubo);
        }
    }

    UnifomBuffer<Type>& GetNext()
    {
        return *m_buffers[++m_index];
    }

private:
    std::vector<std::shared_ptr<UnifomBuffer<Type>>> m_buffers;

    prev::util::CircularIndex<uint32_t> m_index{ 0 };
};
} // namespace prev::render::buffer

#endif