#ifndef __INDEX_BUFFER_H__
#define __INDEX_BUFFER_H__

#include "Buffer.h"

namespace prev::render::buffer {
class IndexBuffer : public Buffer {
public:
    using Buffer::Buffer;

public:
    virtual void Data(const uint16_t* data, const uint32_t count);

    virtual void Data(const uint32_t* data, const uint32_t count);

    VkIndexType GetIndexType() const;

protected:
    VkIndexType m_indexType{ VK_INDEX_TYPE_MAX_ENUM };
};

class HostMappedIndexBuffer final : public IndexBuffer {
public:
    HostMappedIndexBuffer(prev::core::memory::Allocator& allocator, const uint32_t maxCount);

public:
    void Data(const uint16_t* data, const uint32_t count) override;

    void Data(const uint32_t* data, const uint32_t count) override;

private:
    uint32_t m_maxCount;

    void* m_mapped;
};
} // namespace prev::render::buffer

#endif