#include "DefaultEngineImpl.h"

#include "../../device/Device.h"
#include "../../device/DeviceFactory.h"
#include "../../device/PhysicalDevices.h"
#include "../../device/Queue.h"
#include "../../instance/InstanceFactory.h"

#include "../../../common/Logger.h"
#include "../../../render/swapchain/SwapchainFactory.h"

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

    m_rootRenderer = nullptr;
    m_scene = nullptr;

    m_swapchain = nullptr;
    m_renderPass = nullptr;
    m_surface = nullptr; // Surface destructor calls gfxSurfaceDestroy
    m_device = nullptr; // Device destructor calls gfxDeviceDestroy
    m_instance = nullptr; // Instance destructor calls gfxInstanceDestroy
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
    m_instance = prev::core::instance::InstanceFactory{}.Create(m_config.appName, m_config.validation);
}

void DefaultEngineImpl::ResetDevice()
{
    prev::core::device::PhysicalDevices physicalDevices{ m_instance->GetHandle() };
    physicalDevices.Print();

    const auto gpu{ physicalDevices.Find(*m_surface, m_config.gpuIndex) };
    if (!gpu) {
        throw std::runtime_error("No suitable GPU adapter found");
    }

    const std::vector<std::string> extensions{
        GFX_DEVICE_EXTENSION_SWAPCHAIN,
        GFX_DEVICE_EXTENSION_ANISOTROPIC_FILTERING,
        // GFX_DEVICE_EXTENSION_MULTIVIEW
    };

    m_device = prev::core::device::DeviceFactory{}.Create(*gpu, extensions);
    if (!m_device) {
        throw std::runtime_error("Could not create logical device");
    }
    m_device->Print();
}

void DefaultEngineImpl::ResetRenderPass()
{
    const auto formats{ m_surface->GetSupportedFormats(m_device->GetGPU()) };
    if (formats.empty()) {
        throw std::runtime_error("No supported surface formats found for the GPU");
    }
    const GfxFormat colorFormat = formats[0];
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
    const GfxExtent2D extent{ static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height) };
    const GfxSurface surface = m_surface ? static_cast<GfxSurface>(*m_surface) : nullptr;

    GfxPresentMode presentMode = GFX_PRESENT_MODE_FIFO;
    uint32_t imageCount = m_config.swapchainFrameCount;

    if (surface) {
#if defined(__ANDROID__)
        const GfxPresentMode preferred = m_config.VSync ? GFX_PRESENT_MODE_FIFO : GFX_PRESENT_MODE_MAILBOX;
#else
        const GfxPresentMode preferred = m_config.VSync ? GFX_PRESENT_MODE_FIFO : GFX_PRESENT_MODE_IMMEDIATE;
#endif
        const auto presentModes{ m_surface->GetSupportedPresentModes(m_device->GetGPU()) };
        if (!presentModes.empty()) {
            presentMode = presentModes[0];
            for (const auto& m : presentModes) {
                if (m == preferred) {
                    presentMode = m;
                    break;
                }
            }
        }

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