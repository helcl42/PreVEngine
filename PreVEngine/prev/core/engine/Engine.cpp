#include "Engine.h"

#include "impl/EngineImplFactory.h"

#include "../CoreEvents.h"

#include "../../common/Logger.h"

namespace prev::core::engine {
Engine::Engine(const Config& config)
    : m_engineImpl{ impl::EngineImplFactory{}.Create(config) }
{
}

Engine::~Engine()
{
    ShutDown();
}

void Engine::Init()
{
    m_engineImpl->Init();
}

void Engine::InitScene(std::unique_ptr<prev::scene::IScene> scene)
{
    m_engineImpl->InitScene(std::move(scene));
}

void Engine::InitRenderer(std::unique_ptr<prev::render::IRootRenderer> rootRenderer)
{
    m_engineImpl->InitRenderer(std::move(rootRenderer));
}

void Engine::MainLoop()
{
    m_engineImpl->BeginMainLoop();

    while (m_engineImpl->Update())
    {
        prev::event::EventChannel::DispatchQueued();

        if(!m_engineImpl->BeginFrame()) {
            continue;
        }

        m_engineImpl->PollActions();

        auto& scene{ m_engineImpl->GetScene() };
        auto& rootRenderer{ m_engineImpl->GetRootRenderer() };
        auto& swapchain{ m_engineImpl->GetSwapchain() };

        const VkExtent2D extent{ swapchain.GetExtent() };
        const auto deltaTime{ m_engineImpl->GetCurrentDeltaTime() };

        prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, extent.width, extent.height });

        if (m_engineImpl->IsFocused()) {
            scene.Update(deltaTime);

            prev::render::SwapChainFrameContext swapchainFrameContext;
            if (swapchain.BeginFrame(swapchainFrameContext)) {
                const prev::render::RenderContext renderContext{ swapchainFrameContext.frameBuffer, swapchainFrameContext.commandBuffer, swapchainFrameContext.index, { { 0, 0 }, extent } };
                rootRenderer.Render(renderContext, scene);
                swapchain.EndFrame();
            }
        } else {
            LOGW("No focus...");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        m_engineImpl->EndFrame();
    }

    m_engineImpl->EndMainLoop();
}

void Engine::ShutDown()
{
    m_engineImpl->ShutDown();
}

prev::scene::IScene& Engine::GetScene() const
{
    return m_engineImpl->GetScene();
}

prev::render::IRootRenderer& Engine::GetRootRenderer() const
{
    return m_engineImpl->GetRootRenderer();
}

prev::render::ISwapchain& Engine::GetSwapchain() const
{
    return m_engineImpl->GetSwapchain();
}

prev::render::pass::RenderPass& Engine::GetRenderPass() const
{
    return m_engineImpl->GetRenderPass();
}

prev::core::memory::Allocator& Engine::GetAllocator() const
{
    return m_engineImpl->GetAllocator();
}

prev::core::device::Device& Engine::GetDevice() const
{
    return m_engineImpl->GetDevice();
}

const Config& Engine::GetConfig() const
{
    return m_engineImpl->GetConfig();
}

uint32_t Engine::GetViewCount() const
{
    return m_engineImpl->GetViewCount();
}
} // namespace prev::core