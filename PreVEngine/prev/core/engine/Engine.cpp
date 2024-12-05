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

void Engine::InitScene(const std::shared_ptr<prev::scene::IScene>& scene)
{
    m_engineImpl->InitScene(scene);
}

void Engine::InitRenderer(const std::shared_ptr<prev::render::IRootRenderer>& rootRenderer)
{
    m_engineImpl->InitRenderer(rootRenderer);
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

        // TODO - unify extent -> remove & use the swapchain-> GetExtent() ??
        const VkExtent2D extent{ m_engineImpl->GetExtent() };
        const auto deltaTime{ m_engineImpl->GetCurrentDeltaTime() };

        prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, extent.width, extent.height });

        if (m_engineImpl->IsFocused()) {
            // TODO return reference from engine impl and engine instead of shared ptr (internally use unique_ptr) ???
            auto scene{ m_engineImpl->GetScene() };
            auto rootRenderer{ m_engineImpl->GetRootRenderer() };
            auto swapchain{ m_engineImpl->GetSwapchain() };

            scene->Update(deltaTime);

            prev::render::SwapChainFrameContext swapchainFrameContext;
            if (swapchain->BeginFrame(swapchainFrameContext)) {
                const prev::render::RenderContext renderContext{ swapchainFrameContext.frameBuffer, swapchainFrameContext.commandBuffer, swapchainFrameContext.index, { { 0, 0 }, swapchain->GetExtent() } };
                rootRenderer->Render(renderContext, scene);
                swapchain->EndFrame();
            }
        } else {
            LOGW("No focus...\n");
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

std::shared_ptr<prev::scene::IScene> Engine::GetScene() const
{
    return m_engineImpl->GetScene();
}

std::shared_ptr<prev::render::ISwapchain> Engine::GetSwapchain() const
{
    return m_engineImpl->GetSwapchain();
}

std::shared_ptr<prev::render::pass::RenderPass> Engine::GetRenderPass() const
{
    return m_engineImpl->GetRenderPass();
}

std::shared_ptr<prev::render::IRootRenderer> Engine::GetRootRenderer() const
{
    return m_engineImpl->GetRootRenderer();
}

std::shared_ptr<prev::core::memory::Allocator> Engine::GetAllocator() const
{
    return m_engineImpl->GetAllocator();
}

std::shared_ptr<prev::core::device::Device> Engine::GetDevice() const
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