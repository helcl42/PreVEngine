#ifndef __XR_SWAPCHAIN_H__
#define __XR_SWAPCHAIN_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include "../render/pass/RenderPass.h"
#include "../render/swapchain/ISwapchain.h"

#include "../core/device/Device.h"
#include "../util/Utils.h"

namespace prev::xr {
class XrSwapchain final : public prev::render::swapchain::ISwapchain {
public:
    XrSwapchain(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, xr::IXr& xr, GfxSampleCount sampleCount = GFX_SAMPLE_COUNT_1);

    ~XrSwapchain();

public:
    bool BeginFrame(prev::render::swapchain::FrameContext& outContext) override;

    void EndFrame() override;

    void Print() const override;

    GfxExtent2D GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        GfxTexture colorTexture{};
        GfxTextureView colorView{};
        GfxTexture depthTexture{};
        GfxTextureView depthView{};
        GfxFramebuffer framebuffer{};
        GfxCommandEncoder commandEncoder{};
        GfxFence fence{};
    };

private:
    void CreateResources();

    void DestroyResources();

private:
    prev::core::device::Device& m_device;

    prev::render::pass::RenderPass& m_renderPass;

    xr::IXr& m_xr;

    GfxSampleCount m_sampleCount{ GFX_SAMPLE_COUNT_1 };

    const prev::core::device::Queue& m_graphicsQueue;

    GfxExtent2D m_extent{};

    // MSAA resources (shared across all frames)
    GfxTexture m_msaaColorTexture{};
    GfxTextureView m_msaaColorView{};
    GfxTexture m_msaaDepthTexture{};
    GfxTextureView m_msaaDepthView{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex{};
};
} // namespace prev::xr

#endif

#endif