#include "DepthImageBuffer.h"

namespace prev::core::memory::image {
DepthImageBuffer::DepthImageBuffer(Allocator& allocator)
    : AbstractImageBuffer(allocator)
{
}

void DepthImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
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

    m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, m_format, m_mipLevels, m_layerCount);
}

void DepthImageBuffer::Resize(const VkExtent3D& extent)
{
    Destroy();

    m_allocator.CreateImage(extent, m_imageType, m_format, m_sampleCount, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_flags, m_image, m_allocation);
    m_allocator.CreateImageView(m_image, m_format, m_imageViewType, m_mipLevels, m_layerCount, VK_IMAGE_ASPECT_DEPTH_BIT, m_imageView);

    m_extent = extent;
}
} // namespace prev::core::memory::image