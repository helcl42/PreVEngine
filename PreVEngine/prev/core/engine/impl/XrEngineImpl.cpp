#include "XrEngineImpl.h"

#ifdef ENABLE_XR

#include "../../device/Adapters.h"
#include "../../device/Device.h"
#include "../../device/DeviceFactory.h"
#include "../../device/Queue.h"
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
    m_xr = prev::xr::XrFactory{}.Create(m_config.xrMode);

    ResetTiming();
    ResetInstance();
    ResetWindow();
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

void XrEngineImpl::RunFrameLoop(const std::function<bool()>& tick)
{
    m_xr->RunFrameLoop(tick);
}

void XrEngineImpl::ResetInstance()
{
    prev::core::instance::InstanceFactory instanceFactory{};
    m_instance = instanceFactory.Create(m_config.appName, m_config.validation, m_config.renderBackend, m_xr->GetRequiredInstanceExtensions());
}

void XrEngineImpl::ResetDevice()
{
    GfxAdapter selectedAdapter = m_xr->GetAdapter(*m_instance);

    if (selectedAdapter) {
        prev::core::device::Adapter adapter{ selectedAdapter };
        const std::vector<std::string> extensions{
            GFX_DEVICE_EXTENSION_SWAPCHAIN,
            GFX_DEVICE_EXTENSION_ANISOTROPIC_FILTERING,
            GFX_DEVICE_EXTENSION_NON_SOLID_FILL,
            GFX_DEVICE_EXTENSION_MULTIVIEW,
        };
        m_device = prev::core::device::DeviceFactory{}.Create(adapter, extensions, m_xr->GetRequiredDeviceExtensions());
    } else {
        prev::core::device::Adapters adapters{ m_instance->GetHandle() };
        const auto adapter{ adapters.Find(nullptr, m_config.gpuIndex) };
        if (!adapter) {
            throw std::runtime_error("Could not find a suitable GPU adapter");
        }
        m_device = prev::core::device::DeviceFactory{}.Create(*adapter, m_xr->GetRequiredDeviceExtensions());
    }

    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();

    const auto& queue = m_device->GetQueue(prev::core::device::QueueType::GRAPHICS);
    m_xr->UpdateGraphicsBinding(*m_instance, selectedAdapter, *m_device, queue);
}

void XrEngineImpl::ResetRenderPass()
{
    const auto colorFormat = m_xr->GetColorFormat();
    const auto depthFormat = m_xr->GetDepthFormat();
    // The render pass's view count is the number of views rendered in ONE pass (the multiview capability),
    // not the XR eye count. With multiview shaders (OpenXR) that is MAX_PER_PASS_VIEW_COUNT_VALUE (2); on WebGPU,
    // which has no multiview, MAX_PER_PASS_VIEW_COUNT_VALUE is 1 so the pass is mono and stereo is done per-eye.
    // Clamp to the runtime view count so we never exceed what the runtime provides.
    const uint32_t maxViews = static_cast<uint32_t>(MAX_PER_PASS_VIEW_COUNT_VALUE);
    const uint32_t xrViews = GetViewCount();
    const uint32_t viewCount = (xrViews < maxViews) ? xrViews : maxViews;
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
    m_swapchain = std::make_unique<prev::xr::XrSwapchain>(*m_device, *m_renderPass, *m_xr, sampleCount, m_config.maxFramesInFlight);
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl

#endif