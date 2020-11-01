#ifndef __IIMAGE_BUFFER_H__
#define __IIMAGE_BUFFER_H__

#include "ImageBufferCreateInfo.h"

namespace prev::core::memory::image {
class IImageBuffer {
public:
    virtual void Create(const ImageBufferCreateInfo& createInfo) = 0;

    virtual void Resize(const VkExtent3D& extent) = 0;

    virtual void Destroy() = 0;

    virtual void CreateSampler(const float maxLod = 1.0f, const VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const bool enableFiltering = true) = 0;

    virtual void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) = 0;

    virtual VkImage GetImage() const = 0;

    virtual VkImageView GetImageView() const = 0;

    virtual VkSampler GetSampler() const = 0;

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
} // namespace prev::core::memory::image

#endif