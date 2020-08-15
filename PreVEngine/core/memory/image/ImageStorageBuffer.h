#ifndef __IMAGE_STORAGE_BUFFER_H__
#define __IMAGE_STORAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev {
class ImageStorageBuffer : public AbstractImageBuffer {
public:
    ImageStorageBuffer(Allocator& allocator);

    ~ImageStorageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev

#endif // !__IMAGE_STORAGE_BUFFER_H__
