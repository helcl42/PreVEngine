#include "ImageStorageBuffer.h"

#include "../../../util/MathUtils.h"
#include "../../../util/VkUtils.h"
#include "../../instance/Validation.h"

namespace prev::core::memory::image {
ImageStorageBuffer::ImageStorageBuffer(Allocator& allocator)
    : AbstractImageBuffer(allocator)
{
}

void ImageStorageBuffer::Create(const ImageBufferCreateInfo& createInfo)
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

    m_allocator.CreateImage(createInfo.extent, m_imageType, m_format, m_sampleCount, m_mipLevels, m_layerCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, m_flags, m_image, m_allocation);
    m_allocator.TransitionImageLayout(m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, m_format, m_mipLevels, m_layerCount);

    m_imageView = prev::util::vk::CreateImageView(m_allocator.GetDevice(), m_image, m_format, m_imageViewType, m_mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, m_layerCount);

    CreateSampler(static_cast<float>(m_mipLevels), createInfo.addressMode, createInfo.filteringEnabled);
}

void ImageStorageBuffer::Resize(const VkExtent3D& extent)
{
    LOGW("ImageStorageBuffer can not be resized - it has fixed size");
}
} // namespace prev::core::memory::image