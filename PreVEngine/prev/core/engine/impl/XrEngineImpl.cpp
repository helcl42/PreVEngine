#include "XrEngineImpl.h"

#include "../../../xr/XrTimeProvider.h"

#ifdef ENABLE_XR

#include "../../Formats.h"
#include "../../device/Adapters.h"
#include "../../device/Device.h"
#include "../../device/DeviceFactory.h"
#include "../../device/Queue.h"
#include "../../instance/InstanceFactory.h"

#include "../../../common/Logger.h"
#include "../../../event/EventChannel.h"
#include "../../../time/TimeProviderFactory.h"
#include "../../../xr/XrEvents.h"
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

prev::render::swapchain::ISwapchain& XrEngineImpl::GetSwapchain() const
{
    return IsDrawingToWindow() ? *m_windowSwapchain : *m_xrSwapchain;
}

std::unique_ptr<prev::time::ITimeProvider> XrEngineImpl::CreateTimeProvider() const
{
    return std::make_unique<prev::xr::XrTimeProvider>(*m_xr, prev::time::TimeProviderFactory{}.Create(m_config.fixedDeltaTime));
}

void XrEngineImpl::Init()
{
    m_xr = prev::xr::XrFactory{}.Create(m_config.xrMode, m_config.colorManaged);

    ResetTiming();
    ResetInstance();
    ResetWindow();
    if (UsesWindowSurface()) {
        ResetSurface();
    }
    ResetDevice();

    m_xr->CreateSession();

    ResetRenderPass();
    ResetXrSwapchain();
    ResetSwapchain();
}

void XrEngineImpl::ShutDown()
{
    if (m_device) {
        m_device->WaitIdle();
    }
    if (m_rootRenderer) {
        m_rootRenderer->ShutDown();
    }
    if (m_scene) {
        m_scene->ShutDown();
    }

    m_rootRenderer.reset();
    m_scene.reset();

    ReleaseSwapchain();
    ReleaseXrSwapchain(); // before the XR session: it references the session's textures

    m_time.reset();

    if (m_xr) {
        m_xr->DestroySession();
        m_xr.reset();
    }
}

bool XrEngineImpl::Update()
{
    bool result{ m_window->ProcessEvents() };
    m_xr->PollEvents();
    if (m_xr->IsExitRequested()) {
        m_window->Close(); // the same door as the game's Quit: on Android the activity finishes first
    }
    UpdateSessionState();
    m_time->Update();
    return result;
}

bool XrEngineImpl::BeginFrame()
{
    if (IsDrawingToWindow()) {
        return true;
    }
    return m_xr->BeginFrame();
}

void XrEngineImpl::PollActions()
{
    if (!IsDrawingToWindow()) {
        m_xr->PollActions();
    }
}

bool XrEngineImpl::EndFrame()
{
    bool result{ true };
    if (!IsDrawingToWindow()) {
        result = m_xr->EndFrame();
    }
    UpdateFps();
    return result;
}

void XrEngineImpl::RunFrameLoop(const std::function<bool()>& tick)
{
    m_xr->RunFrameLoop(tick); // OpenXR: runs every frame until the app quits; WebXR: only arms the session's frame callback
    if (m_xr->IsSessionOptional()) {
        RunWindowFrameLoop([this, tick]() { // the window ticks the game while no session runs
            if (m_xr->IsSessionRunning()) {
                return true;
            }
            return tick();
        });
    }
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
        const GfxSurface surface = m_surface ? static_cast<GfxSurface>(*m_surface) : nullptr;
        prev::core::device::Adapters adapters{ m_instance->GetHandle() };
        const auto adapter{ adapters.Find(surface, m_config.gpuIndex) };
        if (!adapter) {
            throw std::runtime_error("Could not find a suitable GPU adapter");
        }
        std::vector<std::string> extensions{ m_xr->GetRequiredDeviceExtensions() };
        if (surface) {
            extensions.push_back(GFX_DEVICE_EXTENSION_SWAPCHAIN);
        }
        m_device = prev::core::device::DeviceFactory{}.Create(*adapter, extensions);
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

    // Same reconciliation as the surface path: color management holds only if the actually-selected XR
    // color format is sRGB. CreateSession ran first, so GetColorFormat() reflects any runtime fallback.
    if (m_config.colorManaged && !prev::core::format::IsSrgb(colorFormat)) {
        LOGW("colorManaged requested but the XR runtime provides no sRGB swapchain format; falling back to gamma passthrough");
        m_config.colorManaged = false;
    }
    const uint32_t viewCount = GetPassViewCount();
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
    if (m_surface) {
        m_windowSwapchain = CreateWindowSwapchain(GetPassViewCount()); // shares the render pass: WebXR draws in the window's own format
        m_windowSwapchain->Print();
    }
}

void XrEngineImpl::ReleaseSwapchain()
{
    m_windowSwapchain.reset();
}

bool XrEngineImpl::UsesWindowSurface() const
{
    return m_xr->IsSessionOptional(); // WebXR: the page between sessions; OpenXR shows nothing in the window
}

void XrEngineImpl::ResetXrSwapchain()
{
    m_xrSwapchain = std::make_unique<prev::xr::XrSwapchain>(*m_device, *m_renderPass, *m_xr, static_cast<GfxSampleCount>(m_config.samplesCount), m_config.maxFramesInFlight);
    m_xrSwapchain->Print();
}

void XrEngineImpl::ReleaseXrSwapchain()
{
    m_xrSwapchain.reset();
}

uint32_t XrEngineImpl::GetPassViewCount() const
{
    // The number of views rendered in ONE pass (the multiview capability), not the XR eye count. With multiview
    // shaders (OpenXR) that is MAX_PER_PASS_VIEW_COUNT_VALUE (2); on WebGPU, which has no multiview,
    // MAX_PER_PASS_VIEW_COUNT_VALUE is 1 so the pass is mono and stereo is done per-eye. Clamped to the runtime
    // view count so we never exceed what the runtime provides.
    const uint32_t maxViews = static_cast<uint32_t>(MAX_PER_PASS_VIEW_COUNT_VALUE);
    const uint32_t xrViews = GetViewCount();
    return (xrViews < maxViews) ? xrViews : maxViews;
}

bool XrEngineImpl::IsDrawingToWindow() const
{
    return !m_sessionRunning && m_windowSwapchain;
}

void XrEngineImpl::UpdateSessionState()
{
    const bool running{ m_xr->IsSessionRunning() };
    if (running != m_sessionRunning) {
        m_sessionRunning = running;
        prev::event::EventChannel::Post(prev::xr::XrSessionChangedEvent{ running });
    }

    const bool focused{ m_xr->IsSessionFocused() };
    if (focused != m_sessionFocused) {
        m_sessionFocused = focused;
        prev::event::EventChannel::Post(prev::xr::XrSessionFocusChangedEvent{ focused });
    }
}
} // namespace prev::core::engine::impl

#endif