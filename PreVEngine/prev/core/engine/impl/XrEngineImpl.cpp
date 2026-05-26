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

    m_rootRenderer.reset();
    m_scene.reset();

    m_swapchain.reset(); // destroy swapchain before XR session (it references XR-owned textures)

    if (m_xr) {
        m_xr->DestroySession();
        m_xr.reset();
    }
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
    m_instance = instanceFactory.Create(m_config.appName, m_config.validation, m_config.renderBackend, m_xr->GetVulkanInstanceExtensions());
}

void XrEngineImpl::ResetDevice()
{
    // Ask OpenXR which adapter to use (internally matches VkPhysicalDevice)
    GfxAdapter selectedAdapter = m_xr->GetPhysicalDevice(*m_instance);
    if (!selectedAdapter) {
        throw std::runtime_error("Could not find GfxAdapter matching OpenXR physical device");
    }

    prev::core::device::PhysicalDevice physicalDevice{ selectedAdapter };

    const std::vector<std::string> extensions{
        GFX_DEVICE_EXTENSION_SWAPCHAIN,
        GFX_DEVICE_EXTENSION_ANISOTROPIC_FILTERING,
        GFX_DEVICE_EXTENSION_NON_SOLID_FILL,
        GFX_DEVICE_EXTENSION_MULTIVIEW,
    };

    m_device = prev::core::device::DeviceFactory{}.Create(physicalDevice, extensions, m_xr->GetVulkanDeviceExtensions());
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();

    const auto& queue = m_device->GetQueue(prev::core::device::QueueType::GRAPHICS);
    m_xr->UpdateGraphicsBinding(*m_instance, selectedAdapter, *m_device, queue.family, queue.index);
}

void XrEngineImpl::ResetRenderPass()
{
    const auto colorFormat = m_xr->GetColorFormat();
    const auto depthFormat = m_xr->GetDepthFormat();
    const uint32_t viewCount = GetViewCount();
    const bool storeColor = true;
    const bool storeDepth = m_xr->HasDepthImages();
    const GfxSampleCount sampleCount = static_cast<GfxSampleCount>(m_config.samplesCount);

    if (m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, colorFormat, depthFormat, sampleCount, viewCount, storeColor, storeDepth);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, colorFormat, depthFormat, viewCount, storeColor, storeDepth);
    }
}

void XrEngineImpl::ResetSwapchain()
{
    const GfxSampleCount sampleCount = static_cast<GfxSampleCount>(m_config.samplesCount);
    m_swapchain = std::make_unique<prev::xr::XrSwapchain>(*m_device, *m_renderPass, *m_xr, sampleCount);
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl

#endif