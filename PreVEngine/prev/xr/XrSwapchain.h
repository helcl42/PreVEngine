#ifndef __XR_SWAPCHAIN_H__
#define __XR_SWAPCHAIN_H__

#ifdef ENABLE_XR

#include "IXr.h"

#include "../render/buffer/ImageBuffer.h"
#include "../render/buffer/ImageBufferView.h"
#include "../render/framebuffer/Framebuffer.h"
#include "../render/pass/RenderPass.h"
#include "../render/swapchain/ISwapchain.h"

#include "../core/device/Device.h"
#include "../core/sync/Fence.h"
#include "../util/Utils.h"

#include <memory>

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
        std::unique_ptr<prev::render::buffer::ImageBufferView> colorView;
        GfxTexture depthTexture{};
        std::unique_ptr<prev::render::buffer::ImageBufferView> depthView;
        std::unique_ptr<prev::render::framebuffer::Framebuffer> framebuffer;
        GfxCommandEncoder commandEncoder{};
        std::unique_ptr<prev::core::sync::Fence> fence;
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
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaColorBuffer;
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaDepthBuffer;

    // Owned depth buffer (when XR doesn't provide depth images)
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_ownedDepthBuffer;

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex{};
};
} // namespace prev::xr

#endif

#endif