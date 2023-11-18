#include "AbstractImageBuffer.h"

namespace prev::render::buffer::image {
AbstractImageBuffer::AbstractImageBuffer(prev::core::memory::Allocator& allocator, const VkExtent3D& ext, const VkImageType imgType, const VkFormat fmt, const VkImageCreateFlags flags, const VkSampleCountFlagBits samplesCnt, const bool mipMap, const VkImageViewType vwType, const uint32_t lrCount)
    : m_allocator(allocator)
    , m_extent(ext)
    , m_imageType(imgType)
    , m_format(fmt)
    , m_flags(flags)
    , m_sampleCount(samplesCnt)
    , m_mipMap(mipMap)
    , m_imageViewType(vwType)
    , m_layerCount(lrCount)
    , m_mipLevels(0)
    , m_allocation(VK_NULL_HANDLE)
    , m_image(VK_NULL_HANDLE)
    , m_imageView(VK_NULL_HANDLE)
{
}

AbstractImageBuffer::~AbstractImageBuffer()
{
    Destroy();
}

void AbstractImageBuffer::Destroy()
{
    if (m_image) {
        vkQueueWaitIdle(m_allocator.GetQueue());

    }

    m_image = VK_NULL_HANDLE;
}

VkImage AbstractImageBuffer::GetImage() const
{
    return m_image;
}

VkImageView AbstractImageBuffer::GetImageView() const
{
    return m_imageView;
}

VkFormat AbstractImageBuffer::GetFormat() const
{
    return m_format;
}

VkSampleCountFlagBits AbstractImageBuffer::GetSampleCount() const
{
    return m_sampleCount;
}

VkExtent3D AbstractImageBuffer::GetExtent() const
{
    return m_extent;
}

VkImageType AbstractImageBuffer::GetImageType() const
{
    return m_imageType;
}

VkImageCreateFlags AbstractImageBuffer::GetFlags() const
{
    return m_flags;
}

uint32_t AbstractImageBuffer::GetMipLevels() const
{
    return m_mipLevels;
}

uint32_t AbstractImageBuffer::GetLayerCount() const
{
    return m_layerCount;
}

VkImageViewType AbstractImageBuffer::GetViewType() const
{
    return m_imageViewType;
}
} // namespace prev::render::buffer::image