#include "ImageBuffer.h"

#include "../../../util/MathUtils.h"
#include "../../../util/VkUtils.h"
#include "../../instance/Validation.h"

namespace prev::core::memory::image {
ImageBuffer::ImageBuffer(Allocator& allocator)
    : AbstractImageBuffer(allocator)
{
}

void ImageBuffer::Create(const ImageBufferCreateInfo& createInfo)
{
    m_format = createInfo.format;
    m_sampleCount = createInfo.samplesCount;
    m_layerCount = createInfo.layerCount;
    m_imageViewType = createInfo.viewType;
    m_imageType = createInfo.imageType;
    m_flags = createInfo.flags;
    m_extent = createInfo.extent;

    m_mipLevels = 1;
    if (createInfo.mipMap) {
        m_mipLevels = prev::util::math::Log2(std::max(createInfo.extent.width, createInfo.extent.height)) + 1;
    }

    m_allocator.CreateImage(createInfo.extent, createInfo.imageType, createInfo.format, createInfo.samplesCount, m_mipLevels, createInfo.layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, createInfo.flags, m_image, m_allocation);
    m_allocator.CopyDataToImage(createInfo.extent, createInfo.format, m_mipLevels, createInfo.layerData, createInfo.layerCount, m_image);

    if (m_mipLevels > 1) {
        m_allocator.GenerateMipmaps(m_image, createInfo.format, m_extent, m_mipLevels, createInfo.layerCount);
    } else {
        m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_format, m_mipLevels, createInfo.layerCount);
    }

    m_imageView = prev::util::vk::CreateImageView(m_allocator.GetDevice(), m_image, m_format, m_imageViewType, m_mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, createInfo.layerCount);

    CreateSampler(static_cast<float>(m_mipLevels), createInfo.addressMode, createInfo.filteringEnabled);
}

void ImageBuffer::Resize(const VkExtent3D& extent)
{
    LOGW("ImageBuffer can not be resized - it has fixed size");
}

} // namespace prev::core::memory::image