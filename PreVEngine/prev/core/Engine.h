#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "../core/instance/Instance.h"
#include "../core/memory/Allocator.h"
#include "../event/EventHandler.h"
#include "../render/IRenderer.h"
#include "../render/Swapchain.h"
#include "../scene/IScene.h"
#include "../scene/graph/ISceneNode.h"
#include "../util/Utils.h"
#include "../window/WindowEvents.h"

#include "CoreEvents.h"
#include "EngineConfig.h"
#include "device/Device.h"

namespace prev::core {
class Engine final {
public:
    Engine(const EngineConfig& config);

    ~Engine() = default;

public:
    void Init();

    void InitScene(const std::shared_ptr<prev::scene::IScene>& scene);

    void InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>>& rootRenderer);

    void MainLoop();

    void ShutDown();

public:
    std::shared_ptr<prev::scene::IScene> GetScene() const;

    std::shared_ptr<prev::core::device::Device> GetDevice() const;

    std::shared_ptr<prev::render::Swapchain> GetSwapchain() const;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const;

    std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>> GetRootRenderer() const;

public:
    void operator()(const prev::window::WindowChangeEvent& windowChangeEvent);

    void operator()(const prev::window::WindowResizeEvent& resizeEvent);

    void operator()(const prev::window::SurfaceChanged& surfaceChangedEvent);

private:
    void InitTiming();

    void InitInstance();

    void InitWindow();

    void InitSurface();

    void InitDevice();

    void InitAllocator();

    void InitRenderPass();

    void InitSwapchain();

private:
    prev::event::EventHandler<Engine, prev::window::WindowChangeEvent> m_windowChangedHandler{ *this };

    prev::event::EventHandler<Engine, prev::window::WindowResizeEvent> m_windowResizeEvent{ *this };

    prev::event::EventHandler<Engine, prev::window::SurfaceChanged> m_surfaceChangedEvent{ *this };

private:
    const EngineConfig& m_config;

    std::unique_ptr<prev::util::Clock<float>> m_clock{};

    std::unique_ptr<prev::util::FPSCounter> m_fpsCounter{};

    std::unique_ptr<prev::core::instance::Instance> m_instance{};

    std::unique_ptr<prev::window::IWindow> m_window{};

    VkSurfaceKHR m_surface{};

    std::shared_ptr<prev::core::device::Device> m_device{};

    std::shared_ptr<prev::core::memory::Allocator> m_allocator{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::Swapchain> m_swapchain{};

    std::shared_ptr<prev::scene::IScene> m_scene{};

    std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData>> m_rootRenderer{};
};
} // namespace prev::core

#endif // !__ENGINE_H__
