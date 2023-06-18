#ifndef __ABSTRACT_IMAGE_BUFFER_H__
#define __ABSTRACT_IMAGE_BUFFER_H__

#include "IImageBuffer.h"

#include "../../../core/memory/Allocator.h"

namespace prev::render::buffer::image {
class AbstractImageBuffer : public IImageBuffer {
public:
    AbstractImageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount);

    virtual ~AbstractImageBuffer();

protected:
    void Destroy();

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

    VkExtent3D m_extent;

    VkImageType m_imageType;

    VkFormat m_format;

    VkImageCreateFlags m_flags;

    VkSampleCountFlagBits m_sampleCount;

    bool m_mipMap;

    VkImageViewType m_imageViewType;

    uint32_t m_layerCount;

    uint32_t m_mipLevels;

    VmaAllocation m_allocation;

    VkImage m_image;

    VkImageView m_imageView;
};
} // namespace prev::render::buffer::image

#endif