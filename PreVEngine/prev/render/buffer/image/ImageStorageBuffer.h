#ifndef __IMAGE_STORAGE_BUFFER_H__
#define __IMAGE_STORAGE_BUFFER_H__

#include "AbstractImageBuffer.h"

namespace prev::render::buffer::image {
class ImageStorageBuffer : public AbstractImageBuffer {
public:
    ImageStorageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount);

public:
    void Resize(const VkExtent3D& extent) override;
};
} // namespace prev::render::buffer::image

#endif // !__IMAGE_STORAGE_BUFFER_H__
