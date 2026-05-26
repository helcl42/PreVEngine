#ifndef __IMAGE_BUFFER_VIEW_BUILDER_H__
#define __IMAGE_BUFFER_VIEW_BUILDER_H__

#include "ImageBufferView.h"

#include <memory>

namespace prev::render::buffer {

class ImageBuffer;

class ImageBufferViewBuilder final {
public:
    ImageBufferViewBuilder(const ImageBuffer& imageBuffer);

    ImageBufferViewBuilder(GfxTexture texture, GfxTextureViewType viewType, GfxFormat format, uint32_t layerCount = 1);

    ~ImageBufferViewBuilder() = default;

public:
    ImageBufferViewBuilder& SetBaseMipLevel(uint32_t baseMipLevel);

    ImageBufferViewBuilder& SetMipLevelCount(uint32_t mipLevelCount);

    ImageBufferViewBuilder& SetBaseArrayLayer(uint32_t baseArrayLayer);

    ImageBufferViewBuilder& SetArrayLayerCount(uint32_t arrayLayerCount);

    std::unique_ptr<ImageBufferView> Build() const;

private:
    GfxTexture m_texture{};

    GfxTextureViewType m_viewType{};

    GfxFormat m_format{};

    uint32_t m_layerCount{ 1 };

    uint32_t m_baseMipLevel{ 0 };

    uint32_t m_mipLevelCount{ 1 };

    uint32_t m_baseArrayLayer{ 0 };

    uint32_t m_arrayLayerCount{ 0 }; // 0 means use layer count from constructor
};

} // namespace prev::render::buffer

#endif
