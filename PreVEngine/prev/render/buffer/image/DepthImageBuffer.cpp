#include "DepthImageBuffer.h"

#include "../../../core/Formats.h"
#include "../../../util/VkUtils.h"

namespace prev::render::buffer::image {
DepthImageBuffer::DepthImageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount)
    : AbstractImageBuffer(allocator, ext, imgType, fmt, flags, samplesCnt, mipMap, vwType, lrCount)
{
    DepthImageBuffer::Resize(m_extent);
}

void DepthImageBuffer::Resize(const VkExtent3D& extent)
{
    Destroy();

    m_extent = extent;
    m_mipLevels = 1;

    const bool hasStencil{ prev::core::HasStencilComponent(m_format) };

    m_allocator.CreateImage(m_extent, m_imageType, m_format, m_sampleCount, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_flags, m_image, m_allocation);
    m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, m_format, m_mipLevels, m_layerCount);

    VkImageAspectFlags aspectMask{ VK_IMAGE_ASPECT_DEPTH_BIT };
    if (hasStencil) {
        aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    m_imageView = prev::util::vk::CreateImageView(m_allocator.GetDevice(), m_image, m_format, m_imageViewType, m_mipLevels, aspectMask, m_layerCount);
}
} // namespace prev::render::buffer::image