#ifndef __XR_SWAPCHAIN_H__
#define __XR_SWAPCHAIN_H__

#include "OpenXR.h"

#include "../render/ISwapchain.h"

#include "../render/buffer/ImageBuffer.h"
#include "../render/pass/RenderPass.h"

#include "../core/memory/Allocator.h"
#include "../util/Utils.h"

namespace prev::xr {
class XRSwapchain final : public render::ISwapchain {
public:
    XRSwapchain(core::device::Device& device, core::memory::Allocator& allocator, render::pass::RenderPass& renderPass, xr::OpenXR& xr, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

    ~XRSwapchain();

public:
    std::vector<VkPresentModeKHR> GetPresentModes() const override;

    bool SetPresentMode(bool noTearing, bool powerSave) override;

    bool SetPresentMode(VkPresentModeKHR preferredMode) override;

    bool SetImageCount(uint32_t imageCount) override;

    bool UpdateExtent() override;

    bool BeginFrame(render::SwapChainFrameContext& outContext) override;

    void EndFrame() override;

    void Print() const override;

    const VkExtent2D& GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        VkImage colorImage{};

        VkImageView colorImageView{};

        VkImage depthImage{};

        VkImageView depthImageView{};

        VkExtent2D extent{};

        VkFramebuffer framebuffer{};

        VkCommandBuffer commandBuffer{};

        VkFence fence{};

        void Destroy(VkDevice device)
        {
            vkDestroyFence(device, fence, nullptr);
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    };

private:
    core::device::Device& m_device;

    core::memory::Allocator& m_allocator;

    render::pass::RenderPass& m_renderPass;

    xr::OpenXR& m_openXr;

    VkSurfaceKHR m_surface{};

    VkSampleCountFlagBits m_sampleCount{ VK_SAMPLE_COUNT_1_BIT };

    std::shared_ptr<prev::core::device::Queue> m_presentQueue{};

    std::shared_ptr<prev::core::device::Queue> m_graphicsQueue{};

    VkCommandPool m_commandPool{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    VkExtent2D m_extent{};

    uint32_t m_acquiredIndex{};

    // MSAA
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaColorBuffer{};

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaDepthBuffer{};
};
} // namespace prev::xr

#endif