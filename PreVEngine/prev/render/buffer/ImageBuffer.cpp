#include "ImageBuffer.h"

#include "../../core/Formats.h"
#include "../../util/MathUtils.h"
#include "../../util/VkUtils.h"

namespace prev::render::buffer {

ImageBuffer::ImageBuffer(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

ImageBuffer::~ImageBuffer()
{
    m_allocator.GetQueue().WaitIdle();

    if (m_view) {
        vkDestroyImageView(m_allocator.GetDevice(), m_view, VK_NULL_HANDLE);
    }
    m_allocator.DestroyImage(m_image, m_allocation);
}

void ImageBuffer::UpdateLayout(const VkImageLayout newLayout, VkCommandBuffer commandBuffer)
{
    util::vk::TransitionImageLayout(commandBuffer, m_image, m_layout, newLayout, m_mipLevels, m_aspectMask, m_layerCount);
    m_layout = newLayout;
}

void ImageBuffer::GenerateMipMaps(VkCommandBuffer commandBuffer)
{
    const auto newLayout{ m_layout != VK_IMAGE_LAYOUT_UNDEFINED ? m_layout : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL };

    const auto filter{ prev::core::format::HasDepthComponent(m_format) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR };

    m_mipLevels = prev::util::math::Log2(std::max(m_extent.width, m_extent.height)) + 1;

    util::vk::TransitionImageLayout(commandBuffer, m_image, m_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels, m_aspectMask, m_layerCount);
    util::vk::GenerateMipmaps(commandBuffer, m_image, m_extent, m_mipLevels, m_layerCount, m_aspectMask, filter, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    util::vk::TransitionImageLayout(commandBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout, m_mipLevels, m_aspectMask, m_layerCount);

    m_layout = newLayout;
}

void ImageBuffer::Copy(ImageBuffer& dstImage, VkCommandBuffer commandBuffer)
{
    const auto newLayout{ m_layout != VK_IMAGE_LAYOUT_UNDEFINED ? m_layout : VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };

    const auto filter{ prev::core::format::HasDepthComponent(m_format) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR };

    util::vk::TransitionImageLayout(commandBuffer, m_image, m_layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_mipLevels, m_aspectMask, m_layerCount);
    prev::util::vk::CopyImage(commandBuffer, m_image, m_extent, m_layerCount, m_aspectMask, filter, dstImage.GetLayout(), dstImage);
    util::vk::TransitionImageLayout(commandBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newLayout, m_mipLevels, m_aspectMask, m_layerCount);

    m_layout = newLayout;
}

VkExtent3D ImageBuffer::GetExtent() const
{
    return m_extent;
}

VkFormat ImageBuffer::GetFormat() const
{
    return m_format;
}

VkImage ImageBuffer::GetImage() const
{
    return m_image;
}

VkImageType ImageBuffer::GetImageType() const
{
    return m_type;
}

VkImageView ImageBuffer::GetImageView() const
{
    return m_view;
}

VkImageViewType ImageBuffer::GetImageViewType() const
{
    return m_viewType;
}

VkImageAspectFlags ImageBuffer::GetAspectMask() const
{
    return m_aspectMask;
}

VkSampleCountFlagBits ImageBuffer::GetSampleCount() const
{
    return m_samplesCount;
}

uint32_t ImageBuffer::GetMipLevels() const
{
    return m_mipLevels;
}

uint32_t ImageBuffer::GetLayerCount() const
{
    return m_layerCount;
}

VkImageCreateFlags ImageBuffer::GetCreateFlags() const
{
    return m_createFlags;
}

VkImageUsageFlags ImageBuffer::GetUsageFlags() const
{
    return m_usageFlags;
}

VkImageLayout ImageBuffer::GetLayout() const
{
    return m_layout;
}

void* ImageBuffer::GetMappedData() const
{
    return m_mappedData;
}

ImageBuffer::operator VkImage() const
{
    return m_image;
}

} // namespace prev::render::buffer
