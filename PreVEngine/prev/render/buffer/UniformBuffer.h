#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"

#include "../../util/MathUtils.h"
#include "../../util/Utils.h"

namespace prev::render::buffer {
class UnifomRingBufferItem final {
public:
    UnifomRingBufferItem(VkBuffer buffer, void* data, const uint32_t offset, const uint32_t range);

    ~UnifomRingBufferItem() = default;

public:
    void Update(const void* data);

    uint32_t GetOffset() const;

    uint32_t GetRange() const;

    operator VkBuffer() const;

private:
    VkBuffer m_buffer;

    void* m_mapped;

    uint32_t m_offset;

    uint32_t m_range;
};

template <typename ItemType>
class UniformRingBuffer final : public Buffer {
public:
    UniformRingBuffer(prev::core::memory::Allocator& allocator)
        : Buffer(allocator)
        , m_mapped(nullptr)
    {
    }

    ~UniformRingBuffer() = default;

public:
    void UpdateCapacity(const uint32_t capacity, const uint32_t alignment = 32)
    {
        Clear();

        m_buffers.clear();

        m_index = prev::util::CircularIndex<uint32_t>{ capacity };

        const uint32_t itemSize{ prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(ItemType)), alignment) };
        Data(nullptr, capacity, itemSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED, &m_mapped);

        for (uint32_t i = 0; i < capacity; ++i) {
            auto ubo = std::make_shared<UnifomRingBufferItem>(m_buffer, m_mapped, i * itemSize, itemSize);
            m_buffers.emplace_back(ubo);
        }
    }

    std::shared_ptr<UnifomRingBufferItem> GetNext()
    {
        ++m_index;
        return m_buffers[m_index];
    }

private:
    std::vector<std::shared_ptr<UnifomRingBufferItem>> m_buffers;

    prev::util::CircularIndex<uint32_t> m_index{ 0 };

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif