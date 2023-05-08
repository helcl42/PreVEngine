#ifndef __ABSTRACT_IMAGE_BUFFER_H__
#define __ABSTRACT_IMAGE_BUFFER_H__

#include "IImageBuffer.h"

#include "../../../core/memory/Allocator.h"

namespace prev::render::buffer::image {
class AbstractImageBuffer : public IImageBuffer {
public:
    AbstractImageBuffer(prev::core::memory::Allocator& allocator);

    virtual ~AbstractImageBuffer();

public:
    void Destroy() override;

public:
    VkImage GetImage() const override;

    VkImageView GetImageView() const override;

    VkFormat GetFormat() const override;

    VkSampleCountFlagBits GetSampleCount() const override;

    VkExtent3D GetExtent() const override;

    VkImageType GetImageType() const override;

    VkImageCreateFlags GetFlags() const override;

    uint32_t GetMipLevels() const override;

    uint32_t GetLayerCount() const override;

    VkImageViewType GetViewType() const override;

protected:
    prev::core::memory::Allocator& m_allocator;

    VmaAllocation m_allocation;

    VkImage m_image;

    VkExtent3D m_extent;

    VkImageType m_imageType;

    VkImageCreateFlags m_flags;

    VkFormat m_format;

    VkSampleCountFlagBits m_sampleCount;

    VkImageView m_imageView;

    uint32_t m_mipLevels;

    uint32_t m_layerCount;

    VkImageViewType m_imageViewType;
};
} // namespace prev::render::buffer::image

#endif