#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

#include <vector>

namespace prev::render::buffer::image {
class ImageBuffer : public AbstractImageBuffer {
public:
    ImageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount, const std::vector<const uint8_t*>& lrImageData);

    ~ImageBuffer() = default;

public:
    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev::render::buffer::image

#endif