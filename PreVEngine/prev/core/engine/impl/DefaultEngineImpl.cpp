#include "DefaultEngineImpl.h"

#include "../../device/Device.h"
#include "../../device/DeviceFactory.h"
#include "../../device/PhysicalDevices.h"
#include "../../device/Queue.h"
#include "../../instance/InstanceFactory.h"

#include "../../../common/Logger.h"
#include "../../../render/swapchain/SwapchainFactory.h"

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
    m_instance = prev::core::instance::InstanceFactory{}.Create(m_config.appName, m_config.validation, m_config.renderBackend);
}

void DefaultEngineImpl::ResetDevice()
{
    prev::core::device::PhysicalDevices physicalDevices{ m_instance->GetHandle() };
    physicalDevices.Print();

    const GfxSurface surface = m_surface ? static_cast<GfxSurface>(*m_surface) : nullptr;
    const auto gpu{ physicalDevices.Find(surface, m_config.gpuIndex) };
    if (!gpu) {
        throw std::runtime_error("No suitable GPU adapter found");
    }

    // Enable precise occlusion queries only when the selected adapter reports support.
    uint32_t extensionCount{ 0 };
    gfxAdapterEnumerateExtensions(*gpu, &extensionCount, nullptr);
    std::vector<const char*> availableExtensions(extensionCount);
    gfxAdapterEnumerateExtensions(*gpu, &extensionCount, availableExtensions.data());

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

    m_device = prev::core::device::DeviceFactory{}.Create(*gpu, extensions);
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();
}

void DefaultEngineImpl::ResetRenderPass()
{
    const GfxFormat colorFormat = m_surface ? m_surface->GetPreferredFormat(m_device->GetGPU()) : GFX_FORMAT_B8G8R8A8_UNORM;
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
    const auto size{ m_window->GetSize() };
    const GfxExtent2D extent{ size.width, size.height };
    const GfxSurface surface = m_surface ? static_cast<GfxSurface>(*m_surface) : nullptr;

    GfxPresentMode presentMode = GFX_PRESENT_MODE_FIFO;
    uint32_t imageCount = m_config.swapchainFrameCount;

    if (surface) {
        const GfxPresentMode preferred = m_config.VSync ? GFX_PRESENT_MODE_FIFO : GFX_PRESENT_MODE_IMMEDIATE;
        presentMode = m_surface->GetPreferredPresentMode(m_device->GetGPU(), preferred);

        const auto surfaceInfo{ m_surface->GetInfo(m_device->GetGPU()) };
        imageCount = std::max(surfaceInfo.minImageCount,
            std::min(m_config.swapchainFrameCount, surfaceInfo.maxImageCount > 0 ? surfaceInfo.maxImageCount : m_config.swapchainFrameCount));
    }

    m_swapchain = prev::render::swapchain::SwapchainFactory{}.Create(
        *m_device,
        *m_renderPass,
        surface,
        extent,
        presentMode,
        imageCount,
        GetViewCount(),
        m_config.maxFramesInFlight);
    m_swapchain->Print();
}
} // namespace prev::core::engine::impl