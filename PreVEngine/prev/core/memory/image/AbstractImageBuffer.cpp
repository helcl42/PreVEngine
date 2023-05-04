#include "AbstractImageBuffer.h"
#include "../../instance/Validation.h"

namespace prev::core::memory::image {
AbstractImageBuffer::AbstractImageBuffer(Allocator& allocator)
    : m_allocator(allocator)
    , m_allocation(nullptr)
    , m_image(nullptr)
    , m_extent({ 0, 0, 0 })
    , m_imageType(VkImageType::VK_IMAGE_TYPE_2D)
    , m_flags()
    , m_format(VkFormat::VK_FORMAT_UNDEFINED)
    , m_sampleCount(VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
    , m_imageView(nullptr)
    , m_mipLevels(0)
    , m_layerCount(0)
    , m_imageViewType()
{
}

AbstractImageBuffer::~AbstractImageBuffer()
{
    Destroy();
}

void AbstractImageBuffer::Destroy()
{
    vkQueueWaitIdle(m_allocator.GetQueue());

    if (m_image) {
        m_allocator.DestroyImage(m_image, m_imageView, m_allocation);
    }

    m_image = VK_NULL_HANDLE;
    m_extent = {};
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
} // namespace prev::core::memory::image