#include "ImageBuffer.h"

#include "../../core/DeferredResourceDestroyer.h"

#include <cassert>
#include <memory>
#include <utility>

namespace prev::render::buffer {
namespace {
    // Deduce appropriate pipeline stage and access mask for a given texture layout.
    void GetLayoutBarrierParams(const GfxTextureLayout layout, GfxPipelineStageFlags& stage, GfxAccessFlags& access)
    {
        switch (layout) {
        case GFX_TEXTURE_LAYOUT_UNDEFINED:
            stage = GFX_PIPELINE_STAGE_TOP_OF_PIPE;
            access = GFX_ACCESS_NONE;
            break;
        case GFX_TEXTURE_LAYOUT_TRANSFER_DST:
            stage = GFX_PIPELINE_STAGE_TRANSFER;
            access = GFX_ACCESS_TRANSFER_WRITE;
            break;
        case GFX_TEXTURE_LAYOUT_TRANSFER_SRC:
            stage = GFX_PIPELINE_STAGE_TRANSFER;
            access = GFX_ACCESS_TRANSFER_READ;
            break;
        case GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY:
            stage = GFX_PIPELINE_STAGE_FRAGMENT_SHADER | GFX_PIPELINE_STAGE_COMPUTE_SHADER;
            access = GFX_ACCESS_SHADER_READ;
            break;
        case GFX_TEXTURE_LAYOUT_GENERAL:
            stage = GFX_PIPELINE_STAGE_COMPUTE_SHADER;
            access = GFX_ACCESS_SHADER_READ | GFX_ACCESS_SHADER_WRITE;
            break;
        case GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT:
            stage = GFX_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
            access = GFX_ACCESS_COLOR_ATTACHMENT_READ | GFX_ACCESS_COLOR_ATTACHMENT_WRITE;
            break;
        case GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT:
            stage = GFX_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS | GFX_PIPELINE_STAGE_LATE_FRAGMENT_TESTS;
            access = GFX_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ | GFX_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
            break;
        case GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_READ_ONLY:
            stage = GFX_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS;
            access = GFX_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ;
            break;
        case GFX_TEXTURE_LAYOUT_PRESENT_SRC:
            stage = GFX_PIPELINE_STAGE_BOTTOM_OF_PIPE;
            access = GFX_ACCESS_MEMORY_READ;
            break;
        default:
            stage = GFX_PIPELINE_STAGE_ALL_COMMANDS;
            access = GFX_ACCESS_MEMORY_READ | GFX_ACCESS_MEMORY_WRITE;
            break;
        }
    }

    // Owns a texture and its view, destroying both when it dies. Handed to the deferred resource
    // manager so the GPU handles outlive any in-flight work that still references them.
    class OwnedGfxTexture final {
    public:
        OwnedGfxTexture(ImageBufferView&& view, GfxTexture texture)
            : m_view{ std::move(view) }
            , m_texture{ texture }
        {
        }
        OwnedGfxTexture(const OwnedGfxTexture&) = delete;
        OwnedGfxTexture& operator=(const OwnedGfxTexture&) = delete;
        ~OwnedGfxTexture()
        {
            // m_view's destructor releases the texture view; destroy the texture itself here.
            if (m_texture) {
                gfxTextureDestroy(m_texture);
            }
        }

    private:
        ImageBufferView m_view;
        GfxTexture m_texture{};
    };
} // namespace

ImageBuffer::ImageBuffer(GfxDevice device, GfxQueue queue, CreateInfo&& createInfo)
    : m_device{ device }
    , m_queue{ queue }
    , m_extent{ createInfo.extent }
    , m_format{ createInfo.format }
    , m_texture{ createInfo.texture }
    , m_view{ std::move(createInfo.view) }
    , m_type{ createInfo.type }
    , m_viewType{ createInfo.viewType }
    , m_samplesCount{ createInfo.sampleCount }
    , m_mipLevels{ createInfo.mipLevels }
    , m_layerCount{ createInfo.layerCount }
    , m_usageFlags{ createInfo.usageFlags }
    , m_layout{ createInfo.layout }
    , m_deferredResourceDestroyer{ createInfo.deferredResourceDestroyer }
    , m_destroyExecutionMode{ createInfo.destroyExecutionMode }
    , m_state{ createInfo.stateFlag ? createInfo.stateFlag : std::make_shared<std::atomic<prev::core::ResourceState>>(prev::core::ResourceState::Ready) }
{
    assert(m_deferredResourceDestroyer && "ImageBuffer requires a deferred resource destroyer");
}

bool ImageBuffer::IsDeferred() const
{
    if (m_destroyExecutionMode == ExecutionMode::Immediate) {
        return false;
    }
    return m_deferredResourceDestroyer->IsActive();
}

prev::core::ResourceState ImageBuffer::GetState() const
{
    return m_state->load();
}

ImageBuffer::~ImageBuffer()
{
    // Cancel a still-pending async upload: the uploader checks this at flush and skips a destroyed
    // resource instead of recording work for it / touching it.
    if (m_state && m_state->load() == prev::core::ResourceState::Creating) {
        m_state->store(prev::core::ResourceState::Destroying);
    }

    // Move the GPU handles into an owner that destroys them. When a deferred manager is active, hand
    // it over so destruction happens after any in-flight work; otherwise wait for the queue to drain
    // and let the owner destroy them here as it leaves scope.
    auto handles{ std::make_unique<OwnedGfxTexture>(std::move(m_view), m_texture) };
    m_texture = nullptr;

    if (IsDeferred()) {
        m_deferredResourceDestroyer->Destroy(std::move(handles));
    } else {
        gfxQueueWaitIdle(m_queue);
    }
}

void ImageBuffer::UpdateLayout(const GfxTextureLayout newLayout, GfxCommandEncoder commandEncoder)
{
    if (m_layout == newLayout) {
        return;
    }

    GfxPipelineStageFlags srcStage{}, dstStage{};
    GfxAccessFlags srcAccess{}, dstAccess{};
    GetLayoutBarrierParams(m_layout, srcStage, srcAccess);
    GetLayoutBarrierParams(newLayout, dstStage, dstAccess);

    GfxTextureBarrier barrier{};
    barrier.texture = m_texture;
    barrier.oldLayout = m_layout;
    barrier.newLayout = newLayout;
    barrier.srcStageMask = srcStage;
    barrier.dstStageMask = dstStage;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.baseMipLevel = 0;
    barrier.mipLevelCount = m_mipLevels;
    barrier.baseArrayLayer = 0;
    barrier.arrayLayerCount = m_layerCount;

    GfxPipelineBarrierDescriptor barrierDesc{};
    barrierDesc.sType = GFX_STRUCTURE_TYPE_PIPELINE_BARRIER_DESCRIPTOR;
    barrierDesc.textureBarriers = &barrier;
    barrierDesc.textureBarrierCount = 1;
    gfxCommandEncoderPipelineBarrier(commandEncoder, &barrierDesc);

    m_layout = newLayout;
}

void ImageBuffer::GenerateMipMaps(GfxCommandEncoder commandEncoder)
{
    // gfx generateMipmaps handles all layout transitions internally:
    // - transitions mip 0 from current layout to TRANSFER_SRC
    // - transitions each subsequent mip from UNDEFINED to TRANSFER_DST, blits, then to TRANSFER_SRC
    // - transitions all mips back to the initial layout
    gfxCommandEncoderGenerateMipmaps(commandEncoder, m_texture);
}

void ImageBuffer::Copy(ImageBuffer& dstImage, GfxCommandEncoder commandEncoder)
{
    const auto srcFinalLayout{ m_layout == GFX_TEXTURE_LAYOUT_UNDEFINED ? GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY : m_layout };

    GfxBlitTextureToTextureDescriptor blitDesc{};
    blitDesc.source = m_texture;
    blitDesc.sourceOrigin = { 0, 0, 0 };
    blitDesc.sourceExtent = m_extent;
    blitDesc.sourceMipLevel = 0;
    blitDesc.sourceFinalLayout = srcFinalLayout;
    blitDesc.destination = dstImage.GetTexture();
    blitDesc.destinationOrigin = { 0, 0, 0 };
    blitDesc.destinationExtent = dstImage.GetExtent();
    blitDesc.destinationMipLevel = 0;
    blitDesc.destinationFinalLayout = dstImage.GetLayout();
    blitDesc.filter = GFX_FILTER_MODE_LINEAR;
    gfxCommandEncoderBlitTextureToTexture(commandEncoder, &blitDesc);

    m_layout = srcFinalLayout;
}

GfxExtent3D ImageBuffer::GetExtent() const
{
    return m_extent;
}

GfxFormat ImageBuffer::GetFormat() const
{
    return m_format;
}

GfxTexture ImageBuffer::GetTexture() const
{
    return m_texture;
}

GfxTextureType ImageBuffer::GetTextureType() const
{
    return m_type;
}

const ImageBufferView& ImageBuffer::GetTextureView() const
{
    return m_view;
}

GfxTextureViewType ImageBuffer::GetTextureViewType() const
{
    return m_viewType;
}

GfxSampleCount ImageBuffer::GetSampleCount() const
{
    return m_samplesCount;
}

uint32_t ImageBuffer::GetMipLevels() const
{
    return m_mipLevels;
}

uint32_t ImageBuffer::GetLayerCount() const
{
    return m_layerCount;
}

GfxTextureUsageFlags ImageBuffer::GetUsageFlags() const
{
    return m_usageFlags;
}

GfxTextureLayout ImageBuffer::GetLayout() const
{
    return m_layout;
}

void* ImageBuffer::GetMappedData() const
{
    return m_mappedData;
}

ImageBuffer::operator GfxTexture() const
{
    return m_texture;
}

} // namespace prev::render::buffer
