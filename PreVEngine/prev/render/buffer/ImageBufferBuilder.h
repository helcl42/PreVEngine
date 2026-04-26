#ifndef __IMAGE_BUFFER_BUILDER_H__
#define __IMAGE_BUFFER_BUILDER_H__

#include "ImageBuffer.h"

#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <gfx/gfx.h>
#include <vector>

namespace prev::render::buffer {

class ImageBufferBuilder final {
public:
    ImageBufferBuilder(const prev::core::device::Device& device, const prev::core::device::Queue& queue);

    ~ImageBufferBuilder() = default;

public:
    ImageBufferBuilder& SetExtent(const GfxExtent3D& extent);

    ImageBufferBuilder& SetFormat(GfxFormat format);

    ImageBufferBuilder& SetType(GfxTextureType type);

    ImageBufferBuilder& SetViewType(GfxTextureViewType viewType);

    ImageBufferBuilder& SetSampleCount(GfxSampleCount samples);

    ImageBufferBuilder& SetMipMapEnabled(bool enabled);

    ImageBufferBuilder& SetLayerCount(uint32_t layerCount);

    ImageBufferBuilder& SetUsageFlags(GfxTextureUsageFlags usageFlags);

    ImageBufferBuilder& SetLayout(GfxTextureLayout layout);

    ImageBufferBuilder& SetLayerData(const std::vector<const uint8_t*>& layerData, uint64_t layerDataSize = 0);

    ImageBufferBuilder& SetHostMapped(bool hostMapped);

    std::unique_ptr<ImageBuffer> Build() const;

private:
    void Validate() const;

private:
    const prev::core::device::Device& m_device;

    const prev::core::device::Queue& m_queue;

    GfxExtent3D m_extent{};

    GfxFormat m_format{ GFX_FORMAT_UNDEFINED };

    GfxTextureType m_type{ GFX_TEXTURE_TYPE_MAX_ENUM };

    GfxTextureViewType m_viewType{ GFX_TEXTURE_VIEW_TYPE_MAX_ENUM };

    GfxSampleCount m_sampleCount{ GFX_SAMPLE_COUNT_1 };

    bool m_mipMapEnabled{ false };

    uint32_t m_layerCount{ 1 };

    GfxTextureUsageFlags m_usageFlags{};

    GfxTextureLayout m_layout{ GFX_TEXTURE_LAYOUT_UNDEFINED };

    std::vector<const uint8_t*> m_layersData;

    uint64_t m_layerDataSize{ 0 };

    bool m_hostMapped{ false };
};

} // namespace prev::render::buffer

#endif