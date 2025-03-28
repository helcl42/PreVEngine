#ifndef __STORAGE_BUFFER_H__
#define __STORAGE_BUFFER_H__

#include "Buffer.h"

#include "../../util/MathUtils.h"

namespace prev::render::buffer {

template <typename Type>
class StorageBuffer final : public Buffer {
public:
    StorageBuffer(prev::core::memory::Allocator& allocator, const uint32_t alignment = 32)
        : Buffer(allocator)
    {
        m_count = 1;
        m_stride = prev::util::math::RoundUp(static_cast<uint32_t>(sizeof(Type)), alignment);
    }

    ~StorageBuffer() = default;

public:
    void Data(const Type& itemData)
    {
        EnsureBuffer();

        m_itemData = itemData;
        memcpy(m_mapped, &m_itemData, sizeof(m_itemData));
    }

    Type& Data()
    {
        EnsureBuffer();

        memcpy(&m_itemData, m_mapped, sizeof(m_itemData));

        return m_itemData;
    }

private:
    void EnsureBuffer()
    {
        if (!m_mapped) {
            Buffer::Data(nullptr, m_count, m_stride, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, prev::core::memory::MemoryType::HOST_MAPPED);
        }
    }

private:
    Type m_itemData;
};
} // namespace prev::render::buffer

#endif