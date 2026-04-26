#ifndef __HEADLESS_SWAPCHAIN_H__
#define __HEADLESS_SWAPCHAIN_H__

#include "../ISwapchain.h"

#include "../../pass/RenderPass.h"

#include "../../../core/device/Device.h"
#include "../../../util/Utils.h"

namespace prev::render::swapchain::headless {
class HeadlessSwapchain final : public ISwapchain {
public:
    HeadlessSwapchain(core::device::Device& device, pass::RenderPass& renderPass, GfxExtent2D extent = { 512, 512 }, uint32_t imageCount = 3, uint32_t viewCount = 1);

    ~HeadlessSwapchain();

public:
    bool BeginFrame(FrameContext& outContext) override;

    void EndFrame() override;

    void Print() const override;

    GfxExtent2D GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        GfxTexture colorTexture{};
        GfxTextureView colorView{};
        GfxFramebuffer framebuffer{};
        GfxCommandEncoder commandEncoder{};
        GfxFence fence{};
    };

private:
    void CreateResources();

    void DestroyResources();

private:
    core::device::Device& m_device;

    pass::RenderPass& m_renderPass;

    const core::device::Queue& m_graphicsQueue;

    GfxExtent2D m_extent{};

    uint32_t m_viewCount{ 1 };

    GfxTexture m_depthTexture{};

    GfxTextureView m_depthView{};

    GfxTexture m_msaaColorTexture{};

    GfxTextureView m_msaaColorView{};

    GfxTexture m_msaaDepthTexture{};

    GfxTextureView m_msaaDepthView{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    bool m_isAcquired{ false };

    util::CircularIndex<uint32_t> m_frameIndex{ 3 };
};
} // namespace prev::render::swapchain::headless

#endif
