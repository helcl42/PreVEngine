#ifndef __INDEX_BUFFER_H__
#define __INDEX_BUFFER_H__

#include "Buffer.h"

namespace prev {
class IBO : public Buffer {
public:
    using Buffer::Buffer;

public:
    void Data(const uint16_t* data, const uint32_t count);

    void Data(const uint32_t* data, const uint32_t count);

    VkIndexType GetIndexType() const;

private:
    VkIndexType m_indexType = VK_INDEX_TYPE_MAX_ENUM;
};
} // namespace prev

#endif