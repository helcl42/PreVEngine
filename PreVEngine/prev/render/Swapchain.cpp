#include "Swapchain.h"

#include "buffer/image/DepthImageBuffer.h"
#include "buffer/image/ImageBufferFactory.h"

#include "../core/AllocatorProvider.h"
#include "../core/DeviceProvider.h"
#include "../util/MathUtils.h"
#include "../util/VkUtils.h"

#include <algorithm>

namespace prev::render {
Swapchain::Swapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_surface{ surface }
    , m_sampleCount{ sampleCount }
{
    m_graphicsQueue = m_device.GetQueue(core::device::QueueType::GRAPHICS);
    m_presentQueue = m_device.GetQueue(core::device::QueueType::PRESENT);

    m_swapchain = VK_NULL_HANDLE;
    m_isAcquired = false;

    const VkSurfaceCapabilitiesKHR surfaceCapabilities{ GetSurfaceCapabilities() };
    assert(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    assert(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    assert(surfaceCapabilities.supportedCompositeAlpha & (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR | VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR));

    m_swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    m_swapchainCreateInfo.surface = m_surface;
    m_swapchainCreateInfo.imageFormat = m_renderPass.GetColorFormat();
    m_swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    m_swapchainCreateInfo.imageArrayLayers = 1; // 2 for stereo
    m_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    m_swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    m_swapchainCreateInfo.clipped = VK_TRUE;
    m_swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    m_swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    UpdateExtent();
    SetImageCount(3);

    m_commandPool = prev::util::vk::CreateCommandPool(m_device, m_graphicsQueue->family);

    buffer::image::ImageBufferFactory imageBufferFactory{};
    m_depthBuffer = imageBufferFactory.CreateDepth(buffer::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, renderPass.GetDepthFormat(), VK_SAMPLE_COUNT_1_BIT, 0, false, VK_IMAGE_VIEW_TYPE_2D }, m_allocator);
    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer = imageBufferFactory.CreateColor(buffer::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, m_renderPass.GetColorFormat(), m_sampleCount, 0, false, VK_IMAGE_VIEW_TYPE_2D }, m_allocator);
        m_msaaDepthBuffer = imageBufferFactory.CreateDepth(buffer::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, m_renderPass.GetDepthFormat(), m_sampleCount, 0, false, VK_IMAGE_VIEW_TYPE_2D }, m_allocator);
    }

    m_acquireSemaphore = prev::util::vk::CreateSemaphore(m_device);
    m_renderSemaphore = prev::util::vk::CreateSemaphore(m_device);

    Apply();
}

Swapchain::~Swapchain()
{
    vkDeviceWaitIdle(m_device);

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    if (m_renderSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_renderSemaphore, nullptr);
    }

    if (m_acquireSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_acquireSemaphore, nullptr);
    }

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_device, m_swapchain, 0);
        for (auto& swapchainBuffer : m_swapchainBuffers) {
            swapchainBuffer.Destroy(m_device);
        }

        LOGI("Swapchain destroyed\n");
    }

    m_msaaColorBuffer = nullptr;
    m_msaaDepthBuffer = nullptr;
    m_depthBuffer = nullptr;
}

bool Swapchain::UpdateExtent()
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

    if (currentSurfaceExtent.width == 0xFFFFFFFF) { // 0xFFFFFFFF indicates surface size is set from extent
        const uint32_t defaultWidth = 256;
        const uint32_t defaultHeight = 256;

        LOGW("Can't determine current window surface extent from surface caps. Using defaults instead. (%d x %d)\n", defaultWidth, defaultHeight);

        m_swapchainCreateInfo.imageExtent.width = util::math::Clamp(defaultWidth, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        m_swapchainCreateInfo.imageExtent.height = util::math::Clamp(defaultHeight, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
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
        LOGW("Swapchain using %d framebuffers, instead of %d.\n", count, imageCount);
    }

    if (!!m_swapchain) {
        Apply();
    }

    return count == imageCount;
}

std::vector<VkPresentModeKHR> Swapchain::GetPresentModes() const
{
    uint32_t count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*m_device.GetGPU(), m_surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes(count);
    VKERRCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(*m_device.GetGPU(), m_surface, &count, modes.data()));
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
        LOGW("Requested present-mode is not supported. Reverting to FIFO mode.\n");
    }

    if (!!m_swapchain) {
        Apply();
    }

    return mode == preferredMode;
}

void Swapchain::Print() const
{
    LOGI("Swapchain:\n");

    LOGI("\tFormat  = %3d : %s\n", m_swapchainCreateInfo.imageFormat, util::vk::FormatToString(m_swapchainCreateInfo.imageFormat).c_str());
    LOGI("\tDepth   = %3d : %s\n", m_depthBuffer->GetFormat(), util::vk::FormatToString(m_depthBuffer->GetFormat()).c_str());

    const auto& extent{ m_swapchainCreateInfo.imageExtent };
    LOGI("\tExtent  = %d x %d\n", extent.width, extent.height);
    LOGI("\tBuffers = %d\n", (int)m_swapchainBuffers.size());

    const auto modes{ GetPresentModes() };
    LOGI("\tPresentMode:\n");
    const auto& mode{ m_swapchainCreateInfo.presentMode };
    for (auto m : modes) {
        print((m == mode) ? ConsoleColor::RESET : ConsoleColor::FAINT, "\t\t%s %s\n", (m == mode) ? TICK_CHARACTER : " ", util::vk::PresentModeToString(m).c_str());
    }
    LOGI("\tSharingMode: %s\n", m_swapchainCreateInfo.imageSharingMode == VK_SHARING_MODE_EXCLUSIVE ? "Exclusive" : "Shared");
}

const VkExtent2D& Swapchain::GetExtent() const
{
    return m_swapchainCreateInfo.imageExtent;
}

uint32_t Swapchain::GetImageCount() const
{
    return m_swapchainImagesCount;
}

void Swapchain::Apply()
{
    m_swapchainCreateInfo.oldSwapchain = m_swapchain;

    const std::vector<uint32_t> families = { m_presentQueue->family, m_graphicsQueue->family };
    if (m_presentQueue->family != m_graphicsQueue->family) {
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        m_swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
        m_swapchainCreateInfo.pQueueFamilyIndices = families.data();
    } else {
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    VKERRCHECK(vkCreateSwapchainKHR(m_device, &m_swapchainCreateInfo, nullptr, &m_swapchain));

    if (m_swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_device);
        vkDestroySwapchainKHR(m_device, m_swapchainCreateInfo.oldSwapchain, VK_NULL_HANDLE);
        for (auto& swapchainBuffer : m_swapchainBuffers) {
            swapchainBuffer.Destroy(m_device);
        }
    }

    m_currentFrameIndex = 0;

    const VkExtent3D newExtent{ m_swapchainCreateInfo.imageExtent.width, m_swapchainCreateInfo.imageExtent.height, 1 };
    m_depthBuffer->Resize(newExtent);
    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer->Resize(newExtent);
        m_msaaDepthBuffer->Resize(newExtent);
    }

    std::vector<VkImage> swapchainImages = GetSwapchainImages();
    m_swapchainImagesCount = static_cast<uint32_t>(swapchainImages.size());

    m_swapchainBuffers.resize(m_swapchainImagesCount);
    for (uint32_t i = 0; i < m_swapchainImagesCount; i++) {
        auto image{ swapchainImages[i] };
        auto imageView{ util::vk::CreateImageView(m_device, image, m_swapchainCreateInfo.imageFormat, VK_IMAGE_VIEW_TYPE_2D, 1, VK_IMAGE_ASPECT_COLOR_BIT) };

        std::vector<VkImageView> swapchainImageViews;
        if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            swapchainImageViews.push_back(m_msaaColorBuffer->GetImageView());
            swapchainImageViews.push_back(m_msaaDepthBuffer->GetImageView());
            swapchainImageViews.push_back(imageView); // Add color buffer (unique)
            swapchainImageViews.push_back(m_depthBuffer->GetImageView()); // Add depth buffer (shared)
        } else {
            swapchainImageViews.push_back(imageView); // Add color buffer (unique)
            swapchainImageViews.push_back(m_depthBuffer->GetImageView()); // Add depth buffer (shared)
        }

        auto& swapchainBuffer = m_swapchainBuffers[i];
        swapchainBuffer.image = image;
        swapchainBuffer.view = imageView;
        swapchainBuffer.framebuffer = util::vk::CreateFrameBuffer(m_device, m_renderPass, swapchainImageViews, m_swapchainCreateInfo.imageExtent);
        swapchainBuffer.commandBuffer = util::vk::CreateCommandBuffer(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        swapchainBuffer.fence = util::vk::CreateFence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
        swapchainBuffer.extent = m_swapchainCreateInfo.imageExtent;
    }

    LOGI("---Extent = %d x %d\n", newExtent.width, newExtent.height);

    if (m_swapchainCreateInfo.oldSwapchain == VK_NULL_HANDLE) {
        LOGI("Swapchain created\n");
    }
}

VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCapabilities() const
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VKERRCHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_device.GetGPU(), m_surface, &surfaceCapabilities));
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
    ASSERT(!m_isAcquired, "Swapchain: Previous swapchain buffer has not yet been presented.\n");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_currentFrameIndex] };

    VKERRCHECK(vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX));
    VKERRCHECK(vkResetFences(m_device, 1, &swapchainBuffer.fence));

    uint32_t acquireIndex;
    const auto result{ vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_acquireSemaphore, VK_NULL_HANDLE, &acquireIndex) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        UpdateExtent();
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
    ASSERT(!!m_isAcquired, "Swapchain: A buffer must be acquired before submitting.\n");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_currentFrameIndex] };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_acquireSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_renderSemaphore;

    VKERRCHECK(m_graphicsQueue->Submit(1, &submitInfo, swapchainBuffer.fence));
}

void Swapchain::Present()
{
    ASSERT(!!m_isAcquired, "Swapchain: A buffer must be acquired before presenting.\n");

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_renderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_acquiredIndex;

    bool swapchainChanged{ false };

    const auto result{ vkQueuePresentKHR(*m_presentQueue, &presentInfo) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapchainChanged = UpdateExtent();
    } else if (result != VK_SUCCESS) {
        ShowVkResult(result);
    }

    if (!swapchainChanged) {
        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_swapchainImagesCount;
    }

    m_isAcquired = false;
}

bool Swapchain::BeginFrame(SwapChainFrameContext& outContex)
{
    SwapchainBuffer swapchainBuffer;
    if (!AcquireNext(swapchainBuffer)) {
        return false;
    }

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

    outContex = { swapchainBuffer.framebuffer, swapchainBuffer.commandBuffer, m_acquiredIndex };
    return true;
}

void Swapchain::EndFrame()
{
    const auto& swapchainBuffer{ m_swapchainBuffers[m_currentFrameIndex] };
    VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

    Submit();
    Present();
}
} // namespace prev::render