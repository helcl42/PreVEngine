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

        // Frame scope: acquire the image + begin the single command buffer once. A frame is then rendered in
        // one or more passes: a single multiview pass, or one pass per eye where there is no multiview
        // (e.g. WebGPU). Each pass renders the view window the swapchain reports (viewOffset/viewCount) into
        // that pass's framebuffer; EndFrame submits the command buffer once.
        prev::render::swapchain::FrameContext frameContext;
        if (swapchain.BeginFrame(frameContext)) {
            // Per-frame resource bookkeeping (once): retire this slot's previous resources, then record queued
            // async uploads into the frame's command buffer so they precede all rendering this frame.
            m_engineImpl->GetDeferredResourceDestroyer().AdvanceFrame(frameContext.index);
            m_engineImpl->GetDeferredResourceUploader().Flush(frameContext.commandEncoder);

            const uint32_t passCount{ swapchain.GetPassCount() };
            prev::render::FrameSubmitSync submitSync{};
            for (uint32_t pass = 0; pass < passCount; ++pass) {
                swapchain.BeginPass(frameContext, pass);
                const prev::render::RenderContext renderContext{ frameContext.frameBuffer, frameContext.commandEncoder, frameContext.index, { { 0, 0 }, extent }, frameContext.viewOffset, frameContext.viewCount };
                submitSync = rootRenderer.Render(renderContext, scene); // XR sync is empty; use the last pass's for the single submit
                swapchain.EndPass(pass);
            }
            swapchain.EndFrame(submitSync);
        }
    } else {
        LOGW("No focus...");
#ifndef __EMSCRIPTEN__
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif
    }

    m_engineImpl->EndFrame();
}

bool Engine::Tick()
{
    if (!m_engineImpl->Update()) {
        return false; // quit requested
    }
    RunOneFrame();
    return true;
}

void Engine::MainLoop()
{
    m_engineImpl->BeginMainLoop();

    m_engineImpl->RunFrameLoop([this]() {
        if (!Tick()) {
            m_engineImpl->EndMainLoop();
            return false;
        }
        return true;
    });

#ifdef __EMSCRIPTEN__
    // Web frame loops are asynchronous - keep the runtime alive after RunFrameLoop returns.
    emscripten_exit_with_live_runtime();
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