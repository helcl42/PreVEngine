#include "Engine.h"

#include "impl/EngineImplFactory.h"

#include "../CoreEvents.h"

#include "../../common/Logger.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

void Engine::RunOneFrame()
{
    prev::event::EventChannel::DispatchAll();

    if (!m_engineImpl->BeginFrame()) {
        return;
    }

    m_engineImpl->PollActions();

    auto& scene{ m_engineImpl->GetScene() };
    auto& rootRenderer{ m_engineImpl->GetRootRenderer() };
    auto& swapchain{ m_engineImpl->GetSwapchain() };

    const GfxExtent2D extent{ swapchain.GetExtent() };
    const auto deltaTime{ m_engineImpl->GetCurrentDeltaTime() };

    prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, extent.width, extent.height });

    if (m_engineImpl->IsFocused()) {
        scene.Update(deltaTime);

        prev::render::swapchain::FrameContext frameContext;
        if (swapchain.BeginFrame(frameContext)) {
            const prev::render::RenderContext renderContext{ frameContext.frameBuffer, frameContext.commandEncoder, frameContext.index, { { 0, 0 }, extent } };
            rootRenderer.Render(renderContext, scene);
            swapchain.EndFrame();
        }
    } else {
        LOGW("No focus...");
#ifndef __EMSCRIPTEN__
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif
    }

    m_engineImpl->EndFrame();
}

#ifdef __EMSCRIPTEN__
void Engine::EmscriptenMainLoopCallback(void* arg)
{
    auto* engine = static_cast<Engine*>(arg);
    if (!engine->m_engineImpl->Update()) {
        emscripten_cancel_main_loop();
        engine->m_engineImpl->EndMainLoop();
        return;
    }
    engine->RunOneFrame();
}
#endif

void Engine::MainLoop()
{
    m_engineImpl->BeginMainLoop();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(EmscriptenMainLoopCallback, this, 0, 1);
#else
    while (m_engineImpl->Update()) {
        RunOneFrame();
    }

    m_engineImpl->EndMainLoop();
#endif
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

prev::render::swapchain::ISwapchain& Engine::GetSwapchain() const
{
    return m_engineImpl->GetSwapchain();
}

prev::render::pass::RenderPass& Engine::GetRenderPass() const
{
    return m_engineImpl->GetRenderPass();
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
} // namespace prev::core::engine