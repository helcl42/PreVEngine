#include "XrEngineImpl.h"

#ifdef ENABLE_XR

#include "../../../xr/XrSwapchain.h"
#include "../../device/DeviceFactory.h"

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
    return m_openXr->GetViewCount();
}

float XrEngineImpl::GetCurrentDeltaTime() const
{
    return m_openXr->GetCurrentDeltaTime();
}

void XrEngineImpl::Init()
{
    m_openXr = std::make_unique<prev::xr::OpenXr>();

    ResetTiming();
    ResetInstance();
    ResetWindow();
    ResetSurface();
    ResetDevice();
    ResetAllocator();
    ResetRenderPass();

    m_openXr->CreateSession();

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

    m_openXr->DestroySession();
    m_openXr = nullptr;
}

bool XrEngineImpl::Update()
{
    bool result{ m_window->ProcessEvents() };
    m_openXr->PollEvents();
    m_clock->UpdateClock();
    return result;
}

bool XrEngineImpl::BeginFrame()
{
    return m_openXr->BeginFrame();
}

void XrEngineImpl::PollActions()
{
    m_openXr->PollActions();
}

bool XrEngineImpl::EndFrame()
{
    bool result{ m_openXr->EndFrame() };
    UpdateFps();
    return result;
}

void XrEngineImpl::ResetInstance()
{
    m_instance = std::make_unique<prev::core::instance::Instance>(m_config.validation, std::vector<std::string>{}, m_openXr->GetVulkanInstanceExtensions());
}

void XrEngineImpl::ResetDevice()
{
    prev::core::device::PhysicalDevice physicalDevice{ m_openXr->GetPhysicalDevice(*m_instance), m_openXr->GetVulkanDeviceExtensions() };

    prev::core::device::DeviceFactory deviceFactory{};
    auto device{ deviceFactory.Create(physicalDevice, m_surface) };
    if (!device) {
        throw std::runtime_error("Could not create logical device");
    }

    m_device = std::move(device);
    m_device->Print();

    const auto& queue{ m_device->GetQueue(prev::core::device::QueueType::GRAPHICS) };
    m_openXr->UpdateGraphicsBinding(*m_instance, m_device->GetGPU(), *m_device, queue.family, queue.index);
}

void XrEngineImpl::ResetRenderPass()
{
    const auto colorFormat{ m_openXr->GetColorFormat() };
    const auto depthFormat{ m_openXr->GetDepthFormat() };

    const bool storeColor{ true };
    const bool storeDepth{ m_openXr->HasDepthImages() };

    if (m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, colorFormat, depthFormat, prev::util::vk::GetSampleCountBit(m_config.samplesCount), GetViewCount(), storeColor, storeDepth);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, colorFormat, depthFormat, GetViewCount(), storeColor, storeDepth);
    }
}

void XrEngineImpl::ResetSwapchain()
{
    m_swapchain = std::make_unique<prev::xr::XrSwapchain>(*m_device, *m_allocator, *m_renderPass, *m_openXr, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount));
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl

#endif