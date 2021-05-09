#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "../core/device/PhysicalDevices.h"
#include "../core/instance/Instance.h"
#include "../core/memory/Allocator.h"
#include "../event/EventHandler.h"
#include "../render/IRenderer.h"
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
    Engine(const std::shared_ptr<EngineConfig>& config);

    ~Engine() = default;

public:
    void Init();

    void InitScene();

    void InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode);

    void InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> >& rootRenderer);

    void MainLoop();

    void ShutDown();

    std::shared_ptr<prev::scene::IScene> GetScene() const;

    std::shared_ptr<prev::core::device::Device> GetDevice() const;

public:
    void operator()(const prev::window::WindowChangeEvent& windowChangeEvent);

private:
    void InitTiming();

    void InitInstance();

    void InitWindow();

    void InitSurface();

    void InitDevice();

    void InitAllocator();

private:
    prev::event::EventHandler<Engine, prev::window::WindowChangeEvent> m_windowChangedHandler{ *this };

private:
    std::shared_ptr<EngineConfig> m_config;

    std::unique_ptr<prev::util::Clock<float> > m_clock;

    std::unique_ptr<prev::util::FPSCounter> m_fpsCounter;

    std::unique_ptr<prev::core::instance::Instance> m_instance;

    std::unique_ptr<prev::window::IWindow> m_window;

    std::shared_ptr<prev::core::device::Device> m_device;

    std::shared_ptr<prev::core::memory::Allocator> m_allocator;

    std::shared_ptr<prev::scene::IScene> m_scene;

    VkSurfaceKHR m_surface;
};
} // namespace prev::core

#endif // !__ENGINE_H__
