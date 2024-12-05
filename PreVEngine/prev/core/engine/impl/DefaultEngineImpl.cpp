#include "DefaultEngineImpl.h"

#include "../../../render/Swapchain.h"
#include "../../device/PhysicalDevices.h"
#include "../../device/DeviceFactory.h"

namespace prev::core::engine::impl {
DefaultEngineImpl::DefaultEngineImpl(const Config& config)
    : EngineImpl(config)
{
}

uint32_t DefaultEngineImpl::GetViewCount() const
{
    return 1;
}

float DefaultEngineImpl::GetCurrentDeltaTime() const
{
    return m_clock->GetDelta();
}

VkExtent2D DefaultEngineImpl::GetExtent() const
{
    return { m_window->GetSize().width, m_window->GetSize().height };
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

bool DefaultEngineImpl::EndFrame()
{
    UpdateFps();
    return true;
}

void DefaultEngineImpl::ResetInstance()
{
    m_instance = std::make_unique<prev::core::instance::Instance>(m_config.validation);
}

void DefaultEngineImpl::ResetDevice()
{
    auto physicalDevices{ std::make_shared<prev::core::device::PhysicalDevices>(*m_instance) };
    physicalDevices->Print();

    auto presentablePhysicalDevice{ physicalDevices->FindPresentable(m_surface, m_config.gpuIndex) };
    if (!presentablePhysicalDevice) {
        throw std::runtime_error("No suitable GPU found?!");
    }

    prev::core::device::DeviceFactory deviceFactory{};
    auto device{ deviceFactory.Create(presentablePhysicalDevice, m_surface) };
    if (!device) {
        throw std::runtime_error("Could not create logical device");
    }

    m_device = device;
    m_device->Print();
}

void DefaultEngineImpl::ResetRenderPass()
{
    if(m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount), GetViewCount(), true, false);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, m_surface, GetViewCount(), true, false);
    }
}

void DefaultEngineImpl::ResetSwapchain()
{
    m_swapchain = std::make_shared<prev::render::Swapchain>(*m_device, *m_allocator, *m_renderPass, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount), 1);
#if defined(__ANDROID__)
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR);
#else
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
    m_swapchain->SetImageCount(m_config.swapchainFrameCount);
    m_swapchain->Print();
}
}