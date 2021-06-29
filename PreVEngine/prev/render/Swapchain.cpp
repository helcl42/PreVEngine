#include "Swapchain.h"
#include "../core/AllocatorProvider.h"
#include "../core/DeviceProvider.h"
#include "../core/memory/image/ColorImageBuffer.h"
#include "../core/memory/image/DepthImageBuffer.h"
#include "../util/MathUtils.h"
#include "../util/VkUtils.h"

#include <algorithm>

namespace prev::render {
Swapchain::Swapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount)
    : m_device(device)
    , m_allocator(allocator)
    , m_renderPass(renderPass)
    , m_surface(surface)
    , m_sampleCount(sampleCount)
{
    m_graphicsQueue = m_device.GetQueue(core::device::QueueType::GRAPHICS);
    m_presentQueue = m_device.GetQueue(core::device::QueueType::PRESENT);

    m_swapchain = VK_NULL_HANDLE;
    m_isAcquired = false;

    VkSurfaceCapabilitiesKHR surfaceCapabilities = GetSurfaceCapabilities();
    assert(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    assert(surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    assert(surfaceCapabilities.supportedCompositeAlpha & (VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR | VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR));

    m_swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    m_swapchainCreateInfo.surface = m_surface;
    m_swapchainCreateInfo.imageFormat = m_renderPass.GetFormat();
    m_swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    m_swapchainCreateInfo.imageArrayLayers = 1; // 2 for stereo
    m_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    m_swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    m_swapchainCreateInfo.clipped = VK_TRUE;
    m_swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    if (m_presentQueue->family != m_graphicsQueue->family) {
        const uint32_t families[] = { m_presentQueue->family, m_graphicsQueue->family };
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        m_swapchainCreateInfo.queueFamilyIndexCount = 2;
        m_swapchainCreateInfo.pQueueFamilyIndices = families;
    } else {
        m_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    m_swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    UpdateExtent();
    SetImageCount(3);

    m_commandPool = m_graphicsQueue->CreateCommandPool();

    m_depthBuffer = std::make_unique<core::memory::image::DepthImageBuffer>(m_allocator);
    m_depthBuffer->Create(core::memory::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, renderPass.GetDepthFormat(), VK_SAMPLE_COUNT_1_BIT, 0, false, false, VK_IMAGE_VIEW_TYPE_2D });

    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer = std::make_unique<core::memory::image::ColorImageBuffer>(m_allocator);
        m_msaaColorBuffer->Create(core::memory::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, m_renderPass.GetFormat(), m_sampleCount, 0, false, false, VK_IMAGE_VIEW_TYPE_2D });
        m_msaaDepthBuffer = std::make_unique<core::memory::image::DepthImageBuffer>(m_allocator);
        m_msaaDepthBuffer->Create(core::memory::image::ImageBufferCreateInfo{ m_swapchainCreateInfo.imageExtent, VK_IMAGE_TYPE_2D, m_renderPass.GetDepthFormat(), m_sampleCount, 0, false, false, VK_IMAGE_VIEW_TYPE_2D });
    }

    VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VKERRCHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_acquireSemaphore));
    VKERRCHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_submitSemaphore));

    Apply();
}

Swapchain::~Swapchain()
{
    vkDeviceWaitIdle(m_device);

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    if (m_submitSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_submitSemaphore, nullptr);
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

void Swapchain::UpdateExtent()
{
    const VkSurfaceCapabilitiesKHR surfaceCapabilities{ GetSurfaceCapabilities() };
    const VkExtent2D& currentSurfaceExtent{ surfaceCapabilities.currentExtent };

    if (currentSurfaceExtent.width == 0 || currentSurfaceExtent.height == 0) {
        return;
    }

    if (currentSurfaceExtent.width == 0xFFFFFFFF) // 0xFFFFFFFF indicates surface size is set from extent
    {
        const uint32_t defaultWidth = 256;
        const uint32_t defaultHeight = 256;

        LOGW("Can't determine current window surface extent from surface caps. Using defaults instead. (%d x %d)\n", defaultWidth, defaultHeight);

        m_swapchainCreateInfo.imageExtent.width = util::math::Clamp(defaultWidth, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        m_swapchainCreateInfo.imageExtent.height = util::math::Clamp(defaultHeight, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    } else {
        // because of android notifies about SUBOPTIMAL presence -> it internaly transforms image(rotates) because we use preTransform == VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
        if (currentSurfaceExtent.width == m_swapchainCreateInfo.imageExtent.width && currentSurfaceExtent.height == m_swapchainCreateInfo.imageExtent.height) {
            return;
        }

        m_swapchainCreateInfo.imageExtent = currentSurfaceExtent;
    }

    if (!!m_swapchain) {
        Apply();
    }
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
bool Swapchain::SetPresentMode(bool noTearing, bool powersave)
{
    VkPresentModeKHR mode = static_cast<VkPresentModeKHR>((noTearing ? VK_PRESENT_MODE_MAILBOX_KHR : 0) ^ (powersave ? VK_PRESENT_MODE_FIFO_RELAXED_KHR : 0));
    return SetPresentMode(mode); // if not found, use FIFO
}

bool Swapchain::SetPresentMode(VkPresentModeKHR preferredMode)
{
    VkPresentModeKHR& mode = m_swapchainCreateInfo.presentMode;
    auto modes = GetPresentModes();

    mode = VK_PRESENT_MODE_FIFO_KHR; // default to FIFO mode
    for (auto m : modes) {
        if (m == preferredMode) {
            mode = preferredMode; // if prefered mode is available, select it.
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
    printf("Swapchain:\n");

    printf("\tFormat  = %3d : %s\n", m_swapchainCreateInfo.imageFormat, util::vk::FormatToString(m_swapchainCreateInfo.imageFormat).c_str());
    printf("\tDepth   = %3d : %s\n", m_depthBuffer->GetFormat(), util::vk::FormatToString(m_depthBuffer->GetFormat()).c_str());

    const auto& extent = m_swapchainCreateInfo.imageExtent;
    printf("\tExtent  = %d x %d\n", extent.width, extent.height);
    printf("\tBuffers = %d\n", (int)m_swapchainBuffers.size());

    auto modes = GetPresentModes();
    printf("\tPresentMode:\n");
    const auto& mode = m_swapchainCreateInfo.presentMode;
    for (auto m : modes) {
        print((m == mode) ? ConsoleColor::RESET : ConsoleColor::FAINT, "\t\t%s %s\n", (m == mode) ? cTICK : " ", util::vk::PresentModeToString(m).c_str());
    }
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
    VKERRCHECK(vkCreateSwapchainKHR(m_device, &m_swapchainCreateInfo, nullptr, &m_swapchain));

    if (m_swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_device);
        vkDestroySwapchainKHR(m_device, m_swapchainCreateInfo.oldSwapchain, 0);
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
            swapchainImageViews.push_back(imageView); // Add color buffer (unique)
            swapchainImageViews.push_back(m_msaaDepthBuffer->GetImageView());
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
        swapchainBuffer.fence = util::vk::CreateFence(m_device);
        swapchainBuffer.extent = m_swapchainCreateInfo.imageExtent;
    }

    printf("---Extent = %d x %d\n", newExtent.width, newExtent.height);

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

    uint32_t swapchainImagesCount = 0;
    VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, nullptr));
    swapchainImages.resize(swapchainImagesCount);
    VKERRCHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImagesCount, swapchainImages.data()));

    return swapchainImages;
}

bool Swapchain::AcquireNext(SwapchainBuffer& next)
{
    ASSERT(!m_isAcquired, "CSwapchain: Previous swapchain buffer has not yet been presented.\n");

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

    const auto& swapchainBuffer{ m_swapchainBuffers.at(m_acquiredIndex) };

    vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX);

    next = swapchainBuffer;

    return true;
}

void Swapchain::Submit()
{
    ASSERT(!!m_isAcquired, "CSwapchain: A buffer must be acquired before submitting.\n");

    auto& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_acquireSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_submitSemaphore;

    vkResetFences(m_device, 1, &swapchainBuffer.fence);

    VKERRCHECK(vkQueueSubmit(*m_graphicsQueue, 1, &submitInfo, swapchainBuffer.fence));
}

void Swapchain::Present()
{
    ASSERT(!!m_isAcquired, "CSwapchain: A buffer must be acquired before presenting.\n");

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_submitSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_acquiredIndex;

    const auto result{ vkQueuePresentKHR(*m_presentQueue, &presentInfo) };
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        UpdateExtent();
    } else {
        ShowVkResult(result);
    }

    m_isAcquired = false;
}

bool Swapchain::BeginFrame(VkFramebuffer& frmmeBuffer, VkCommandBuffer& commandBuffer, uint32_t& acquiredIndex)
{
    SwapchainBuffer swapchainBuffer;
    if (!AcquireNext(swapchainBuffer)) {
        return false;
    }

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

    frmmeBuffer = swapchainBuffer.framebuffer;
    commandBuffer = swapchainBuffer.commandBuffer;
    acquiredIndex = m_acquiredIndex;
    return true;
}

void Swapchain::EndFrame()
{
    auto& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];
    VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

    Submit();
    Present();

    m_currentFrameIndex = (m_currentFrameIndex + 1) % m_swapchainImagesCount;
}
} // namespace prev::render