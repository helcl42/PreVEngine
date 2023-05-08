#ifndef __IMAGE_STORAGE_BUFFER_H__
#define __IMAGE_STORAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev::render::buffer::image {
class ImageStorageBuffer : public AbstractImageBuffer {
public:
    ImageStorageBuffer(prev::core::memory::Allocator& allocator);

    ~ImageStorageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev::render::buffer::image

#endif // !__IMAGE_STORAGE_BUFFER_H__
