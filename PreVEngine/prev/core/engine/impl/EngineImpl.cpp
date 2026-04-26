#include "EngineImpl.h"

#include "../../../common/Logger.h"
#include "../../../util/MathUtils.h"
#include "../../../window/Window.h"

#include <stdexcept>
#include <vector>

namespace prev::core::engine::impl {
EngineImpl::EngineImpl(const Config& config)
    : m_config{ config }
{
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

    m_swapchain = nullptr;
    m_surface = nullptr;
    ResetSurface();
    ResetSwapchain();
}

void EngineImpl::operator()(const prev::window::WindowResizeEvent& resizeEvent)
{
    m_device->WaitIdle();
    m_swapchain = nullptr;
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
    m_surface = std::make_unique<prev::render::surface::Surface>(*m_instance, m_window->GetNativeWindowHandle());
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultMultisampledRenderPass(const prev::core::device::Device& device, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount, uint32_t viewCount, bool storeColor, bool storeDepth)
{
    GfxRenderPassColorAttachmentTarget resolveTarget{};
    resolveTarget.format = colorFormat;
    resolveTarget.sampleCount = GFX_SAMPLE_COUNT_1;
    resolveTarget.ops = { GFX_LOAD_OP_CLEAR, storeColor ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE };
    resolveTarget.finalLayout = GFX_TEXTURE_LAYOUT_PRESENT_SRC;

    GfxRenderPassColorAttachment colorAttachment{};
    colorAttachment.target.format = colorFormat;
    colorAttachment.target.sampleCount = sampleCount;
    colorAttachment.target.ops = { GFX_LOAD_OP_CLEAR, GFX_STORE_OP_DONT_CARE };
    colorAttachment.target.finalLayout = GFX_TEXTURE_LAYOUT_COLOR_ATTACHMENT;
    colorAttachment.resolveTarget = &resolveTarget;

    GfxRenderPassDepthStencilAttachmentTarget depthResolveTarget{};
    depthResolveTarget.format = depthFormat;
    depthResolveTarget.sampleCount = GFX_SAMPLE_COUNT_1;
    depthResolveTarget.depthOps = { GFX_LOAD_OP_CLEAR, storeDepth ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE };
    depthResolveTarget.stencilOps = { GFX_LOAD_OP_DONT_CARE, GFX_STORE_OP_DONT_CARE };
    depthResolveTarget.finalLayout = GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

    GfxRenderPassDepthStencilAttachment depthAttachment{};
    depthAttachment.target.format = depthFormat;
    depthAttachment.target.sampleCount = sampleCount;
    depthAttachment.target.depthOps = { GFX_LOAD_OP_CLEAR, GFX_STORE_OP_DONT_CARE };
    depthAttachment.target.stencilOps = { GFX_LOAD_OP_DONT_CARE, GFX_STORE_OP_DONT_CARE };
    depthAttachment.target.finalLayout = GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
    depthAttachment.resolveTarget = &depthResolveTarget;

    GfxRenderPassMultiviewDescriptor multiviewDesc{};
    uint32_t viewMask = prev::util::math::SetBits<uint32_t>(viewCount);
    uint32_t correlationMask = viewMask;
    multiviewDesc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_DESCRIPTOR;
    multiviewDesc.pNext = nullptr;
    multiviewDesc.viewMask = viewMask;
    multiviewDesc.correlationMasks = &correlationMask;
    multiviewDesc.correlationMaskCount = 1;

    GfxRenderPassDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_DESCRIPTOR;
    desc.pNext = (viewCount > 1) ? &multiviewDesc : nullptr;
    desc.label = "Default MSAA RenderPass";
    desc.colorAttachments = &colorAttachment;
    desc.colorAttachmentCount = 1;
    desc.depthStencilAttachment = &depthAttachment;

    GfxRenderPass renderPass{};
    if (gfxDeviceCreateRenderPass(device, &desc, &renderPass) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create MSAA render pass");
    }
    return std::make_unique<prev::render::pass::RenderPass>(device, renderPass, colorFormat, depthFormat, sampleCount);
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultRenderPass(const prev::core::device::Device& device, GfxFormat colorFormat, GfxFormat depthFormat, uint32_t viewCount, bool storeColor, bool storeDepth)
{
    GfxRenderPassColorAttachment colorAttachment{};
    colorAttachment.target.format = colorFormat;
    colorAttachment.target.sampleCount = GFX_SAMPLE_COUNT_1;
    colorAttachment.target.ops = { GFX_LOAD_OP_CLEAR, storeColor ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE };
    colorAttachment.target.finalLayout = GFX_TEXTURE_LAYOUT_PRESENT_SRC;
    colorAttachment.resolveTarget = nullptr;

    GfxRenderPassDepthStencilAttachment depthAttachment{};
    depthAttachment.target.format = depthFormat;
    depthAttachment.target.sampleCount = GFX_SAMPLE_COUNT_1;
    depthAttachment.target.depthOps = { GFX_LOAD_OP_CLEAR, storeDepth ? GFX_STORE_OP_STORE : GFX_STORE_OP_DONT_CARE };
    depthAttachment.target.stencilOps = { GFX_LOAD_OP_DONT_CARE, GFX_STORE_OP_DONT_CARE };
    depthAttachment.target.finalLayout = GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
    depthAttachment.resolveTarget = nullptr;

    GfxRenderPassMultiviewDescriptor multiviewDesc{};
    uint32_t viewMask = prev::util::math::SetBits<uint32_t>(viewCount);
    uint32_t correlationMask = viewMask;
    multiviewDesc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_DESCRIPTOR;
    multiviewDesc.pNext = nullptr;
    multiviewDesc.viewMask = viewMask;
    multiviewDesc.correlationMasks = &correlationMask;
    multiviewDesc.correlationMaskCount = 1;

    GfxRenderPassDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_DESCRIPTOR;
    desc.pNext = (viewCount > 1) ? &multiviewDesc : nullptr;
    desc.label = "Default RenderPass";
    desc.colorAttachments = &colorAttachment;
    desc.colorAttachmentCount = 1;
    desc.depthStencilAttachment = &depthAttachment;

    GfxRenderPass renderPass{};
    if (gfxDeviceCreateRenderPass(device, &desc, &renderPass) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create render pass");
    }
    return std::make_unique<prev::render::pass::RenderPass>(device, renderPass, colorFormat, depthFormat, GFX_SAMPLE_COUNT_1);
}

void EngineImpl::UpdateFps()
{
    if (m_fpsCounter->Tick()) {
        LOGI("FPS %f", m_fpsCounter->GetAverageFPS());
    }
}
} // namespace prev::core::engine::impl
