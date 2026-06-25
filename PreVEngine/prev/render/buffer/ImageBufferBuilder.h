#ifndef __IMAGE_BUFFER_BUILDER_H__
#define __IMAGE_BUFFER_BUILDER_H__

#include "ExecutionMode.h"
#include "ImageBuffer.h"

#include "../../core/DeferredResourceDestroyer.h"
#include "../../core/DeferredResourceUploader.h"
#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <atomic>
#include <functional>
#include <memory>
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

    ImageBufferBuilder& SetDestroyExecutionMode(ExecutionMode executionMode);

    // Builds the image and runs its upload/layout/mipmap work immediately (submit + wait); the returned
    // image is ready to use. Use this off the frame loop or whenever a synchronous result is fine.
    std::unique_ptr<ImageBuffer> Build() const;

    // Builds the image and records its upload/layout/mipmap work into the given command encoder instead
    // of submitting — no wait. The image is ready once the caller submits that encoder, so the caller
    // MUST record this before the work that uses the image and OUTSIDE a render pass (copies/barriers are
    // illegal inside one). Staging is defer-destroyed via the resource manager.
    std::unique_ptr<ImageBuffer> Build(GfxCommandEncoder commandEncoder) const;

    // Builds the image immediately but leaves it unpopulated and NOT ready: the upload/mipmap/layout work
    // is queued with the device's DeferredResourceUploader and recorded into a frame's encoder at frame
    // start. The image flips to ready once flushed (pops in a frame later). Consumers MUST skip it while
    // IsReady() is false. The returned image must be kept alive until it becomes ready. No queue stall.
    std::unique_ptr<ImageBuffer> BuildAsync() const;

private:
    void Validate() const;

    // Shared implementation: commandEncoder == nullptr -> immediate (submit + wait); otherwise records
    // the creation work into the encoder.
    std::unique_ptr<ImageBuffer> BuildImpl(GfxCommandEncoder commandEncoder) const;

    // Allocates the texture + view and constructs the (unpopulated) ImageBuffer with the given initial/
    // shared lifecycle state (Ready/None for sync builds, a shared Creating state for async builds).
    // Outputs the computed mip level count so the caller can record mipmap generation.
    std::unique_ptr<ImageBuffer> CreateImage(uint32_t& outMipLevels, const std::shared_ptr<std::atomic<prev::core::ResourceState>>& stateFlag) const;

    void UploadLayerData(GfxTexture texture, GfxCommandEncoder commandEncoder) const;

    // Packs all layers into one host-visible staging buffer (created + filled now). Caller owns it.
    GfxBuffer CreateLayerStagingBuffer() const;

    // Builds the staging->texture copy recorder (UNDEFINED -> TRANSFER_DST -> SHADER_READ_ONLY).
    std::function<void(GfxCommandEncoder)> MakeLayerCopyRecorder(GfxBuffer staging, GfxTexture texture) const;

    void RecordCommands(const std::function<void(GfxCommandEncoder)>& record, GfxCommandEncoder commandEncoder) const;

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

    std::vector<const uint8_t*> m_layersData{};

    uint64_t m_layerDataSize{ 0 };

    bool m_hostMapped{ false };

    ExecutionMode m_destroyExecutionMode{ ExecutionMode::Auto };
};

} // namespace prev::render::buffer

#endif