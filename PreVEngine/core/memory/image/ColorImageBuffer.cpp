#include "ColorImageBuffer.h"

namespace PreVEngine {
ColorImageBuffer::ColorImageBuffer(Allocator& allocator)
    : AbstractImageBuffer(allocator)
{
}

void ColorImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
{
    m_format = createInfo.format;
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

    m_allocator.CreateImage(extent, m_imageType, m_format, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_flags, m_image, m_allocation);
    m_allocator.CreateImageView(m_image, m_format, m_imageViewType, m_mipLevels, m_layerCount, VK_IMAGE_ASPECT_COLOR_BIT, m_imageView);

    m_extent = extent;
}
} // namespace PreVEngine