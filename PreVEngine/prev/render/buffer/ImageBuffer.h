#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "ImageBufferView.h"

#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

namespace prev::render::buffer {
class ImageBufferBuilder;

class ImageBuffer final {
private:
    ImageBuffer(GfxDevice device, GfxQueue queue);

public:
    ~ImageBuffer();

public:
    void UpdateLayout(const GfxTextureLayout newLayout, GfxCommandEncoder commandEncoder);

    void GenerateMipMaps(GfxCommandEncoder commandEncoder);

    void Copy(ImageBuffer& dstImage, GfxCommandEncoder commandEncoder);

    GfxExtent3D GetExtent() const;

    GfxFormat GetFormat() const;

    GfxTexture GetTexture() const;

    GfxTextureType GetTextureType() const;

    const ImageBufferView& GetTextureView() const;

    GfxTextureViewType GetTextureViewType() const;

    GfxSampleCount GetSampleCount() const;

    uint32_t GetMipLevels() const;

    uint32_t GetLayerCount() const;

    GfxTextureUsageFlags GetUsageFlags() const;

    GfxTextureLayout GetLayout() const;

    void* GetMappedData() const;

public:
    operator GfxTexture() const;

public:
    friend class ImageBufferBuilder;

private:
    GfxDevice m_device;

    GfxQueue m_queue;

    GfxExtent3D m_extent{};

    GfxFormat m_format{};

    GfxTexture m_texture{};

    ImageBufferView m_view{ nullptr };

    GfxTextureType m_type{};

    GfxTextureViewType m_viewType{};

    GfxSampleCount m_samplesCount{};

    uint32_t m_mipLevels{};

    uint32_t m_layerCount{};

    GfxTextureUsageFlags m_usageFlags{};

    GfxTextureLayout m_layout{ GFX_TEXTURE_LAYOUT_UNDEFINED };

    void* m_mappedData{};
};

} // namespace prev::render::buffer

#endif
