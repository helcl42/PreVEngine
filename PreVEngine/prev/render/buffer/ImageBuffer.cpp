#include "ImageBuffer.h"

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

    vkDestroyImageView(m_allocator.GetDevice(), m_view, VK_NULL_HANDLE);
    m_allocator.DestroyImage(m_image, m_allocation);
}

void ImageBuffer::UpdateLayout(const VkImageLayout newLayout)
{
    // TODO -> use task engine for this !?
    m_allocator.TransitionImageLayout(m_image, m_layout, newLayout, m_mipLevels, m_aspectMask, m_layerCount);
    m_layout = newLayout;
}

void ImageBuffer::UpdateLayout(const VkImageLayout newLayout, VkCommandBuffer commandBuffer)
{
    util::vk::TransitionImageLayout(commandBuffer, m_image, m_layout, newLayout, m_mipLevels, m_aspectMask, m_layerCount);
    m_layout = newLayout;
}

void ImageBuffer::GenerateMipMaps(const VkImageLayout newLLayout)
{
    m_mipLevels = prev::util::math::Log2(std::max(m_extent.width, m_extent.height)) + 1;
    m_allocator.TransitionImageLayout(m_image, m_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels, m_aspectMask, m_layerCount);
    m_allocator.GenerateMipmaps(m_image, m_format, m_extent, m_mipLevels, m_layerCount, m_aspectMask, newLLayout);
    m_layout = newLLayout;
}

void ImageBuffer::GenerateMipMaps(const VkImageLayout newLLayout, VkCommandBuffer commandBuffer)
{
    m_mipLevels = prev::util::math::Log2(std::max(m_extent.width, m_extent.height)) + 1;
    util::vk::TransitionImageLayout(commandBuffer, m_image, m_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels, m_aspectMask, m_layerCount);
    util::vk::GenerateMipmaps(commandBuffer, m_image, m_extent, m_mipLevels, m_layerCount, m_aspectMask, newLLayout);
    m_layout = newLLayout;
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
    return m_createFlags;
}

VkImageLayout ImageBuffer::GetLayout() const
{
    return m_layout;
}

ImageBuffer::operator VkImage() const
{
    return m_image;
}

} // namespace prev::render::buffer