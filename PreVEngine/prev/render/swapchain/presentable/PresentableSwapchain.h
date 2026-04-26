#ifndef __PRESENTABLE_SWAPCHAIN_H__
#define __PRESENTABLE_SWAPCHAIN_H__

#include "../ISwapchain.h"

#include "../../pass/RenderPass.h"

#include "../../../core/device/Device.h"
#include "../../../util/Utils.h"

namespace prev::render::swapchain::presentable {
class PresentableSwapchain final : public ISwapchain {
public:
    PresentableSwapchain(core::device::Device& device, pass::RenderPass& renderPass, GfxSurface surface, GfxExtent2D extent, GfxPresentMode presentMode, uint32_t imageCount, uint32_t viewCount = 1, uint32_t maxFramesInFlight = 0);

    ~PresentableSwapchain();

public:
    bool BeginFrame(FrameContext& outContext) override;

    void EndFrame() override;

    void Print() const override;

    GfxExtent2D GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        GfxTextureView view{};
        GfxFramebuffer framebuffer{};
        GfxSemaphore renderSemaphore{};
    };

    struct FrameInFlight {
        GfxCommandEncoder commandEncoder{};
        GfxSemaphore acquireSemaphore{};
        GfxFence fence{};
    };

private:
    void CreateResources();

    void DestroyResources();

private:
    core::device::Device& m_device;

    pass::RenderPass& m_renderPass;

    const core::device::Queue& m_graphicsQueue;

    const core::device::Queue& m_presentQueue;

    uint32_t m_viewCount{ 1 };

    uint32_t m_maxFramesInFlight{ 0 };

    GfxSwapchain m_swapchain{};

    GfxExtent2D m_extent{};

    GfxTexture m_depthTexture{};

    GfxTextureView m_depthView{};

    GfxTexture m_msaaColorTexture{};

    GfxTextureView m_msaaColorView{};

    GfxTexture m_msaaDepthTexture{};

    GfxTextureView m_msaaDepthView{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    std::vector<FrameInFlight> m_framesInFlight;

    uint32_t m_acquiredIndex{};

    bool m_isAcquired{ false };

    util::CircularIndex<uint32_t> m_frameIndex{ 3 };
};
} // namespace prev::render::swapchain::presentable

#endif
