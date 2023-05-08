#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev::render::buffer::image {
class ImageBuffer : public AbstractImageBuffer {
public:
    ImageBuffer(prev::core::memory::Allocator& allocator);

    ~ImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev::render::buffer::image

#endif