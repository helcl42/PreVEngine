#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "../../core/memory/Allocator.h"

namespace prev::render::buffer {
class ImageBufferBuilder;

class ImageBuffer final {
private:
    ImageBuffer(prev::core::memory::Allocator& allocator);

public:
    ~ImageBuffer();

public:
    void UpdateLayout(const VkImageLayout newLayout, VkCommandBuffer commandBuffer);

    void GenerateMipMaps(VkCommandBuffer commandBuffer);

    void Copy(ImageBuffer& dstImage, VkCommandBuffer commandBuffer);

    VkExtent3D GetExtent() const;

    VkFormat GetFormat() const;

    VkImage GetImage() const;

    VkImageType GetImageType() const;

    VkImageView GetImageView() const;

    VkImageViewType GetImageViewType() const;

    VkImageAspectFlags GetAspectMask() const;

    VkSampleCountFlagBits GetSampleCount() const;

    uint32_t GetMipLevels() const;

    uint32_t GetLayerCount() const;

    VkImageCreateFlags GetCreateFlags() const;

    VkImageUsageFlags GetUsageFlags() const;

    VkImageLayout GetLayout() const;

    void* GetMappedData() const;

    operator VkImage() const;

public:
    friend class ImageBufferBuilder;

private:
    prev::core::memory::Allocator& m_allocator;

    VkExtent3D m_extent;

    VkFormat m_format;

    VkImage m_image;

    VmaAllocation m_allocation;

    VkImageType m_type;

    VkImageView m_view;

    VkImageViewType m_viewType;

    VkImageAspectFlags m_aspectMask;

    VkSampleCountFlagBits m_samplesCount;

    uint32_t m_mipLevels;

    uint32_t m_layerCount;

    VkImageCreateFlags m_createFlags;

    VkImageUsageFlags m_usageFlags;

    VkImageLayout m_layout;

    void* m_mappedData;
};

} // namespace prev::render::buffer

#endif
