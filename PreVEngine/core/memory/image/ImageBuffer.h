#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev {
class ImageBuffer : public AbstractImageBuffer {
public:
    ImageBuffer(Allocator& allocator);

    ~ImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev

#endif