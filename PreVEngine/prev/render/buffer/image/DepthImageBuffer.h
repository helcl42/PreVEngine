#ifndef __DEPTH_IMAGE_BUFFER_H__
#define __DEPTH_IMAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev::render::buffer::image {
class DepthImageBuffer : public AbstractImageBuffer {
public:
    DepthImageBuffer(prev::core::memory::Allocator& allocator);

    ~DepthImageBuffer() = default;

public:
    void Create(const ImageBufferCreateInfo& createInfo) override;

    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev::render::buffer::image

#endif