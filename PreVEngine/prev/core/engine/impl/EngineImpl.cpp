#include "EngineImpl.h"

#include "../../../common/Logger.h"
#include "../../../render/pass/RenderPassBuilder.h"
#include "../../../util/MathUtils.h"
#include "../../../window/Window.h"

#include <stdexcept>
#include <vector>

namespace prev::core::engine::impl {
EngineImpl::EngineImpl(const Config& config)
    : m_config{ config }
{
#if defined(GFX_HEADLESS_BUILD)
    if (!m_config.headless) {
        LOGW("BUILD_HEADLESS: no windowing system compiled in - forcing headless mode (windowed rendering unavailable)");
    }
    m_config.headless = true;
#endif
}

EngineImpl::~EngineImpl()
{
}

prev::scene::IScene& EngineImpl::GetScene() const
{
    return *m_scene;
}

prev::render::IRootRenderer& EngineImpl::GetRootRenderer() const
{
    return *m_rootRenderer;
}

prev::render::swapchain::ISwapchain& EngineImpl::GetSwapchain() const
{
    return *m_swapchain;
}

prev::render::pass::RenderPass& EngineImpl::GetRenderPass() const
{
    return *m_renderPass;
}

prev::core::device::Device& EngineImpl::GetDevice() const
{
    return *m_device;
}

prev::core::DeferredResourceDestroyer& EngineImpl::GetDeferredResourceDestroyer()
{
    return m_device->GetDeferredResourceDestroyer();
}

prev::core::DeferredResourceUploader& EngineImpl::GetDeferredResourceUploader()
{
    return m_device->GetDeferredResourceUploader();
}

const Config& EngineImpl::GetConfig() const
{
    return m_config;
}

void EngineImpl::InitScene(std::unique_ptr<prev::scene::IScene> scene)
{
    m_scene = std::move(scene);
    m_scene->Init();
}

void EngineImpl::InitRenderer(std::unique_ptr<prev::render::IRootRenderer> rootRenderer)
{
    m_rootRenderer = std::move(rootRenderer);
    m_rootRenderer->Init();
}

void EngineImpl::BeginMainLoop()
{
    ResetTiming();
}

void EngineImpl::EndMainLoop()
{
}

bool EngineImpl::IsFocused() const
{
    return m_window->HasFocus();
}

void EngineImpl::operator()(const prev::window::WindowChangeEvent& windowChangeEvent)
{
    m_device->WaitIdle();

    m_swapchain.reset();
    m_surface.reset();
    m_device->GetDeferredResourceDestroyer().RetireAll();
    ResetSurface();
    ResetSwapchain();
}

void EngineImpl::operator()(const prev::window::WindowResizeEvent& resizeEvent)
{
    m_device->WaitIdle();
    m_swapchain.reset();
    m_device->GetDeferredResourceDestroyer().RetireAll();
    ResetSwapchain();
}

void EngineImpl::ResetTiming()
{
    m_clock = std::make_unique<prev::util::Clock<float>>();
    m_fpsCounter = std::make_unique<prev::util::FPSCounter>();
}

void EngineImpl::ResetWindow()
{
    prev::window::WindowCreateInfo windowCreateInfo{};
    windowCreateInfo.headless = m_config.headless;
    windowCreateInfo.title = m_config.appName;
    windowCreateInfo.fullScreen = m_config.fullScreen;
    windowCreateInfo.left = m_config.windowPosition.x;
    windowCreateInfo.top = m_config.windowPosition.y;
    windowCreateInfo.width = m_config.windowSize.x;
    windowCreateInfo.height = m_config.windowSize.y;

    m_window = std::make_unique<prev::window::Window>(windowCreateInfo);
}

void EngineImpl::ResetSurface()
{
    if (m_config.headless) {
        return;
    }
    m_surface = std::make_unique<prev::render::surface::Surface>(*m_instance, m_window->GetNativeWindowHandle());
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultMultisampledRenderPass(const prev::core::device::Device& device, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount, uint32_t viewCount, bool storeColor, bool storeDepth)
{
    return prev::render::pass::RenderPassBuilder(device)
        .AddColorAttachment(colorFormat, sampleCount, {}, GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT, GFX_LOAD_OP_CLEAR, GFX_STORE_OP_DONT_CARE)
        .AddColorAttachment(colorFormat, GFX_SAMPLE_COUNT_1, {}, GFX_TEXTURE_LAYOUT_PRESENT_SRC, GFX_LOAD_OP_CLEAR, storeColor ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE, true)
        .AddDepthAttachment(depthFormat, sampleCount, 1.0f, 0, GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT, GFX_LOAD_OP_CLEAR, GFX_STORE_OP_DONT_CARE)
        .AddDepthAttachment(depthFormat, GFX_SAMPLE_COUNT_1, 1.0f, 0, GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT, GFX_LOAD_OP_CLEAR, storeDepth ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE, true)
        .SetViewCount(viewCount)
        .Build();
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultRenderPass(const prev::core::device::Device& device, GfxFormat colorFormat, GfxFormat depthFormat, uint32_t viewCount, bool storeColor, bool storeDepth)
{
    return prev::render::pass::RenderPassBuilder(device)
        .AddColorAttachment(colorFormat, GFX_SAMPLE_COUNT_1, {}, GFX_TEXTURE_LAYOUT_PRESENT_SRC, GFX_LOAD_OP_CLEAR, storeColor ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE)
        .AddDepthAttachment(depthFormat, GFX_SAMPLE_COUNT_1, 1.0f, 0, GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT, GFX_LOAD_OP_CLEAR, storeDepth ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE)
        .SetViewCount(viewCount)
        .Build();
}

void EngineImpl::UpdateFps()
{
    if (m_fpsCounter->Tick()) {
        LOGI("FPS %f", m_fpsCounter->GetAverageFPS());
    }
}
} // namespace prev::core::engine::impl
