#include "Scene.h"
#include "../render/pass/RenderPassBuilder.h"
#include "../util/VkUtils.h"

namespace prev::scene {
Scene::Scene(const SceneConfig& sceneConfig, const std::shared_ptr<prev::core::device::Device>& device, const std::shared_ptr<prev::core::memory::Allocator>& allocator, VkSurfaceKHR surface)
    : m_config(sceneConfig)
    , m_device(device)
    , m_allocator(allocator)
    , m_surface(surface)
{
}

void Scene::Init()
{
    InitRenderPass();
    InitSwapchain();
}

void Scene::InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode)
{
    prev::scene::graph::GraphTraversal::Instance().SetRootNode(rootNode);
    m_rootNode = rootNode;
    m_rootNode->Init();
}

void Scene::InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>>& rootRenderer)
{
    m_rootRenderer = rootRenderer;
    m_rootRenderer->Init();
}

void Scene::Update(float deltaTime)
{
    m_rootNode->Update(deltaTime);
}

void Scene::Render()
{
    VkFramebuffer frameBuffer;
    VkCommandBuffer commandBuffer;
    uint32_t frameInFlightIndex;
    if (m_swapchain->BeginFrame(frameBuffer, commandBuffer, frameInFlightIndex)) {
        const prev::render::RenderContext renderContext{ frameBuffer, commandBuffer, frameInFlightIndex, { { 0, 0 }, m_swapchain->GetExtent() } };

        m_rootRenderer->BeforeRender(renderContext);
        m_rootRenderer->PreRender(renderContext);
        m_rootRenderer->Render(renderContext, m_rootNode);
        m_rootRenderer->PostRender(renderContext);
        m_rootRenderer->AfterRender(renderContext);

        m_swapchain->EndFrame();
    }
}

void Scene::ShutDownRenderer()
{
    m_rootRenderer->ShutDown();
}

void Scene::ShutDownSceneGraph()
{
    m_rootNode->ShutDown();
    m_rootNode = nullptr;
    prev::scene::graph::GraphTraversal::Instance().SetRootNode(nullptr);
}

void Scene::ShutDown()
{
    m_swapchain = nullptr;
    m_renderPass = nullptr;
}

std::shared_ptr<prev::render::Swapchain> Scene::GetSwapchain() const
{
    return m_swapchain;
}

std::shared_ptr<prev::render::pass::RenderPass> Scene::GetRenderPass() const
{
    return m_renderPass;
}

std::shared_ptr<prev::scene::graph::ISceneNode> Scene::GetRootNode() const
{
    return m_rootNode;
}

std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>> Scene::GetRootRenderer() const
{
    return m_rootRenderer;
}

void Scene::operator()(const prev::window::WindowResizeEvent& resizeEvent)
{
    m_swapchain->UpdateExtent();
}

void Scene::operator()(const prev::window::SurfaceChanged& surfaceChangedEvent)
{
    m_surface = surfaceChangedEvent.surface;
    InitSwapchain();
    m_swapchain->UpdateExtent();
}

void Scene::InitRenderPass()
{
    prev::render::pass::RenderPassBuilder renderPassBuilder{ *m_device };

    const auto colorFormat{ m_device->GetGPU()->FindSurfaceFormat(m_surface) };
    const auto depthFormat{ m_device->GetGPU()->FindDepthFormat() };
    const VkClearColorValue clearColor{ { 0.5f, 0.5f, 0.5f, 1.0f } };
    const VkSampleCountFlagBits sampleCount{ prev::util::vk::GetSampleCountBit(m_config.samplesCount) };

    if (sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        std::vector<VkSubpassDependency> dependencies{ 2 };
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        m_renderPass = renderPassBuilder
                           .AddColorAttachment(colorFormat, sampleCount, clearColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) // color buffer, multisampled
                           .AddDepthAttachment(depthFormat, sampleCount, { 1.0f, 0 }) // depth buffer, multisampled
                           .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, true) // color buffer, resolve buffer
                           .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT, { 1.0f, 0 }, true) // depth buffer, resolve buffer
                           .AddSubpass({ 0, 1 }, { 2, 3 }) // resolve ref will be at index 2 & 3
                           .AddSubpassDependencies(dependencies)
                           .Build();
    } else {
        m_renderPass = renderPassBuilder
                           .AddColorAttachment(colorFormat, VK_SAMPLE_COUNT_1_BIT, clearColor)
                           .AddDepthAttachment(depthFormat, VK_SAMPLE_COUNT_1_BIT)
                           .AddSubpass({ 0, 1 })
                           .Build();
    }
}

void Scene::InitSwapchain()
{
    m_swapchain = std::make_shared<prev::render::Swapchain>(*m_device, *m_allocator, *m_renderPass, m_surface, prev::util::vk::GetSampleCountBit(m_config.samplesCount));
#if defined(__ANDROID__)
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#else
    m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
    m_swapchain->SetImageCount(m_config.framesInFlight);
    m_swapchain->Print();
}
} // namespace prev::scene