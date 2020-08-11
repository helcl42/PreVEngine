#include "AbstractImageBuffer.h"
#include "../../instance/Validation.h"

namespace PreVEngine {
AbstractImageBuffer::AbstractImageBuffer(Allocator& allocator)
    : m_allocator(allocator)
    , m_allocation(nullptr)
    , m_image(nullptr)
    , m_extent({ 0, 0, 0 })
    , m_imageType(VkImageType::VK_IMAGE_TYPE_2D)
    , m_flags()
    , m_format(VkFormat::VK_FORMAT_UNDEFINED)
    , m_imageView(nullptr)
    , m_sampler(nullptr)
    , m_mipLevels(0)
    , m_layerCount(0)
    , m_imageViewType()
{
}

AbstractImageBuffer::~AbstractImageBuffer()
{
    Destroy();
}

void AbstractImageBuffer::CreateSampler(const float maxLod, const VkSamplerAddressMode addressMode, const bool enableFiltering)
{
    if (m_sampler) {
        vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
    }

    VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    if (enableFiltering) {
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = 16;
    } else {
        samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
        samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1;
    }
    samplerCreateInfo.addressModeU = addressMode;
    samplerCreateInfo.addressModeV = addressMode;
    samplerCreateInfo.addressModeW = addressMode;
    samplerCreateInfo.mipLodBias = 0;

    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.minLod = 0;
    samplerCreateInfo.maxLod = maxLod;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    VKERRCHECK(vkCreateSampler(m_allocator.GetDevice(), &samplerCreateInfo, nullptr, &m_sampler));
}

void AbstractImageBuffer::UpdateSampler(const VkSamplerCreateInfo& samplerInfo)
{
    if (m_sampler) {
        vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
    }

    VKERRCHECK(vkCreateSampler(m_allocator.GetDevice(), &samplerInfo, nullptr, &m_sampler));
}

void AbstractImageBuffer::Destroy()
{
    vkQueueWaitIdle(m_allocator.GetQueue());

    if (m_sampler) {
        vkDestroySampler(m_allocator.GetDevice(), m_sampler, nullptr);
    }

    if (m_image) {
        m_allocator.DestroyImage(m_image, m_imageView, m_allocation);
    }

    m_image = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
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

VkSampler AbstractImageBuffer::GetSampler() const
{
    return m_sampler;
}

VkFormat AbstractImageBuffer::GetFormat() const
{
    return m_format;
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
} // namespace PreVEngine