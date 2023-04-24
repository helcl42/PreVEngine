#include "ColorImageBuffer.h"

#include "../../../util/VkUtils.h"

namespace prev::core::memory::image {
ColorImageBuffer::ColorImageBuffer(Allocator& allocator)
    : AbstractImageBuffer(allocator)
{
}

void ColorImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
{
    m_format = createInfo.format;
    m_sampleCount = createInfo.samplesCount;
    m_mipLevels = 1;
    m_layerCount = createInfo.layerCount;
    m_imageViewType = createInfo.viewType;
    m_imageType = createInfo.imageType;
    m_flags = createInfo.flags;
    m_sampler = VK_NULL_HANDLE;

    Resize(createInfo.extent);
}

void ColorImageBuffer::Resize(const VkExtent3D& extent)
{
    Destroy();

    m_allocator.CreateImage(extent, m_imageType, m_format, m_sampleCount, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_flags, m_image, m_allocation);
    m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, m_format, m_mipLevels, m_layerCount);

    m_imageView = prev::util::vk::CreateImageView(m_allocator.GetDevice(), m_image, m_format, m_imageViewType, m_mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, m_layerCount);

    m_extent = extent;
}
} // namespace prev::core::memory::image