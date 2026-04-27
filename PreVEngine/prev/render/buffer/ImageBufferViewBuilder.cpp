#include "ImageBufferViewBuilder.h"

namespace prev::render::buffer {

ImageBufferViewBuilder::ImageBufferViewBuilder(const ImageBuffer& imageBuffer)
    : m_imageBuffer{ imageBuffer }
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
    const uint32_t arrayLayerCount = m_arrayLayerCount > 0 ? m_arrayLayerCount : m_imageBuffer.GetLayerCount();

    GfxTextureViewDescriptor viewDesc{};
    viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
    viewDesc.viewType = m_imageBuffer.GetTextureViewType();
    viewDesc.format = m_imageBuffer.GetFormat();
    viewDesc.baseMipLevel = m_baseMipLevel;
    viewDesc.mipLevelCount = m_mipLevelCount;
    viewDesc.baseArrayLayer = m_baseArrayLayer;
    viewDesc.arrayLayerCount = arrayLayerCount;

    GfxTextureView view{};
    gfxTextureCreateView(m_imageBuffer.GetTexture(), &viewDesc, &view);
    return std::make_unique<ImageBufferView>(view);
}

} // namespace prev::render::buffer
