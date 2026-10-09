#include "DefaultEngineImpl.h"

#include "../../Formats.h"
#include "../../device/Adapters.h"
#include "../../device/Device.h"
#include "../../device/DeviceFactory.h"
#include "../../device/Queue.h"
#include "../../instance/InstanceFactory.h"

#include "../../../common/Logger.h"
#include "../../../time/TimeProviderFactory.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace prev::core::engine::impl {
DefaultEngineImpl::DefaultEngineImpl(const Config& config)
    : EngineImpl(config)
{
}

DefaultEngineImpl::~DefaultEngineImpl()
{
    ShutDown();
}

std::unique_ptr<prev::time::ITimeProvider> DefaultEngineImpl::CreateTimeProvider() const
{
    return prev::time::TimeProviderFactory{}.Create(m_config.fixedDeltaTime);
}

void DefaultEngineImpl::Init()
{
    ResetTiming();
    ResetInstance();
    ResetWindow();
    ResetSurface();
    ResetDevice();
    ResetRenderPass();
    ResetSwapchain();
}

void DefaultEngineImpl::ShutDown()
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

    m_swapchain.reset();
    m_renderPass.reset();
    m_surface.reset(); // Surface destructor calls gfxSurfaceDestroy
    m_device.reset(); // Device destructor calls gfxDeviceDestroy
    m_instance.reset(); // Instance destructor calls gfxInstanceDestroy
}

bool DefaultEngineImpl::Update()
{
    bool result{ m_window->ProcessEvents() };
    m_time->Update();
    return result;
}

bool DefaultEngineImpl::BeginFrame()
{
    return m_swapchain != nullptr; // no surface (Android background): skip the frame, keep pumping events
}

void DefaultEngineImpl::PollActions()
{
}

bool DefaultEngineImpl::EndFrame()
{
    UpdateFps();
    return true;
}

prev::render::swapchain::ISwapchain& DefaultEngineImpl::GetSwapchain() const
{
    return *m_swapchain;
}

uint32_t DefaultEngineImpl::GetViewCount() const
{
    return 1;
}

void DefaultEngineImpl::RunFrameLoop(const std::function<bool()>& tick)
{
    RunWindowFrameLoop(tick);
}

void DefaultEngineImpl::ResetInstance()
{
    m_instance = prev::core::instance::InstanceFactory{}.Create(m_config.appName, m_config.validation, m_config.renderBackend);
}

void DefaultEngineImpl::ResetDevice()
{
    prev::core::device::Adapters adapters{ m_instance->GetHandle() };
    adapters.Print();

    const GfxSurface surface = m_surface ? static_cast<GfxSurface>(*m_surface) : nullptr;
    const auto adapter{ adapters.Find(surface, m_config.gpuIndex) };
    if (!adapter) {
        throw std::runtime_error("No suitable GPU adapter found");
    }

    // Enable precise occlusion queries only when the selected adapter reports support.
    uint32_t extensionCount{ 0 };
    gfxAdapterEnumerateExtensions(*adapter, &extensionCount, nullptr);
    std::vector<const char*> availableExtensions(extensionCount);
    gfxAdapterEnumerateExtensions(*adapter, &extensionCount, availableExtensions.data());

    auto isExtensionAvailable = [&](const std::string& ext) {
        return std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](const char* e) {
            return e != nullptr && std::string(e) == ext;
        }) != availableExtensions.end();
    };

    // Build extension list based on what's actually supported by the adapter
    std::vector<std::string> extensions;

    if (!m_config.headless) {
        extensions.push_back(GFX_DEVICE_EXTENSION_SWAPCHAIN);
    }

    if (isExtensionAvailable(GFX_DEVICE_EXTENSION_ANISOTROPIC_FILTERING)) {
        extensions.push_back(GFX_DEVICE_EXTENSION_ANISOTROPIC_FILTERING);
    }
    if (isExtensionAvailable(GFX_DEVICE_EXTENSION_NON_SOLID_FILL)) {
        extensions.push_back(GFX_DEVICE_EXTENSION_NON_SOLID_FILL);
    }
    if (isExtensionAvailable(GFX_DEVICE_EXTENSION_OCCLUSION_QUERY_PRECISE)) {
        extensions.push_back(GFX_DEVICE_EXTENSION_OCCLUSION_QUERY_PRECISE);
    }

    m_device = prev::core::device::DeviceFactory{}.Create(*adapter, extensions);
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();
}

void DefaultEngineImpl::ResetRenderPass()
{
    const GfxFormat colorFormat = m_surface
        ? m_surface->GetPreferredFormat(m_device->GetAdapter(), m_config.colorManaged)
        : (m_config.colorManaged ? GFX_FORMAT_B8G8R8A8_UNORM_SRGB : GFX_FORMAT_B8G8R8A8_UNORM);

    if (m_config.colorManaged && !prev::core::format::IsSrgb(colorFormat)) {
        LOGW("colorManaged requested but the present surface exposes no sRGB format; falling back to gamma passthrough");
        m_config.colorManaged = false;
    }

    const GfxFormat depthFormat = GFX_FORMAT_DEPTH32_FLOAT;
    const GfxSampleCount sampleCount = static_cast<GfxSampleCount>(m_config.samplesCount);

    if (m_config.samplesCount > 1) {
        m_renderPass = CreateDefaultMultisampledRenderPass(*m_device, colorFormat, depthFormat, sampleCount, GetViewCount(), true, false);
    } else {
        m_renderPass = CreateDefaultRenderPass(*m_device, colorFormat, depthFormat, GetViewCount(), true, false);
    }
    LOGI("GFX render pass created");
}

void DefaultEngineImpl::ResetSwapchain()
{
    m_swapchain = CreateWindowSwapchain(GetViewCount());
    m_swapchain->Print();
}

void DefaultEngineImpl::ReleaseSwapchain()
{
    m_swapchain.reset();
}
} // namespace prev::core::engine::impl