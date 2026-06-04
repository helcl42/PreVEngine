#include "ImageBufferBuilder.h"
#include "ImageBufferViewBuilder.h"

#include "../../core/CommandsExecutor.h"
#include "../../util/MathUtils.h"

#include <stdexcept>

namespace prev::render::buffer {
namespace {
    GfxTextureViewType DeduceViewTypeFromTextureType(const GfxTextureType type)
    {
        switch (type) {
        case GFX_TEXTURE_TYPE_1D:
            return GFX_TEXTURE_VIEW_TYPE_1D;
        case GFX_TEXTURE_TYPE_3D:
            return GFX_TEXTURE_VIEW_TYPE_3D;
        case GFX_TEXTURE_TYPE_2D:
        default:
            return GFX_TEXTURE_VIEW_TYPE_2D;
        }
    }
} // namespace

ImageBufferBuilder::ImageBufferBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue)
    : m_device{ device }
    , m_queue{ queue }
{
}

ImageBufferBuilder& ImageBufferBuilder::SetExtent(const GfxExtent3D& extent)
{
    m_extent = extent;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetFormat(GfxFormat format)
{
    m_format = format;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetType(GfxTextureType type)
{
    m_type = type;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetViewType(GfxTextureViewType viewType)
{
    m_viewType = viewType;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetSampleCount(GfxSampleCount samples)
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

ImageBufferBuilder& ImageBufferBuilder::SetUsageFlags(GfxTextureUsageFlags usageFlags)
{
    m_usageFlags = usageFlags;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetLayout(GfxTextureLayout layout)
{
    m_layout = layout;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetLayerData(const std::vector<const uint8_t*>& layerData, uint64_t layerDataSize)
{
    m_layersData = layerData;
    m_layerDataSize = layerDataSize;
    return *this;
}

ImageBufferBuilder& ImageBufferBuilder::SetHostMapped(bool hostMapped)
{
    m_hostMapped = hostMapped;
    return *this;
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::Build() const
{
    Validate();

    const auto viewType{ m_viewType == GFX_TEXTURE_VIEW_TYPE_MAX_ENUM ? DeduceViewTypeFromTextureType(m_type) : m_viewType };
    const auto mipLevels{ m_mipMapEnabled ? prev::util::math::Log2(std::max(m_extent.width, m_extent.height)) + 1 : 1 };

    GfxTextureUsageFlags usageFlags{ m_usageFlags };
    if (!m_layersData.empty()) {
        usageFlags |= GFX_TEXTURE_USAGE_COPY_DST;
    }
    if (mipLevels > 1) {
        usageFlags |= GFX_TEXTURE_USAGE_COPY_SRC;
        usageFlags |= GFX_TEXTURE_USAGE_COPY_DST;
        usageFlags |= GFX_TEXTURE_USAGE_RENDER_ATTACHMENT; // WebGPU backend generates mipmaps via render passes
    }

    GfxTextureDescriptor texDesc{};
    texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
    texDesc.type = m_type;
    texDesc.size = m_extent;
    texDesc.arrayLayerCount = m_layerCount;
    texDesc.mipLevelCount = mipLevels;
    texDesc.sampleCount = m_sampleCount;
    texDesc.format = m_format;
    texDesc.usage = usageFlags;

    GfxTexture texture{};
    if (gfxDeviceCreateTexture(m_device, &texDesc, &texture) != GFX_RESULT_SUCCESS || !texture) {
        throw std::runtime_error("Could not create texture");
    }

    // Upload layer data per layer
    if (!m_layersData.empty() && m_layerDataSize > 0) {
        for (uint32_t layer = 0; layer < m_layerCount && layer < static_cast<uint32_t>(m_layersData.size()); ++layer) {
            GfxOrigin3D origin{ 0, 0, 0 };
            gfxQueueWriteTexture(m_queue, texture, &origin, &m_extent, 0 /* mipLevel */, layer, m_layersData[layer], m_layerDataSize, 0, GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY);
        }
    }

    auto imageBuffer{ std::unique_ptr<ImageBuffer>(new ImageBuffer(m_device, m_queue)) };
    imageBuffer->m_extent = m_extent;
    imageBuffer->m_format = m_format;
    imageBuffer->m_mipLevels = mipLevels;
    imageBuffer->m_texture = texture;
    imageBuffer->m_type = m_type;
    imageBuffer->m_view = ImageBufferView{ nullptr };
    imageBuffer->m_viewType = viewType;
    imageBuffer->m_samplesCount = m_sampleCount;
    imageBuffer->m_layerCount = m_layerCount;
    imageBuffer->m_usageFlags = usageFlags;
    imageBuffer->m_layout = m_layersData.empty() ? GFX_TEXTURE_LAYOUT_UNDEFINED : GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY;

    // Create default texture view via the view builder.
    // Skip for host-mapped textures (they use mapped data directly).
    if (!m_hostMapped) {
        auto defaultView = ImageBufferViewBuilder{ *imageBuffer }
                               .SetMipLevelCount(mipLevels)
                               .SetArrayLayerCount(m_layerCount)
                               .Build();
        imageBuffer->m_view = std::move(*defaultView);
    }

    // Transition to requested layout and optionally generate mipmaps.
    // Only generate mipmaps during build when data was uploaded; otherwise the caller
    // is responsible (e.g. after a compute pass fills the texture).
    // Skip transition entirely if layout is UNDEFINED (e.g. render attachments).
    if (mipLevels > 1 && !m_layersData.empty()) {
        prev::core::CommandsExecutor cmds{ m_device, m_queue };
        cmds.ExecuteImmediate([&](GfxCommandEncoder enc) {
            imageBuffer->GenerateMipMaps(enc);
            imageBuffer->UpdateLayout(m_layout, enc);
        });
    } else if (m_layout != GFX_TEXTURE_LAYOUT_UNDEFINED && imageBuffer->m_layout != m_layout) {
        prev::core::CommandsExecutor cmds{ m_device, m_queue };
        cmds.ExecuteImmediate([&](GfxCommandEncoder enc) {
            imageBuffer->UpdateLayout(m_layout, enc);
        });
    }

    return imageBuffer;
}

void ImageBufferBuilder::Validate() const
{
    if (m_extent.width == 0 || m_extent.height == 0 || m_extent.depth == 0) {
        throw std::runtime_error("Invalid image buffer extent - all components must have nonzero value.");
    }

    if (m_format == GFX_FORMAT_UNDEFINED) {
        throw std::runtime_error("Invalid image format - undefined.");
    }

    if (m_type == GFX_TEXTURE_TYPE_MAX_ENUM) {
        throw std::runtime_error("Invalid image type - undefined.");
    }
}

} // namespace prev::render::buffer
