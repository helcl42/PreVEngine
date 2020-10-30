#include "Scene.h"

namespace prev::scene {
Scene::Scene(const std::shared_ptr<SceneConfig>& sceneConfig, const std::shared_ptr<prev::core::device::Device>& device, VkSurfaceKHR surface)
    : m_config(sceneConfig)
    , m_device(device)
    , m_surface(surface)
{
}

void Scene::Init()
{
    InitQueues();
    InitRenderPass();
    InitAllocator();
    InitSwapchain();

    AllocatorProvider::Instance().SetAllocator(m_allocator);
    ComputeProvider::Instance().Set(m_computeQueue, m_computeAllocator);
}

void Scene::InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode)
{
    prev::scene::graph::GraphTraversal::Instance().SetRootNode(rootNode);
    m_rootNode = rootNode;
    m_rootNode->Init();
}

void Scene::InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> >& rootRenderer)
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
        prev::render::RenderContext renderContext{ frameBuffer, commandBuffer, frameInFlightIndex, m_swapchain->GetExtent() };

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
    prev::scene::graph::GraphTraversal::Instance().SetRootNode(nullptr);
}

void Scene::ShutDown()
{
    ComputeProvider::Instance().Reset();
    AllocatorProvider::Instance().SetAllocator(nullptr);
}

std::shared_ptr<prev::core::device::Device> Scene::GetDevice() const
{
    return m_device;
}

std::shared_ptr<prev::render::Swapchain> Scene::GetSwapchain() const
{
    return m_swapchain;
}

std::shared_ptr<prev::render::pass::RenderPass> Scene::GetRenderPass() const
{
    return m_renderPass;
}

std::shared_ptr<prev::core::memory::Allocator> Scene::GetAllocator() const
{
    return m_allocator;
}

std::shared_ptr<prev::scene::graph::ISceneNode> Scene::GetRootNode() const
{
    return m_rootNode;
}

std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> > Scene::GetRootRenderer() const
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

void Scene::InitQueues()
{
    m_presentQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, m_surface); // compute + graphics + present queue
    m_graphicsQueue = m_presentQueue; // they might be the same or not
    m_computeQueue = m_presentQueue;
    if (!m_presentQueue) {
        m_presentQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT, m_surface); // graphics + present-queue
        m_graphicsQueue = m_presentQueue;
        if (!m_presentQueue) {
            m_presentQueue = m_device->AddQueue(0, m_surface); // create present-queue
            m_graphicsQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT); // create graphics queue
        }
        m_computeQueue = m_device->AddQueue(VK_QUEUE_COMPUTE_BIT);
    }
}

void Scene::InitRenderPass()
{
    const auto colorFormat = m_device->GetGPU().FindSurfaceFormat(m_surface);
    const auto depthFormat = m_device->GetGPU().FindDepthFormat();

    m_renderPass = std::make_shared<prev::render::pass::RenderPass>(*m_device);
    m_renderPass->AddColorAttachment(colorFormat, { 0.5f, 0.5f, 0.5f, 1.0f });
    m_renderPass->AddDepthAttachment(depthFormat);
    m_renderPass->AddSubpass({ 0, 1 });
}

void Scene::InitAllocator()
{
    m_allocator = std::make_shared<prev::core::memory::Allocator>(*m_graphicsQueue); // Create "Vulkan Memory Aloocator"
    printf("Allocator created\n");

    if (m_computeQueue != nullptr && m_graphicsQueue != m_computeQueue) {
        m_computeAllocator = std::make_shared<prev::core::memory::Allocator>(*m_computeQueue);
        printf("Allocator Compute created\n");
    } else {
        m_computeAllocator = m_allocator;
    }
}

void Scene::InitSwapchain()
{
    m_swapchain = std::make_shared<prev::render::Swapchain>(*m_presentQueue, *m_graphicsQueue, *m_renderPass, *m_allocator);
#if defined(__ANDROID__)
    m_swapchain->SetPresentMode(m_config->VSync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#else
    m_swapchain->SetPresentMode(m_config->VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
    m_swapchain->SetImageCount(m_config->framesInFlight);
    m_swapchain->Print();
}
} // namespace prev::scene