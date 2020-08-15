#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"
#include "../../../util/MathUtils.h"

namespace prev {
class UBO {
public:
    UBO(VkBuffer buffer, void* data, const uint32_t offset, const uint32_t range);

    ~UBO() = default;

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
class UBOPool : public Buffer {
public:
    UBOPool(Allocator& allocator)
        : Buffer(allocator)
        , m_index(0)
        , m_capacity(0)
        , m_mapped(nullptr)
    {
    }

    virtual ~UBOPool() = default;

public:
    void AdjustCapactity(const uint32_t capacity, const uint32_t alignment = 32)
    {
        Clear();

        m_poolItems.clear();

        m_capacity = capacity;
        m_index = 0;

        const uint32_t itemSize = MathUtil::RoundUp(static_cast<uint32_t>(sizeof(ItemType)), alignment);

        Data(nullptr, capacity, itemSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, &m_mapped);

        for (uint32_t i = 0; i < capacity; i++) {
            auto ubo = std::make_shared<UBO>(m_buffer, m_mapped, i * itemSize, itemSize);
            m_poolItems.emplace_back(ubo);
        }
    }

    std::shared_ptr<UBO> GetNext()
    {
        m_index = (m_index + 1) % m_poolItems.size();
        return m_poolItems.at(m_index);
    }

private:
    std::vector<std::shared_ptr<UBO> > m_poolItems;

    uint32_t m_capacity;

    uint32_t m_index;

    void* m_mapped;
};
} // namespace prev

#endif