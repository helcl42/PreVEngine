#include "ImageBufferBuilder.h"
#include "ImageBufferViewBuilder.h"
#include "OwnedGfxBuffer.h"

#include "../../core/CommandsExecutor.h"
#include "../../util/MathUtils.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <memory>
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

ImageBufferBuilder& ImageBufferBuilder::SetDestroyExecutionMode(ExecutionMode executionMode)
{
    m_destroyExecutionMode = executionMode;
    return *this;
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::Build() const
{
    return BuildImpl(nullptr);
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::Build(GfxCommandEncoder commandEncoder) const
{
    return BuildImpl(commandEncoder);
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::CreateImage(uint32_t& outMipLevels, const std::shared_ptr<std::atomic<prev::core::ResourceState>>& stateFlag) const
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

    ImageBuffer::CreateInfo createInfo{};
    createInfo.extent = m_extent;
    createInfo.format = m_format;
    createInfo.texture = texture;
    createInfo.type = m_type;
    createInfo.viewType = viewType;
    createInfo.sampleCount = m_sampleCount;
    createInfo.mipLevels = mipLevels;
    createInfo.layerCount = m_layerCount;
    createInfo.usageFlags = usageFlags;
    // Initial layout reflects the texture's state right after the (pending) upload; the requested final
    // layout (m_layout) is applied when the creation work is recorded.
    createInfo.layout = m_layersData.empty() ? GFX_TEXTURE_LAYOUT_UNDEFINED : GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY;
    createInfo.deferredResourceDestroyer = &m_device.GetDeferredResourceDestroyer();
    createInfo.destroyExecutionMode = m_destroyExecutionMode;
    createInfo.stateFlag = stateFlag;

    // Build the default view up front (host-mapped textures use mapped data directly, so they get none).
    // The view only needs the texture handle and metadata, so it doesn't require a constructed ImageBuffer.
    if (!m_hostMapped) {
        createInfo.view = std::move(*ImageBufferViewBuilder{ texture, viewType, m_format, m_layerCount }
                .SetMipLevelCount(mipLevels)
                .SetArrayLayerCount(m_layerCount)
                .Build());
    }

    outMipLevels = mipLevels;
    return std::unique_ptr<ImageBuffer>(new ImageBuffer(m_device, m_queue, std::move(createInfo)));
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::BuildImpl(GfxCommandEncoder commandEncoder) const
{
    const bool hasData{ !m_layersData.empty() && m_layerDataSize > 0 };
    // Synchronous builds are usable on return (Ready); a texture built without data is allocated but
    // unpopulated (None) — its content is produced by a pass, not gated by the readiness check.
    auto state{ std::make_shared<std::atomic<prev::core::ResourceState>>(hasData ? prev::core::ResourceState::Ready : prev::core::ResourceState::None) };

    uint32_t mipLevels{};
    auto imageBuffer{ CreateImage(mipLevels, state) };

    if (hasData) {
        UploadLayerData(imageBuffer->GetTexture(), commandEncoder);
    }

    // Generate mipmaps and/or transition to the requested layout. Mipmaps are only generated here when
    // data was uploaded; otherwise the caller does it (e.g. after a compute pass fills the texture).
    // A layout of UNDEFINED means "leave as is" (e.g. render attachments).
    ImageBuffer* const img{ imageBuffer.get() };
    const GfxTextureLayout layout{ m_layout };
    if (mipLevels > 1 && hasData) {
        RecordCommands([img, layout](GfxCommandEncoder enc) {
            img->GenerateMipMaps(enc);
            img->UpdateLayout(layout, enc);
        },
            commandEncoder);
    } else if (layout != GFX_TEXTURE_LAYOUT_UNDEFINED && imageBuffer->GetLayout() != layout) {
        RecordCommands([img, layout](GfxCommandEncoder enc) {
            img->UpdateLayout(layout, enc);
        },
            commandEncoder);
    }

    return imageBuffer;
}

std::unique_ptr<ImageBuffer> ImageBufferBuilder::BuildAsync() const
{
    const bool hasData{ !m_layersData.empty() && m_layerDataSize > 0 };
    if (!hasData) {
        // Nothing to stream; an async build with no data has no benefit, so build it ready immediately.
        return BuildImpl(nullptr);
    }

    // Allocate now but leave it Creating; the upload/mipmap/layout work is recorded into a frame encoder
    // by the uploader at frame start, which then flips the state to Ready. The state is shared with the
    // resource so it survives the upload being recorded after the builder returns, and lets the resource's
    // destructor cancel the upload (Destroying) if it is dropped first.
    auto state{ std::make_shared<std::atomic<prev::core::ResourceState>>(prev::core::ResourceState::Creating) };

    uint32_t mipLevels{};
    auto imageBuffer{ CreateImage(mipLevels, state) };

    const GfxBuffer staging{ CreateLayerStagingBuffer() };
    auto copyRecorder{ MakeLayerCopyRecorder(staging, imageBuffer->GetTexture()) };

    ImageBuffer* const img{ imageBuffer.get() };
    const GfxTextureLayout layout{ m_layout };

    // One combined recorder: copy layers, then (when mipmapped) generate mips, then transition to the
    // requested layout — the same work the synchronous paths record, replayed at flush time.
    auto record{ [copyRecorder = std::move(copyRecorder), img, layout, mipLevels](GfxCommandEncoder enc) {
        copyRecorder(enc);
        if (mipLevels > 1) {
            img->GenerateMipMaps(enc);
            img->UpdateLayout(layout, enc);
        } else if (layout != GFX_TEXTURE_LAYOUT_UNDEFINED) {
            img->UpdateLayout(layout, enc);
        }
    } };

    m_device.GetDeferredResourceUploader().Enqueue(std::move(record), state, staging);

    return imageBuffer;
}

void ImageBufferBuilder::UploadLayerData(GfxTexture texture, GfxCommandEncoder commandEncoder) const
{
    const GfxBuffer staging{ CreateLayerStagingBuffer() };

    // Record the buffer->texture copy. RecordCommands runs it immediately or into the caller's encoder;
    // the lambda is invoked synchronously either way.
    RecordCommands(MakeLayerCopyRecorder(staging, texture), commandEncoder);

    // Immediate path already submitted + waited, so the staging buffer can be freed now; the recorded path
    // runs when the caller submits its encoder, so defer-destroy it to outlive that submission.
    if (commandEncoder) {
        m_device.GetDeferredResourceDestroyer().Destroy(std::make_unique<OwnedGfxBuffer>(staging));
    } else {
        gfxBufferDestroy(staging);
    }
}

GfxBuffer ImageBufferBuilder::CreateLayerStagingBuffer() const
{
    const uint32_t layerCount{ std::min(m_layerCount, static_cast<uint32_t>(m_layersData.size())) };
    const uint64_t stagingSize{ m_layerDataSize * layerCount };

    GfxBufferDescriptor stagingDesc{};
    stagingDesc.sType = GFX_STRUCTURE_TYPE_BUFFER_DESCRIPTOR;
    stagingDesc.size = stagingSize;
    stagingDesc.usage = GFX_BUFFER_USAGE_MAP_WRITE | GFX_BUFFER_USAGE_COPY_SRC;
    stagingDesc.memoryProperties = GFX_MEMORY_PROPERTY_HOST_VISIBLE | GFX_MEMORY_PROPERTY_HOST_COHERENT;

    GfxBuffer staging{};
    if (gfxDeviceCreateBuffer(m_device, &stagingDesc, &staging) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create staging buffer for texture upload");
    }

    void* mapped{ nullptr };
    if (gfxBufferMap(staging, 0, stagingSize, &mapped) != GFX_RESULT_SUCCESS) {
        gfxBufferDestroy(staging);
        throw std::runtime_error("Failed to map staging buffer for texture upload");
    }
    for (uint32_t layer = 0; layer < layerCount; ++layer) {
        memcpy(static_cast<uint8_t*>(mapped) + layer * m_layerDataSize, m_layersData[layer], m_layerDataSize);
    }
    gfxBufferUnmap(staging);

    return staging;
}

std::function<void(GfxCommandEncoder)> ImageBufferBuilder::MakeLayerCopyRecorder(GfxBuffer staging, GfxTexture texture) const
{
    const uint32_t layerCount{ std::min(m_layerCount, static_cast<uint32_t>(m_layersData.size())) };
    const GfxExtent3D extent{ m_extent };
    const uint64_t layerDataSize{ m_layerDataSize };

    // Copies each layer (UNDEFINED -> TRANSFER_DST -> SHADER_READ_ONLY). Captures only handles/POD so it is
    // safe to replay later (caller's encoder or the deferred uploader).
    return [staging, texture, layerCount, extent, layerDataSize](GfxCommandEncoder enc) {
        for (uint32_t layer = 0; layer < layerCount; ++layer) {
            GfxCopyBufferToTextureDescriptor copyDesc{};
            copyDesc.source = staging;
            copyDesc.sourceOffset = layer * layerDataSize;
            copyDesc.destination = texture;
            copyDesc.origin = { 0, 0, 0 };
            copyDesc.extent = extent;
            copyDesc.mipLevel = 0;
            copyDesc.arrayLayer = layer;
            copyDesc.finalLayout = GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY;
            gfxCommandEncoderCopyBufferToTexture(enc, &copyDesc);
        }
    };
}

void ImageBufferBuilder::RecordCommands(const std::function<void(GfxCommandEncoder)>& record, GfxCommandEncoder commandEncoder) const
{
    if (commandEncoder) {
        // Record into the caller's encoder; runs when they submit it (they order it before any use).
        record(commandEncoder);
    } else {
        // Immediate: record into a one-off encoder, submit, and wait — ready when Build() returns.
        prev::core::CommandsExecutor cmds{ m_device, m_queue };
        cmds.ExecuteImmediate(record);
    }
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
