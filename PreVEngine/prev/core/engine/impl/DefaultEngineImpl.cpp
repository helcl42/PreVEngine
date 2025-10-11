#include "DefaultEngineImpl.h"

#include "../../device/DeviceFactory.h"
#include "../../device/PhysicalDevices.h"
#include "../../instance/InstanceFactory.h"

#include "../../../render/swapchain/SwapchainFactory.h"

namespace prev::core::engine::impl {
DefaultEngineImpl::DefaultEngineImpl(const Config& config)
    : EngineImpl(config)
{
}

DefaultEngineImpl::~DefaultEngineImpl()
{
    ShutDown();
}

uint32_t DefaultEngineImpl::GetViewCount() const
{
    return 1;
}

float DefaultEngineImpl::GetCurrentDeltaTime() const
{
    return m_clock->GetDelta();
}

void DefaultEngineImpl::Init()
{
    ResetTiming();
    ResetInstance();
    ResetWindow();
    ResetSurface();
    ResetDevice();
    ResetAllocator();
    ResetRenderPass();
    ResetSwapchain();
}

void DefaultEngineImpl::ShutDown()
{
    if (m_rootRenderer) {
        m_rootRenderer->ShutDown();
    }
    if (m_scene) {
        m_scene->ShutDown();
    }

    m_rootRenderer = nullptr;
    m_scene = nullptr;
}

bool DefaultEngineImpl::Update()
{
    bool result{ m_window->ProcessEvents() };
    m_clock->UpdateClock();
    return result;
}

bool DefaultEngineImpl::BeginFrame()
{
    return true;
}

void DefaultEngineImpl::PollActions()
{
}

bool DefaultEngineImpl::EndFrame()
{
    UpdateFps();
    return true;
}

void DefaultEngineImpl::ResetInstance()
{
    prev::core::instance::InstanceFactory instanceFactory{};
    m_instance = instanceFactory.Create(m_config.validation, m_config.appName);
}

void DefaultEngineImpl::ResetDevice()
{
    prev::core::device::PhysicalDevices physicalDevices{ *m_instance };
    physicalDevices.Print();

    auto presentablePhysicalDevice{ physicalDevices.FindPresentable(m_surface, m_config.gpuIndex) };
    if (!presentablePhysicalDevice) {
        throw std::runtime_error("No suitable GPU found?!");
    }

    prev::core::device::DeviceFactory deviceFactory{};
    m_device = deviceFactory.Create(*presentablePhysicalDevice, m_surface);
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();
}

void DefaultEngineImpl::ResetRenderPass()
{
    const auto colorFormat{ m_device->GetGPU().FindSurfaceFormat(m_surface).format };
    const auto depthFormat{ m_device->GetGPU().FindDepthFormat() };

    const uint32_t viewCount{ GetViewCount() };

    const bool storeColor{ true };
    const bool storeDepth{ false };

    const VkImageLayout colorLayout{ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };
    const VkImageLayout depthLayout{ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    if (m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, colorFormat, depthFormat, prev::util::vk::GetSampleCountBit(m_config.samplesCount), viewCount, storeColor, storeDepth, colorLayout, depthLayout);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, colorFormat, depthFormat, viewCount, storeColor, storeDepth, colorLayout, depthLayout);
    }
}

void DefaultEngineImpl::ResetSwapchain()
{
    m_swapchain = prev::render::swapchain::SwapchainFactory{}.Create(*m_device, *m_allocator, *m_renderPass, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount), GetViewCount());
#if defined(__ANDROID__)
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR);
#else
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
    m_swapchain->SetImageCount(m_config.swapchainFrameCount);
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl