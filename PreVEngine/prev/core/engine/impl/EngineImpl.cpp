#include "EngineImpl.h"

#include "../../../render/pass/RenderPassBuilder.h"
#include "../../../window/Window.h"

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

prev::render::ISwapchain& EngineImpl::GetSwapchain() const
{
    return *m_swapchain;
}

prev::render::pass::RenderPass& EngineImpl::GetRenderPass() const
{
    return *m_renderPass;
}

prev::core::memory::Allocator& EngineImpl::GetAllocator() const
{
    return *m_allocator;
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

    m_swapchain = nullptr; // swapchain needs to be destroyed before surface
    ResetSurface();
    ResetSwapchain();
}

void EngineImpl::operator()(const prev::window::WindowResizeEvent& resizeEvent)
{
    m_swapchain->UpdateExtent();
}

void EngineImpl::ResetTiming()
{
    m_clock = std::make_unique<prev::util::Clock<float>>();
    m_fpsCounter = std::make_unique<prev::util::FPSCounter>();
}

void EngineImpl::ResetWindow()
{
    prev::window::WindowCreateInfo windowCreateInfo{};
    windowCreateInfo.title = m_config.appName;
    windowCreateInfo.fullScreen = m_config.fullScreen;
    windowCreateInfo.left = m_config.windowPosition.x;
    windowCreateInfo.top = m_config.windowPosition.y;
    windowCreateInfo.width = m_config.windowSize.x;
    windowCreateInfo.height = m_config.windowSize.y;

    m_window = std::make_unique<prev::window::Window>(*m_instance, windowCreateInfo);
}

void EngineImpl::ResetSurface()
{
    m_surface = m_window->ResetSurface();
}

void EngineImpl::ResetAllocator()
{
    // TODO
    //  -> Allocator uses only graphics queue for it's internal commands
    //     - make queue type as a parameter or even better decouple it from queue completely
    m_allocator = std::make_unique<prev::core::memory::Allocator>(*m_instance, *m_device, m_device->GetQueue(device::QueueType::GRAPHICS)); // Create "Vulkan Memory Allocator"
    LOGI("Allocator created");
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultMultisampledRenderPass(const prev::core::device::Device& device, const VkSurfaceKHR surface, const VkSampleCountFlagBits sampleCount, const uint32_t viewCount, const bool storeColor, const bool storeDepth)
{
    const auto colorFormat{ device.GetGPU().FindSurfaceFormat(surface) };
    const auto depthFormat{ device.GetGPU().FindDepthFormat() };

    const VkClearColorValue clearColor{ { 0.5f, 0.5f, 0.5f, 1.0f } };
    const VkClearDepthStencilValue clearDepth{ MAX_DEPTH, 0 };

    std::vector<VkSubpassDependency> dependencies(2);
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    return prev::render::pass::RenderPassBuilder{ device, viewCount }
        .AddColorAttachment(colorFormat, sampleCount, clearColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE) // color buffer, multisampled
        .AddDepthAttachment(depthFormat, sampleCount, clearDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE) // depth buffer, multisampled
        .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ATTACHMENT_LOAD_OP_CLEAR, storeColor ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            true) // color buffer, resolve buffer
        .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, clearDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, storeDepth ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            true) // depth buffer, resolve buffer
        .AddSubpass({ 0, 1 }, { 2, 3 }) // resolve ref will be at index 2 & 3
        .AddSubpassDependencies(dependencies)
        .Build();
}

std::unique_ptr<prev::render::pass::RenderPass> EngineImpl::CreateDefaultRenderPass(const prev::core::device::Device& device, const VkSurfaceKHR surface, const uint32_t viewCount, const bool storeColor, const bool storeDepth)
{
    const auto colorFormat{ device.GetGPU().FindSurfaceFormat(surface) };
    const auto depthFormat{ device.GetGPU().FindDepthFormat() };

    const VkClearColorValue clearColor{ { 0.5f, 0.5f, 0.5f, 1.0f } };
    const VkClearDepthStencilValue clearDepth{ MAX_DEPTH, 0 };

    std::vector<VkSubpassDependency> dependencies(2);
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_NONE;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_NONE;
    dependencies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    return prev::render::pass::RenderPassBuilder{ device, viewCount }
        .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ATTACHMENT_LOAD_OP_CLEAR, storeColor ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE)
        .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, clearDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, storeDepth ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE)
        .AddSubpass({ 0, 1 })
        .AddSubpassDependencies(dependencies)
        .Build();
}

void EngineImpl::UpdateFps()
{
    if (m_fpsCounter->Tick()) {
        LOGI("FPS %f", m_fpsCounter->GetAverageFPS());
    }
}
} // namespace prev::core::engine::impl