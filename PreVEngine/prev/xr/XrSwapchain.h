#ifndef __XR_SWAPCHAIN_H__
#define __XR_SWAPCHAIN_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include "../render/buffer/ImageBuffer.h"
#include "../render/buffer/ImageBufferView.h"
#include "../render/framebuffer/Framebuffer.h"
#include "../render/pass/RenderPass.h"
#include "../render/swapchain/ISwapchain.h"
#include "../render/swapchain/SwapchainTargets.h"

#include "../core/device/Device.h"
#include "../core/sync/Fence.h"
#include "../util/Utils.h"

#include <memory>

namespace prev::xr {
class XrSwapchain final : public prev::render::swapchain::ISwapchain {
public:
    XrSwapchain(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, xr::IXr& xr, GfxSampleCount sampleCount = GFX_SAMPLE_COUNT_1, uint32_t maxFramesInFlight = 0);

    ~XrSwapchain();

public:
    bool BeginFrame(prev::render::swapchain::FrameContext& outContext) override;

    void BeginPass(prev::render::swapchain::FrameContext& outContext, uint32_t passIndex) override;

    void EndPass(uint32_t passIndex) override;

    void EndFrame(const prev::render::FrameSubmitSync& submitSync) override;

    GfxExtent2D GetExtent() const override;

    uint32_t GetImageCount() const override;

    uint32_t GetPassCount() const override;

    void Print() const override;

private:
    struct RenderTarget {
        std::unique_ptr<prev::render::buffer::ImageBufferView> colorView;
        std::unique_ptr<prev::render::buffer::ImageBufferView> depthView;
        std::unique_ptr<prev::render::framebuffer::Framebuffer> framebuffer;
    };

    // One per XR image slot: targets (built lazily, rebuilt on imagesChanged) + encoder + pacing fence.
    struct SwapchainBuffer {
        GfxTexture colorTexture{};
        GfxTexture depthTexture{};
        RenderTarget multiviewTarget;
        std::vector<RenderTarget> perEyeTargets;
        GfxCommandEncoder commandEncoder{};
        std::unique_ptr<prev::core::sync::Fence> fence;
    };

private:
    void CreateResources(const GfxExtent2D& extent);

    void DestroyResources();

    void RecreateResources(const GfxExtent2D& extent);

    void BuildSlotTargets(SwapchainBuffer& sb, GfxTexture colorTexture, GfxTexture depthTexture);

    bool HasBuiltTargets(const SwapchainBuffer& sb) const;

    void DestroySlotTargets(SwapchainBuffer& sb);

    RenderTarget BuildLayerTarget(GfxTexture colorTexture, GfxTexture depthTexture, uint32_t layer);

    void CreateBufferCommands(SwapchainBuffer& sb);

    void DestroyBufferCommands(SwapchainBuffer& sb);

    bool IsMultiview() const;

private:
    prev::core::device::Device& m_device;

    prev::render::pass::RenderPass& m_renderPass;

    xr::IXr& m_xr;

    GfxSampleCount m_sampleCount{ GFX_SAMPLE_COUNT_1 };

    const prev::core::device::Queue& m_graphicsQueue;

    GfxExtent2D m_extent{};

    std::unique_ptr<prev::render::swapchain::SwapchainTargets> m_targets;

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex{};

    bool m_imagesChanged{}; // this frame's images were re-imported (WebXR) -> no fence pacing

    bool m_passActive{};

    // Rotating frame-in-flight slot for per-frame-image backends (image slot stays 0, but per-frame
    // pools need a rotating index); from Config::maxFramesInFlight, 0 -> 2.
    uint32_t m_framesInFlight{ 2 };

    uint32_t m_frameInFlightIndex{ 0 };
};
} // namespace prev::xr

#endif

#endif
