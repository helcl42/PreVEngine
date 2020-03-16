#ifndef __SCENE_H__
#define __SCENE_H__

#include "Buffers.h"
#include "Devices.h"
#include "Events.h"
#include "RenderPass.h"
#include "SceneGraph.h"
#include "Swapchain.h"
#include "Window.h"

namespace PreVEngine {
struct SceneConfig {
    // swapchain
    bool VSync = true;

    uint32_t framesInFlight = 3;
};

template <typename NodeFlagsType>
class Scene;

class AllocatorProvider final : public Singleton<AllocatorProvider> {
private:
    friend class Singleton<AllocatorProvider>;

    template <typename NodeFlagsType>
    friend class Scene;

private:
    std::shared_ptr<Allocator> m_allocator;

private:
    AllocatorProvider() = default;

public:
    ~AllocatorProvider() = default;

private:
    void SetAllocator(const std::shared_ptr<Allocator>& device)
    {
        m_allocator = device;
    }

public:
    std::shared_ptr<Allocator> GetAllocator() const
    {
        return m_allocator;
    }
};

template <typename NodeFlagsType>
class IScene {
public:
    virtual void Init() = 0;

    virtual void InitSceneGraph() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Render() = 0;

    virtual void ShutDownSceneGraph() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<ISceneNode<NodeFlagsType> > GetRootNode() = 0;

    virtual void SetSceneRoot(const std::shared_ptr<ISceneNode<NodeFlagsType> >& root) = 0;

    virtual std::shared_ptr<Device> GetDevice() = 0;

    virtual std::shared_ptr<Swapchain> GetSwapchain() = 0;

    virtual std::shared_ptr<RenderPass> GetRenderPass() = 0;

    virtual std::shared_ptr<Allocator> GetAllocator() = 0;

public:
    virtual ~IScene() = default;
};

template <typename NodeFlagsType>
class Scene : public IScene<NodeFlagsType> {
private:
    EventHandler<Scene, WindowResizeEvent> m_windowResizeEvent{ *this };

    EventHandler<Scene, SurfaceChanged> m_surfaceChangedEvent{ *this };

protected:
    std::shared_ptr<SceneConfig> m_config;

    std::shared_ptr<Device> m_device;

    Queue* m_presentQueue;

    Queue* m_graphicsQueue;

    VkSurfaceKHR m_surface;

protected:
    std::shared_ptr<Allocator> m_allocator;

    std::shared_ptr<RenderPass> m_renderPass;

    std::shared_ptr<Swapchain> m_swapchain;

    std::shared_ptr<ISceneNode<NodeFlagsType> > m_rootNode;

public:
    Scene(const std::shared_ptr<SceneConfig>& sceneConfig, const std::shared_ptr<Device>& device, VkSurfaceKHR surface)
        : m_config(sceneConfig)
        , m_device(device)
        , m_surface(surface)
    {
    }

    virtual ~Scene() = default;

private:
    void InitQueues()
    {
        m_presentQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT, m_surface); // graphics + present-queue
        m_graphicsQueue = m_presentQueue; // they might be the same or not
        if (!m_presentQueue) {
            m_presentQueue = m_device->AddQueue(0, m_surface); // create present-queue
            m_graphicsQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT); // create graphics queue
        }
    }

    void InitRenderPass()
    {
        VkFormat colorFormat = m_device->GetGPU().FindSurfaceFormat(m_surface);
        VkFormat depthFormat = m_device->GetGPU().FindDepthFormat();

        m_renderPass = std::make_shared<RenderPass>(*m_device);
        m_renderPass->AddColorAttachment(colorFormat, { 0.5f, 0.5f, 0.5f, 1.0f });
        m_renderPass->AddDepthAttachment(depthFormat);
        m_renderPass->AddSubpass({ 0, 1 });
    }

    void InitAllocator()
    {
        m_allocator = std::make_shared<Allocator>(*m_graphicsQueue); // Create "Vulkan Memory Aloocator"
        printf("Allocator created\n");
    }

    void InitSwapchain()
    {
        m_swapchain = std::make_shared<Swapchain>(*m_allocator, *m_renderPass, m_graphicsQueue, m_graphicsQueue);
#if defined(__ANDROID__)
        m_swapchain->SetPresentMode(m_config->VSync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#else
        m_swapchain->SetPresentMode(m_config->VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
#endif
        m_swapchain->SetImageCount(m_config->framesInFlight);
        m_swapchain->Print();
    }

public:
    void Init() override
    {
        InitQueues();
        InitRenderPass();
        InitAllocator();
        InitSwapchain();

        AllocatorProvider::Instance().SetAllocator(m_allocator);
    }

    void InitSceneGraph() override
    {
        m_rootNode->Init();
    }

    void Update(float deltaTime) override
    {
        m_rootNode->Update(deltaTime);
    }

    void Render() override
    {
        VkFramebuffer fraemBuffer;
        VkCommandBuffer commandBuffer;
        uint32_t frameInFlightIndex;
        if (m_swapchain->BeginFrame(fraemBuffer, commandBuffer, frameInFlightIndex)) {
            RenderContext renderContext{ fraemBuffer, commandBuffer, frameInFlightIndex, m_swapchain->GetExtent() };

            m_rootNode->Render(renderContext);

            m_swapchain->EndFrame();
        }
    }

    void ShutDownSceneGraph() override
    {
        m_rootNode->ShutDown();

        GraphTraversal<NodeFlagsType>::Instance().SetRootNode(nullptr);
    }

    void ShutDown() override
    {
        AllocatorProvider::Instance().SetAllocator(nullptr);
    }

public:
    std::shared_ptr<Device> GetDevice() override
    {
        return m_device;
    }

    std::shared_ptr<Swapchain> GetSwapchain() override
    {
        return m_swapchain;
    }

    std::shared_ptr<RenderPass> GetRenderPass() override
    {
        return m_renderPass;
    }

    std::shared_ptr<Allocator> GetAllocator() override
    {
        return m_allocator;
    }

    std::shared_ptr<ISceneNode<NodeFlagsType> > GetRootNode() override
    {
        return m_rootNode;
    }

    void SetSceneRoot(const std::shared_ptr<ISceneNode<NodeFlagsType> >& root) override
    {
        GraphTraversal<NodeFlagsType>::Instance().SetRootNode(root);

        m_rootNode = root;
    }

public:
    void operator()(const WindowResizeEvent& resizeEvent)
    {
        m_swapchain->UpdateExtent();
    }

    void operator()(const SurfaceChanged& surfaceChangedEvent)
    {
        m_surface = surfaceChangedEvent.surface;
        InitSwapchain();
        m_swapchain->UpdateExtent();
    }
};
} // namespace PreVEngine

#endif // !__SCENE_H__