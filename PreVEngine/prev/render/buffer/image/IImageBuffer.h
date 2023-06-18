#ifndef __IIMAGE_BUFFER_H__
#define __IIMAGE_BUFFER_H__

#include "ImageBufferCreateInfo.h"

namespace prev::render::buffer::image {
class IImageBuffer {
public:
    virtual void Resize(const VkExtent3D& extent) = 0;

    virtual VkImage GetImage() const = 0;

    virtual VkImageView GetImageView() const = 0;

    virtual VkFormat GetFormat() const = 0;

    virtual VkSampleCountFlagBits GetSampleCount() const = 0;

    virtual VkExtent3D GetExtent() const = 0;

    virtual VkImageType GetImageType() const = 0;

    virtual VkImageCreateFlags GetFlags() const = 0;

    virtual uint32_t GetMipLevels() const = 0;

    virtual uint32_t GetLayerCount() const = 0;

    virtual VkImageViewType GetViewType() const = 0;

public:
    virtual ~IImageBuffer() = default;
};
} // namespace prev::render::buffer::image

#endif