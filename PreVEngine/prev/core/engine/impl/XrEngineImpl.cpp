#include "XrEngineImpl.h"

#ifdef ENABLE_XR

#include "../../device/DeviceFactory.h"
#include "../../instance/InstanceFactory.h"

#include "../../../xr/XrFactory.h"
#include "../../../xr/XrSwapchain.h"

namespace prev::core::engine::impl {
XrEngineImpl::XrEngineImpl(const Config& config)
    : EngineImpl(config)
{
}

XrEngineImpl::~XrEngineImpl()
{
    ShutDown();
}

uint32_t XrEngineImpl::GetViewCount() const
{
    return m_xr->GetViewCount();
}

float XrEngineImpl::GetCurrentDeltaTime() const
{
    return m_xr->GetCurrentDeltaTime();
}

void XrEngineImpl::Init()
{
    m_xr = prev::xr::XrFactory{}.Create();

    ResetTiming();
    ResetInstance();
    ResetWindow();
    ResetSurface();
    ResetDevice();
    ResetAllocator();
    ResetRenderPass();

    m_xr->CreateSession();

    ResetSwapchain();
}

void XrEngineImpl::ShutDown()
{
    if (m_rootRenderer) {
        m_rootRenderer->ShutDown();
    }
    if (m_scene) {
        m_scene->ShutDown();
    }

    m_rootRenderer = nullptr;
    m_scene = nullptr;

    m_xr->DestroySession();
    m_xr = nullptr;
}

bool XrEngineImpl::Update()
{
    bool result{ m_window->ProcessEvents() };
    m_xr->PollEvents();
    m_clock->UpdateClock();
    return result;
}

bool XrEngineImpl::BeginFrame()
{
    return m_xr->BeginFrame();
}

void XrEngineImpl::PollActions()
{
    m_xr->PollActions();
}

bool XrEngineImpl::EndFrame()
{
    bool result{ m_xr->EndFrame() };
    UpdateFps();
    return result;
}

void XrEngineImpl::ResetInstance()
{
    prev::core::instance::InstanceFactory instanceFactory{};
    m_instance = instanceFactory.Create(m_config.validation, m_config.appName, std::vector<std::string>{}, m_xr->GetVulkanInstanceExtensions());
}

void XrEngineImpl::ResetDevice()
{
    prev::core::device::PhysicalDevice physicalDevice{ m_xr->GetPhysicalDevice(*m_instance), m_xr->GetVulkanDeviceExtensions() };

    prev::core::device::DeviceFactory deviceFactory{};
    m_device = deviceFactory.Create(physicalDevice, m_surface);
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();

    const auto& queue{ m_device->GetQueue(prev::core::device::QueueType::GRAPHICS) };
    m_xr->UpdateGraphicsBinding(*m_instance, m_device->GetGPU(), *m_device, queue.family, queue.index);
}

void XrEngineImpl::ResetRenderPass()
{
    const auto colorFormat{ m_xr->GetColorFormat() };
    const auto depthFormat{ m_xr->GetDepthFormat() };

    const uint32_t viewCount{ GetViewCount() };

    const bool storeColor{ true };
    const bool storeDepth{ m_xr->HasDepthImages() };

    const VkImageLayout colorLayout{ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    const VkImageLayout depthLayout{ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    if (m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, colorFormat, depthFormat, prev::util::vk::GetSampleCountBit(m_config.samplesCount), viewCount, storeColor, storeDepth, colorLayout, depthLayout);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, colorFormat, depthFormat, GetViewCount(), storeColor, storeDepth, colorLayout, depthLayout);
    }
}

void XrEngineImpl::ResetSwapchain()
{
    m_swapchain = std::make_unique<prev::xr::XrSwapchain>(*m_device, *m_allocator, *m_renderPass, *m_xr, prev::util::vk::GetSampleCountBit(m_config.samplesCount));
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl

#endif