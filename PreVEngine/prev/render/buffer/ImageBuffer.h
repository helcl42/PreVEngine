#ifndef __IMAGE_BUFFER_H__
#define __IMAGE_BUFFER_H__

#include "ExecutionMode.h"
#include "ImageBufferView.h"

#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/IResource.h"
#include "../../core/device/Device.h"

#include <atomic>
#include <memory>

namespace prev::render::buffer {
class ImageBufferBuilder;

class ImageBuffer final : public prev::core::IResource {
private:
    struct CreateInfo {
        GfxExtent3D extent{};
        GfxFormat format{};
        GfxTexture texture{};
        ImageBufferView view{}; // default (empty) view (null for host-mapped textures); moved into the buffer
        GfxTextureType type{};
        GfxTextureViewType viewType{};
        GfxSampleCount sampleCount{};
        uint32_t mipLevels{};
        uint32_t layerCount{};
        GfxTextureUsageFlags usageFlags{};
        GfxTextureLayout layout{ GFX_TEXTURE_LAYOUT_UNDEFINED };
        prev::core::DeferredResourceDestroyer* deferredResourceDestroyer{};
        ExecutionMode destroyExecutionMode{ ExecutionMode::Auto };
        // Initial/shared lifecycle state. Null defaults to Ready; async builds share a Creating state
        // with the uploader so it survives the resource being dropped before the upload is flushed.
        std::shared_ptr<std::atomic<prev::core::ResourceState>> stateFlag{};
    };

    ImageBuffer(GfxDevice device, GfxQueue queue, CreateInfo&& createInfo);

public:
    ~ImageBuffer() override;

public:
    prev::core::ResourceState GetState() const override;

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
    bool IsDeferred() const;

    static void RecordLayoutTransition(GfxCommandEncoder commandEncoder, GfxTexture texture, uint32_t mipLevels, uint32_t layerCount, GfxTextureLayout oldLayout, GfxTextureLayout newLayout);

private:
    GfxDevice m_device{};

    GfxQueue m_queue{};

    GfxExtent3D m_extent{};

    GfxFormat m_format{};

    GfxTexture m_texture{};

    ImageBufferView m_view{};

    GfxTextureType m_type{};

    GfxTextureViewType m_viewType{};

    GfxSampleCount m_samplesCount{};

    uint32_t m_mipLevels{};

    uint32_t m_layerCount{};

    GfxTextureUsageFlags m_usageFlags{};

    GfxTextureLayout m_layout{ GFX_TEXTURE_LAYOUT_UNDEFINED };

    void* m_mappedData{};

    prev::core::DeferredResourceDestroyer* m_deferredResourceDestroyer{};

    ExecutionMode m_destroyExecutionMode{ ExecutionMode::Auto };

    std::shared_ptr<std::atomic<prev::core::ResourceState>> m_state{};
};

} // namespace prev::render::buffer

#endif
