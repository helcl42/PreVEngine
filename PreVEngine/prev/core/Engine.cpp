#include "Engine.h"

#include "../scene/Scene.h"
#include "../util/Utils.h"
#include "../window/Window.h"
#include "DeviceProvider.h"

namespace prev::core {
Engine::Engine(const std::shared_ptr<EngineConfig>& config)
    : m_config(config)
{
}

void Engine::Init()
{
    InitTiming();
    InitInstance();
    InitWindow();
    InitSurface();
    InitDevice();

    DeviceProvider::Instance().SetDevice(m_device);
}

void Engine::InitScene()
{
    m_scene = std::make_shared<prev::scene::Scene>(m_config->sceneConfig, m_device, m_surface);
    m_scene->Init();
}

void Engine::InitSceneGraph(const std::shared_ptr<prev::scene::graph::ISceneNode>& rootNode)
{
    m_scene->InitSceneGraph(rootNode);
}

void Engine::InitRenderer(const std::shared_ptr<prev::render::IRenderer<prev::render::DefaultRenderContextUserData> >& rootRenderer)
{
    m_scene->InitRenderer(rootRenderer);
}

void Engine::MainLoop()
{
    m_clock->Reset();

    while (m_window->ProcessEvents()) // Main event loop, runs until window is closed.
    {
        prev::event::EventChannel::DispatchAll();

        m_clock->UpdateClock();
        const auto deltaTime = m_clock->GetDelta();

        prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, m_window->GetSize().width, m_window->GetSize().height });

        if (m_window->HasFocus()) {
            m_scene->Update(deltaTime);
            m_scene->Render();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        m_fpsService->Update(deltaTime);
    }
}

void Engine::ShutDown()
{
    m_scene->ShutDownRenderer();
    m_scene->ShutDownSceneGraph();
    m_scene->ShutDown();

    DeviceProvider::Instance().SetDevice(nullptr);
}

std::shared_ptr<prev::scene::IScene> Engine::GetScene() const
{
    return m_scene;
}

void Engine::operator()(const prev::window::WindowChangeEvent& windowChangeEvent)
{
    vkDeviceWaitIdle(*m_device);

    InitSurface();

    prev::event::EventChannel::Post(prev::window::SurfaceChanged{ m_surface });
}

void Engine::InitTiming()
{
    m_clock = std::make_unique<prev::util::Clock<float> >();
    m_fpsService = std::make_unique<prev::util::FPSService>();
}

void Engine::InitInstance()
{
    m_instance = std::make_unique<prev::core::instance::Instance>(m_config->validation);
}

void Engine::InitWindow()
{
    if (m_config->fullScreen) {
        m_window = std::make_unique<prev::window::Window>(m_config->appName.c_str());
    } else {
        m_window = std::make_unique<prev::window::Window>(m_config->appName.c_str(), m_config->windowSize.x, m_config->windowSize.y);
        m_window->SetPosition({ static_cast<int16_t>(m_config->windowPosition.x), static_cast<int16_t>(m_config->windowPosition.y) });
    }
}

void Engine::InitSurface()
{
    m_surface = m_window->GetSurface(*m_instance);
}

void Engine::InitDevice()
{
    auto physicalDevices = std::make_shared<prev::core::device::PhysicalDevices>(*m_instance);
    physicalDevices->Print();

    auto physicalDevice = physicalDevices->FindPresentable(m_surface);
    if (!physicalDevice) {
        throw std::runtime_error("No suitable GPU found?!");
    }

    m_device = std::make_shared<prev::core::device::Device>(*physicalDevice);
    m_device->Print();
}
} // namespace prev::core