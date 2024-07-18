#include "ImageBufferBuilder.h"

#include "../../core/CommandsExecutor.h"
#include "../../core/Formats.h"
#include "../../util/MathUtils.h"
#include "../../util/VkUtils.h"

namespace prev::render::buffer {
namespace {
    VkImageAspectFlags DeduceApectMaskFromFormat(const VkFormat format)
    {
        VkImageAspectFlags result{ VK_IMAGE_ASPECT_NONE_KHR };
        if (core::format::HasDepthComponent(format)) {
            result |= VK_IMAGE_ASPECT_DEPTH_BIT;
            if (core::format::HasStencilComponent(format)) {
                result |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            result |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        return result;
    }

    VkImageViewType DeduceImageViewTypeFromImageType(const VkImageType imageType)
    {
        switch (imageType) {
        case VK_IMAGE_TYPE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case VK_IMAGE_TYPE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        case VK_IMAGE_TYPE_2D:
        default:
            return VK_IMAGE_VIEW_TYPE_2D;
        }
    }
} // namespace

ImageBufferBuilder::ImageBufferBuilder(prev::core::memory::Allocator& allocator)
    : m_allocator{ allocator }
{
}

ImageBufferBuilder& ImageBufferBuilder::SetExtent(const VkExtent3D& extent)
{
    m_extent = extent;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetFormat(VkFormat format)
{
    m_format = format;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetType(VkImageType type)
{
    m_type = type;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetViewType(VkImageViewType viewType)
{
    m_viewType = viewType;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetSampleCount(VkSampleCountFlagBits samples)
{
    m_sampleCount = samples;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetMipMapEnabled(bool enabled)
{
    m_mipMapEnabled = enabled;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetLayerCount(uint32_t layerCount)
{
    m_layerCount = layerCount;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetCreateFlags(VkImageCreateFlags createFlags)
{
    m_createFlags = createFlags;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetUsageFlags(VkImageUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetLayout(VkImageLayout layout)
{
    m_layout = layout;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetAspectMask(VkImageAspectFlags aspectMask)
{
    m_aspectMask = aspectMask;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SeTiling(VkImageTiling tiling)
{
    m_tiling = tiling;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetLayerData(const std::vector<const uint8_t*>& layerData)
{
    m_layersData = layerData;
    return *this;
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::Build() const
{
    if (m_extent.width == 0 || m_extent.height == 0 || m_extent.depth == 0) {
        LOGE("Invalid image buffer extent - all components must have nonzero value.");
    }

    if (m_format == VK_FORMAT_UNDEFINED) {
        LOGE("Invalid image format - undefined.");
    }

    if (m_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        LOGE("Invalid image layout - undefined.");
    }

    if (m_type == VK_IMAGE_TYPE_MAX_ENUM) {
        LOGE("Invalid image type - undefined.");
    }

    // some logic is automatic to minimize boiler plate code for common usa cases
    const auto imageViewType{ m_viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM ? DeduceImageViewTypeFromImageType(m_type) : m_viewType };
    const auto aspectMask{ m_aspectMask == VK_IMAGE_ASPECT_NONE_KHR ? DeduceApectMaskFromFormat(m_format) : m_aspectMask };
    const auto mipMapLevels{ m_mipMapEnabled ? prev::util::math::Log2(std::max(m_extent.width, m_extent.height)) + 1 : 1 };

    VkImage image;
    VmaAllocation allocation;
    m_allocator.CreateImage(m_extent, m_type, m_format, m_sampleCount, mipMapLevels, m_layerCount, m_tiling, m_usageFlags, m_createFlags, image, allocation);

    auto imageView{ prev::util::vk::CreateImageView(m_allocator.GetDevice(), image, m_format, imageViewType, mipMapLevels, aspectMask, m_layerCount) };

    auto imageBuffer{ std::make_unique<ImageBuffer>(m_allocator) };
    imageBuffer->m_extent = m_extent;
    imageBuffer->m_format = m_format;
    imageBuffer->m_mipLevels = mipMapLevels;
    imageBuffer->m_image = image;
    imageBuffer->m_allocation = allocation;
    imageBuffer->m_type = m_type;
    imageBuffer->m_view = imageView;
    imageBuffer->m_viewType = imageViewType;
    imageBuffer->m_aspectMask = aspectMask;
    imageBuffer->m_samplesCount = m_sampleCount;
    imageBuffer->m_mipLevels = mipMapLevels;
    imageBuffer->m_layerCount = m_layerCount;
    imageBuffer->m_createFlags = m_createFlags;
    imageBuffer->m_usageFlags = m_usageFlags;
    imageBuffer->m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

    prev::core::CommandsExecutor commandsExecutor{ m_allocator.GetDevice(), m_allocator.GetQueue() };
    commandsExecutor.ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        if (!m_layersData.empty()) {
            imageBuffer->UpdateLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBuffer);
        }
    });

    if (!m_layersData.empty()) {
        m_allocator.CopyDataToImage(m_extent, m_format, m_layersData, m_layerCount, image);
    }

    commandsExecutor.ExecuteImmediate([&](VkCommandBuffer commandBuffer) {
        if (!m_layersData.empty()) {
            if (mipMapLevels > 1) {
                // prev::util::vk::GetFormatProperties(); // check if possible -> is linear sanpler available?
                imageBuffer->GenerateMipMaps(m_layout, commandBuffer);
            } else {
                imageBuffer->UpdateLayout(m_layout, commandBuffer);
            }
        } else {
            imageBuffer->UpdateLayout(m_layout, commandBuffer);
        }
    });

    return imageBuffer;
}

} // namespace prev::render::buffer