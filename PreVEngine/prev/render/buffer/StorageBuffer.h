#ifndef __STORAGE_BUFFER_H__
#define __STORAGE_BUFFER_H__

#include "Buffer.h"

#include "../../util/MathUtils.h"

namespace prev::render::buffer {

template <typename ItemType>
class StorageBuffer final : public Buffer {
public:
    StorageBuffer(prev::core::memory::Allocator& allocator, const uint32_t alignment = 32)
        : Buffer(allocator)
        , m_itemSize(prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(ItemType)), alignment))
        , m_mapped(nullptr)
    {
    }

    ~StorageBuffer() = default;

public:
    void WriteData(const ItemType& itemData)
    {
        EnsureBuffer();

        m_itemData = itemData;
        memcpy(m_mapped, &m_itemData, sizeof(m_itemData));
    }

    ItemType& ReadData()
    {
        EnsureBuffer();

        memcpy(&m_itemData, m_mapped, sizeof(m_itemData));

        return m_itemData;
    }

private:
    void EnsureBuffer()
    {
        if (!m_mapped) {
            Buffer::Data(nullptr, 1, m_itemSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED, &m_mapped);
        }
    }

private:
    ItemType m_itemData;

    uint32_t m_itemSize;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif