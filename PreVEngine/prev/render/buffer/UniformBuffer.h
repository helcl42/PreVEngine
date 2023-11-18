#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"

#include "../../util/MathUtils.h"

namespace prev::render::buffer {
class UniformBuffer final {
public:
    UniformBuffer(VkBuffer buffer, void* data, const uint32_t offset, const uint32_t range);

    ~UniformBuffer() = default;

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
class UniformBufferRing final : public Buffer {
public:
    UniformBufferRing(prev::core::memory::Allocator& allocator)
        : Buffer(allocator)
        , m_capacity(0)
        , m_index(0)
        , m_mapped(nullptr)
    {
    }

    ~UniformBufferRing() = default;

public:
    void AdjustCapactity(const uint32_t capacity, const uint32_t alignment = 32)
    {
        Clear();

        m_buffers.clear();

        m_capacity = capacity;
        m_index = 0;

        const uint32_t itemSize = prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(ItemType)), alignment);
        Data(nullptr, capacity, itemSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);

        for (uint32_t i = 0; i < capacity; i++) {
            auto ubo = std::make_shared<UniformBuffer>(m_buffer, m_mapped, i * itemSize, itemSize);
            m_buffers.emplace_back(ubo);
        }
    }

    std::shared_ptr<UniformBuffer> GetNext()
    {
        m_index = (m_index + 1) % m_buffers.size();
        return m_buffers[m_index];
    }

private:
    std::vector<std::shared_ptr<UniformBuffer>> m_buffers;

    uint32_t m_capacity;

    uint32_t m_index;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif