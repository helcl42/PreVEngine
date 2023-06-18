#include "ColorImageBuffer.h"

#include "../../../util/VkUtils.h"

namespace prev::render::buffer::image {
ColorImageBuffer::ColorImageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount)
    : AbstractImageBuffer(allocator, ext, imgType, fmt, flags, samplesCnt, mipMap, vwType, lrCount)
{
    ColorImageBuffer::Resize(m_extent);
}

void ColorImageBuffer::Resize(const VkExtent3D& extent)
{
    Destroy();

    m_extent = extent;
    m_mipLevels = 1;

    m_allocator.CreateImage(m_extent, m_imageType, m_format, m_sampleCount, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_flags, m_image, m_allocation);
    m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, m_format, m_mipLevels, m_layerCount);

    m_imageView = prev::util::vk::CreateImageView(m_allocator.GetDevice(), m_image, m_format, m_imageViewType, m_mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, m_layerCount);
}
} // namespace prev::render::buffer::image