#ifndef __IMAGE_BUFFER_BUILDER_H__
#define __IMAGE_BUFFER_BUILDER_H__

#include "ImageBuffer.h"

namespace prev::render::buffer {

class ImageBufferBuilder final {
public:
    ImageBufferBuilder(prev::core::memory::Allocator& allocator);

    ~ImageBufferBuilder() = default;

public:
    ImageBufferBuilder& SetExtent(const VkExtent3D& extent);

    ImageBufferBuilder& SetFormat(VkFormat format);

    ImageBufferBuilder& SetType(VkImageType type);

    ImageBufferBuilder& SetViewType(VkImageViewType viewType);

    ImageBufferBuilder& SetSampleCount(VkSampleCountFlagBits samples);

    ImageBufferBuilder& SetMipMapEnabled(bool enabled);

    ImageBufferBuilder& SetLayerCount(uint32_t layerCount);

    ImageBufferBuilder& SetCreateFlags(VkImageCreateFlags createFlags);

    ImageBufferBuilder& SetUsageFlags(VkImageUsageFlags usageFlags);

    ImageBufferBuilder& SetLayout(VkImageLayout layout);

    ImageBufferBuilder& SetAspectMask(VkImageAspectFlags aspectMask);

    ImageBufferBuilder& SeTiling(VkImageTiling tiling);

    ImageBufferBuilder& SetLayerData(const std::vector<const uint8_t*>& layerData);

    std::unique_ptr<ImageBuffer> Build() const;

private:
    prev::core::memory::Allocator& m_allocator;

    VkExtent3D m_extent{};

    VkFormat m_format{ VK_FORMAT_UNDEFINED };

    VkImageType m_type{ VK_IMAGE_TYPE_MAX_ENUM };

    VkImageViewType m_viewType{ VK_IMAGE_VIEW_TYPE_MAX_ENUM };

    VkSampleCountFlagBits m_sampleCount{ VK_SAMPLE_COUNT_1_BIT };

    bool m_mipMapEnabled{ false };

    uint32_t m_layerCount{ 1 };

    VkImageCreateFlags m_createFlags{};

    VkImageUsageFlags m_usageFlags{};

    VkImageLayout m_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

    VkImageAspectFlags m_aspectMask{ VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM };

    VkImageTiling m_tiling{ VK_IMAGE_TILING_OPTIMAL };

    std::vector<const uint8_t*> m_layersData;
};

} // namespace prev::render::buffer

#endif