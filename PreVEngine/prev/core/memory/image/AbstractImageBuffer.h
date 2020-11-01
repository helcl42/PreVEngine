#ifndef __ABSTRACT_IMAGE_BUFFER_H__
#define __ABSTRACT_IMAGE_BUFFER_H__

#include "IImageBuffer.h"

namespace prev::core::memory::image {
class AbstractImageBuffer : public IImageBuffer {
public:
    AbstractImageBuffer(Allocator& allocator);

    virtual ~AbstractImageBuffer();

public:
    void UpdateSampler(const VkSamplerCreateInfo& samplerInfo) override;

    void Destroy() override;

    void CreateSampler(const float maxLod = 1.0f, const VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const bool enableFiltering = true) override;

public:
    VkImage GetImage() const override;

    VkImageView GetImageView() const override;

    VkSampler GetSampler() const override;

    VkFormat GetFormat() const override;

    VkSampleCountFlagBits GetSampleCount() const override;

    VkExtent3D GetExtent() const override;

    VkImageType GetImageType() const override;

    VkImageCreateFlags GetFlags() const override;

    uint32_t GetMipLevels() const override;

    uint32_t GetLayerCount() const override;

    VkImageViewType GetViewType() const override;

protected:
    Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkImage m_image;

    VkExtent3D m_extent;

    VkImageType m_imageType;

    VkImageCreateFlags m_flags;

    VkFormat m_format;

    VkSampleCountFlagBits m_sampleCount;

    VkImageView m_imageView;

    VkSampler m_sampler;

    uint32_t m_mipLevels;

    uint32_t m_layerCount;

    VkImageViewType m_imageViewType;
};
} // namespace prev::core::memory::image

#endif