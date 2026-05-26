#include "ImageBufferViewBuilder.h"
#include "ImageBuffer.h"

namespace prev::render::buffer {

ImageBufferViewBuilder::ImageBufferViewBuilder(const ImageBuffer& imageBuffer)
    : m_texture{ imageBuffer.GetTexture() }
    , m_viewType{ imageBuffer.GetTextureViewType() }
    , m_format{ imageBuffer.GetFormat() }
    , m_layerCount{ imageBuffer.GetLayerCount() }
{
}

ImageBufferViewBuilder::ImageBufferViewBuilder(GfxTexture texture, GfxTextureViewType viewType, GfxFormat format, uint32_t layerCount)
    : m_texture{ texture }
    , m_viewType{ viewType }
    , m_format{ format }
    , m_layerCount{ layerCount }
{
}

ImageBufferViewBuilder& ImageBufferViewBuilder::SetBaseMipLevel(uint32_t baseMipLevel)
{
    m_baseMipLevel = baseMipLevel;
    return *this;
}

ImageBufferViewBuilder& ImageBufferViewBuilder::SetMipLevelCount(uint32_t mipLevelCount)
{
    m_mipLevelCount = mipLevelCount;
    return *this;
}

ImageBufferViewBuilder& ImageBufferViewBuilder::SetBaseArrayLayer(uint32_t baseArrayLayer)
{
    m_baseArrayLayer = baseArrayLayer;
    return *this;
}

ImageBufferViewBuilder& ImageBufferViewBuilder::SetArrayLayerCount(uint32_t arrayLayerCount)
{
    m_arrayLayerCount = arrayLayerCount;
    return *this;
}

std::unique_ptr<ImageBufferView> ImageBufferViewBuilder::Build() const
{
    const uint32_t arrayLayerCount = m_arrayLayerCount > 0 ? m_arrayLayerCount : m_layerCount;

    GfxTextureViewDescriptor viewDesc{};
    viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
    viewDesc.viewType = m_viewType;
    viewDesc.format = m_format;
    viewDesc.baseMipLevel = m_baseMipLevel;
    viewDesc.mipLevelCount = m_mipLevelCount;
    viewDesc.baseArrayLayer = m_baseArrayLayer;
    viewDesc.arrayLayerCount = arrayLayerCount;

    GfxTextureView view{};
    gfxTextureCreateView(m_texture, &viewDesc, &view);
    return std::make_unique<ImageBufferView>(view);
}

} // namespace prev::render::buffer
