#ifndef __XR_SWAPCHAIN_H__
#define __XR_SWAPCHAIN_H__

#ifdef ENABLE_XR

#include "OpenXr.h"

#include "../render/ISwapchain.h"

#include "../render/buffer/ImageBuffer.h"
#include "../render/pass/RenderPass.h"

#include "../core/device/Device.h"
#include "../core/memory/Allocator.h"
#include "../util/Utils.h"

namespace prev::xr {
class XrSwapchain final : public prev::render::ISwapchain {
public:
    XrSwapchain(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, xr::OpenXr& xr, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

    ~XrSwapchain();

public:
    std::vector<VkPresentModeKHR> GetPresentModes() const override;

    bool SetPresentMode(bool noTearing, bool powerSave) override;

    bool SetPresentMode(VkPresentModeKHR preferredMode) override;

    bool SetImageCount(uint32_t imageCount) override;

    bool UpdateExtent(uint32_t width, uint32_t height) override;

    bool BeginFrame(prev::render::SwapChainFrameContext& outContext) override;

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
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    xr::OpenXr& m_openXr;

    VkSurfaceKHR m_surface{};

    VkSampleCountFlagBits m_sampleCount{ VK_SAMPLE_COUNT_1_BIT };

    const prev::core::device::Queue& m_graphicsQueue;

    const prev::core::device::Queue& m_presentQueue;

    VkCommandPool m_commandPool{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    VkExtent2D m_extent{};

    uint32_t m_acquiredIndex{};

    // MSAA
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaColorBuffer{};

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaDepthBuffer{};

    // Depth buffer (created when OpenXR does not provide it)
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};
};
} // namespace prev::xr

#endif

#endif