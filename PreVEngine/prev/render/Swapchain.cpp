#include "Swapchain.h"

#include "buffer/ImageBufferBuilder.h"

#include "../util/MathUtils.h"
#include "../util/VkUtils.h"

#include <algorithm>

namespace prev::render {
Swapchain::Swapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount, uint32_t viewCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_surface{ surface }
    , m_sampleCount{ sampleCount }
    , m_viewCount{ viewCount }
    , m_graphicsQueue{ m_device.GetQueue(core::device::QueueType::GRAPHICS) }
    , m_presentQueue{ m_device.GetQueue(core::device::QueueType::PRESENT) }
    , m_swapchain{ VK_NULL_HANDLE }
    , m_acquiredIndex{ 0 }
    , m_isAcquired{ false }
{
    const VkSurfaceCapabilitiesKHR surfaceCapabilities{ GetSurfaceCapabilities() };
    assert(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    assert(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    assert(surfaceCapabilities.supportedCompositeAlpha & (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR | VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR));

    m_swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    m_swapchainCreateInfo.surface = m_surface;
    m_swapchainCreateInfo.imageFormat = m_renderPass.GetColorFormat();
    m_swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    m_swapchainCreateInfo.imageArrayLayers = m_viewCount;
    m_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    m_swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    m_swapchainCreateInfo.clipped = VK_TRUE;
    m_swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    m_swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    UpdateExtent(512, 512);
    SetImageCount(3);

    m_commandPool = prev::util::vk::CreateCommandPool(m_device, m_graphicsQueue.family);

    Apply();
}

Swapchain::~Swapchain()
{
    m_device.WaitIdle();

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_device, m_swapchain, 0);
        for (auto& swapchainBuffer : m_swapchainBuffers) {
            swapchainBuffer.Destroy(m_device);
        }

        LOGI("Swapchain destroyed");
    }

    m_msaaColorBuffer = nullptr;
    m_msaaDepthBuffer = nullptr;
    m_depthBuffer = nullptr;
}

bool Swapchain::UpdateExtent(uint32_t width, uint32_t height)
{
    const VkSurfaceCapabilitiesKHR surfaceCapabilities{ GetSurfaceCapabilities() };
    const VkExtent2D& currentSurfaceExtent{ surfaceCapabilities.currentExtent };

    if (currentSurfaceExtent.width == 0 || currentSurfaceExtent.height == 0) {
        return false;
    }

    // because of android notifies about SUBOPTIMAL presence -> it internaly transforms image(rotates) because we use preTransform == VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    if (currentSurfaceExtent.width == m_swapchainCreateInfo.imageExtent.width && currentSurfaceExtent.height == m_swapchainCreateInfo.imageExtent.height) {
        return false;
    }

    if (currentSurfaceExtent.width == std::numeric_limits<uint32_t>::max() || currentSurfaceExtent.height == std::numeric_limits<uint32_t>::min()) {
        LOGW("Can't determine current window surface extent from surface caps. Using provided extent instead. (%d x %d)", width, height);
        m_swapchainCreateInfo.imageExtent.width = util::math::Clamp(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        m_swapchainCreateInfo.imageExtent.height = util::math::Clamp(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    } else {
        m_swapchainCreateInfo.imageExtent = currentSurfaceExtent;
    }

    if (!!m_swapchain) {
        Apply();
    }
    return true;
}

bool Swapchain::SetImageCount(uint32_t imageCount)
{
    const VkSurfaceCapabilitiesKHR surfaceCapabilities{ GetSurfaceCapabilities() };

    uint32_t count{ std::max(imageCount, surfaceCapabilities.minImageCount) };
    if (surfaceCapabilities.maxImageCount > 0) {
        count = std::min(count, surfaceCapabilities.maxImageCount);
    }

    m_swapchainCreateInfo.minImageCount = count;

    if (count != imageCount) {
        LOGW("Swapchain using %d framebuffers, instead of %d.", count, imageCount);
    }

    if (!!m_swapchain) {
        Apply();
    }

    return count == imageCount;
}

std::vector<VkPresentModeKHR> Swapchain::GetPresentModes() const
{
    uint32_t count{};
    VKERRCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.GetGPU(), m_surface, &count, nullptr));
    std::vector<VkPresentModeKHR> modes(count);
    VKERRCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.GetGPU(), m_surface, &count, modes.data()));
    return modes;
}

// ---------------------------- Present Mode ----------------------------
// noTearing : TRUE = Wait for next vsync, to swap buffers.  FALSE = faster fps.
// powersave  : TRUE = Limit framerate to vsync (60 fps).    FALSE = lower latency.
bool Swapchain::SetPresentMode(bool noTearing, bool powerSave)
{
    const VkPresentModeKHR mode{ static_cast<VkPresentModeKHR>((noTearing ? VK_PRESENT_MODE_MAILBOX_KHR : 0) ^ (powerSave ? VK_PRESENT_MODE_FIFO_RELAXED_KHR : 0)) };
    return SetPresentMode(mode); // if not found, use FIFO
}

bool Swapchain::SetPresentMode(VkPresentModeKHR preferredMode)
{
    const auto modes{ GetPresentModes() };

    VkPresentModeKHR& mode{ m_swapchainCreateInfo.presentMode };
    mode = VK_PRESENT_MODE_FIFO_KHR; // default to FIFO mode
    for (const auto& m : modes) {
        if (m == preferredMode) {
            mode = preferredMode; // if prefered mode is available, select it.
            break;
        }
    }

    if (mode != preferredMode) {
        LOGW("Requested present-mode is not supported. Reverting to FIFO mode.");
    }

    if (!!m_swapchain) {
        Apply();
    }

    return mode == preferredMode;
}

void Swapchain::Print() const
{
    LOGI("Swapchain:");

    LOGI("\tColor   = %3d : %s", m_swapchainCreateInfo.imageFormat, util::vk::FormatToString(m_swapchainCreateInfo.imageFormat).c_str());
    LOGI("\tDepth   = %3d : %s", m_depthBuffer->GetFormat(), util::vk::FormatToString(m_depthBuffer->GetFormat()).c_str());

    const auto& extent{ m_swapchainCreateInfo.imageExtent };
    LOGI("\tExtent  = %d x %d", extent.width, extent.height);
    LOGI("\tBuffers = %d", static_cast<int>(m_swapchainBuffers.size()));

    const auto modes{ GetPresentModes() };
    LOGI("\tPresentMode:");
    const auto& mode{ m_swapchainCreateInfo.presentMode };
    for (auto m : modes) {
        LOGI("\t\t%s %s", (m == mode) ? TICK_CHARACTER : " ", util::vk::PresentModeToString(m).c_str());
    }
    LOGI("\tSharingMode: %s", m_swapchainCreateInfo.imageSharingMode == VK_SHARING_MODE_EXCLUSIVE ? "Exclusive" : "Shared");
}

const VkExtent2D& Swapchain::GetExtent() const
{
    return m_swapchainCreateInfo.imageExtent;
}

uint32_t Swapchain::GetImageCount() const
{
    return static_cast<uint32_t>(m_swapchainBuffers.size());
}

void Swapchain::Apply()
{
    m_swapchainCreateInfo.oldSwapchain = m_swapchain;

    const std::vector<uint32_t> families = { m_presentQueue.family, m_graphicsQueue.family };
    if (m_presentQueue.family != m_graphicsQueue.family) {
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        m_swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
        m_swapchainCreateInfo.pQueueFamilyIndices = families.data();
    } else {
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    VKERRCHECK(vkCreateSwapchainKHR(m_device, &m_swapchainCreateInfo, nullptr, &m_swapchain));

    if (m_swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE) {
        m_device.WaitIdle();
        vkDestroySwapchainKHR(m_device, m_swapchainCreateInfo.oldSwapchain, VK_NULL_HANDLE);
        for (auto& swapchainBuffer : m_swapchainBuffers) {
            swapchainBuffer.Destroy(m_device);
        }
    }

    const VkExtent3D newExtent{ m_swapchainCreateInfo.imageExtent.width, m_swapchainCreateInfo.imageExtent.height, 1 };
    const VkImageViewType imageViewType{ m_viewCount > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D };

    m_depthBuffer = buffer::ImageBufferBuilder{ m_allocator }
                        .SetExtent(newExtent)
                        .SetFormat(m_renderPass.GetDepthFormat())
                        .SetType(VK_IMAGE_TYPE_2D)
                        .SetViewType(imageViewType)
                        .SetLayerCount(m_viewCount)
                        .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                        .Build();
    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer = buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(newExtent)
                                .SetFormat(m_renderPass.GetColorFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(imageViewType)
                                .SetLayerCount(m_viewCount)
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                                .Build();
        m_msaaDepthBuffer = buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(newExtent)
                                .SetFormat(m_renderPass.GetDepthFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(imageViewType)
                                .SetLayerCount(m_viewCount)
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                                .Build();
    }

    const auto swapchainImages{ GetSwapchainImages() };
    const auto swapchainImagesCount{ static_cast<uint32_t>(swapchainImages.size()) };

    m_frameIndex = util::CircularIndex{ swapchainImagesCount };

    m_swapchainBuffers.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        auto image{ swapchainImages[i] };
        auto imageView{ util::vk::CreateImageView(m_device, image, m_swapchainCreateInfo.imageFormat, imageViewType, 1, VK_IMAGE_ASPECT_COLOR_BIT, m_viewCount) };

        std::vector<VkImageView> swapchainImageViews;
        if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            swapchainImageViews.push_back(m_msaaColorBuffer->GetImageView());
            swapchainImageViews.push_back(m_msaaDepthBuffer->GetImageView());
        }
        swapchainImageViews.push_back(imageView); // Add color buffer (unique)
        swapchainImageViews.push_back(m_depthBuffer->GetImageView()); // Add depth buffer (shared)

        auto& swapchainBuffer{ m_swapchainBuffers[i] };
        swapchainBuffer.image = image;
        swapchainBuffer.view = imageView;
        swapchainBuffer.framebuffer = util::vk::CreateFrameBuffer(m_device, m_renderPass, swapchainImageViews, m_swapchainCreateInfo.imageExtent);
        swapchainBuffer.commandBuffer = util::vk::CreateCommandBuffer(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        swapchainBuffer.fence = util::vk::CreateFence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
        swapchainBuffer.extent = m_swapchainCreateInfo.imageExtent;
        swapchainBuffer.renderSemaphore = util::vk::CreateSemaphore(m_device);
        swapchainBuffer.presentSemaphore = util::vk::CreateSemaphore(m_device);
    }

    if (m_swapchainCreateInfo.oldSwapchain == VK_NULL_HANDLE) {
        LOGI("Swapchain created");
    }
}

VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCapabilities() const
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VKERRCHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.GetGPU(), m_surface, &surfaceCapabilities));
    return surfaceCapabilities;
}

std::vector<VkImage> Swapchain::GetSwapchainImages() const
{
    std::vector<VkImage> swapchainImages;

    uint32_t swapchainImagesCount{ 0 };
    VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, nullptr));
    swapchainImages.resize(swapchainImagesCount);
    VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, swapchainImages.data()));

    return swapchainImages;
}

bool Swapchain::AcquireNext(SwapchainBuffer& next)
{
    ASSERT(!m_isAcquired, "Swapchain: Previous swapchain buffer has not yet been presented.");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };

    VKERRCHECK(vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX));
    VKERRCHECK(vkResetFences(m_device, 1, &swapchainBuffer.fence));

    uint32_t acquireIndex;
    const auto result{ vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, swapchainBuffer.presentSemaphore, VK_NULL_HANDLE, &acquireIndex) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        UpdateExtent(m_swapchainCreateInfo.imageExtent.width, m_swapchainCreateInfo.imageExtent.height);
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        ASSERT(true, "failed to acquire swap chain image!");
    }

    m_acquiredIndex = acquireIndex;
    m_isAcquired = true;
    next = swapchainBuffer;

    return true;
}

void Swapchain::Submit()
{
    ASSERT(!!m_isAcquired, "Swapchain: A buffer must be acquired before submitting.");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &swapchainBuffer.presentSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &swapchainBuffer.renderSemaphore;

    VKERRCHECK(m_graphicsQueue.Submit(1, &submitInfo, swapchainBuffer.fence));
}

void Swapchain::Present()
{
    ASSERT(!!m_isAcquired, "Swapchain: A buffer must be acquired before presenting.");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &swapchainBuffer.renderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_acquiredIndex;

    bool swapchainChanged{ false };

    const auto result{ m_presentQueue.Present(&presentInfo) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapchainChanged = UpdateExtent(m_swapchainCreateInfo.imageExtent.width, m_swapchainCreateInfo.imageExtent.height);
    } else if (result != VK_SUCCESS) {
        ShowVkResult(result);
    }

    if (!swapchainChanged) {
        ++m_frameIndex;
    }

    m_isAcquired = false;
}

bool Swapchain::BeginFrame(SwapChainFrameContext& outContext)
{
    SwapchainBuffer swapchainBuffer;
    if (!AcquireNext(swapchainBuffer)) {
        return false;
    }

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

    outContext = { swapchainBuffer.framebuffer, swapchainBuffer.commandBuffer, m_frameIndex };
    return true;
}

void Swapchain::EndFrame()
{
    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };
    VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

    Submit();
    Present();
}
} // namespace prev::render
