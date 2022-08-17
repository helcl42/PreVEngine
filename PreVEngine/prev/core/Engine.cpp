#include "Engine.h"
#include "AllocatorProvider.h"
#include "DeviceProvider.h"
#include "device/DeviceFactory.h"

#include "../scene/Scene.h"
#include "../util/Utils.h"
#include "../window/Window.h"

#include <map>

namespace prev::core {
Engine::Engine(const EngineConfig& config)
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
    InitAllocator();

    DeviceProvider::Instance().SetDevice(m_device);
    AllocatorProvider::Instance().SetAllocator(m_allocator);
}

void Engine::InitScene()
{
    m_scene = std::make_shared<prev::scene::Scene>(m_config.sceneConfig, m_device, m_allocator, m_surface);
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
        const auto deltaTime{ m_clock->GetDelta() };

        prev::event::EventChannel::Post(NewIterationEvent{ deltaTime, m_window->GetSize().width, m_window->GetSize().height });

        if (m_window->HasFocus()) {
            m_scene->Update(deltaTime);
            m_scene->Render();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (m_fpsCounter->Tick()) {
            LOGI("FPS %f\n", m_fpsCounter->GetAverageFPS());
        }
    }
}

void Engine::ShutDown()
{
    m_scene->ShutDownRenderer();
    m_scene->ShutDownSceneGraph();
    m_scene->ShutDown();

    AllocatorProvider::Instance().SetAllocator(nullptr);
    DeviceProvider::Instance().SetDevice(nullptr);
}

std::shared_ptr<prev::scene::IScene> Engine::GetScene() const
{
    return m_scene;
}

std::shared_ptr<prev::core::device::Device> Engine::GetDevice() const
{
    return m_device;
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
    m_fpsCounter = std::make_unique<prev::util::FPSCounter>();
}

void Engine::InitInstance()
{
    m_instance = std::make_unique<prev::core::instance::Instance>(m_config.validation);
}

void Engine::InitWindow()
{
    prev::window::WindowCreateInfo windowCreateInfo{};
    windowCreateInfo.title = m_config.appName;
    windowCreateInfo.fullScreen = m_config.fullScreen;
    windowCreateInfo.left = static_cast<int16_t>(m_config.windowPosition.x);
    windowCreateInfo.top = static_cast<int16_t>(m_config.windowPosition.y);
    windowCreateInfo.width = static_cast<uint16_t>(m_config.windowSize.x);
    windowCreateInfo.height = static_cast<uint16_t>(m_config.windowSize.y);

    m_window = std::make_unique<prev::window::Window>(windowCreateInfo);
}

void Engine::InitSurface()
{
    m_surface = m_window->GetSurface(*m_instance);
}

void Engine::InitDevice()
{
    auto physicalDevices{ std::make_shared<prev::core::device::PhysicalDevices>(*m_instance) };
    physicalDevices->Print();

    auto presentablePhysicalDevice{ physicalDevices->FindPresentable(m_surface) };
    if (!presentablePhysicalDevice) {
        throw std::runtime_error("No suitable GPU found?!");
    }

    prev::core::device::DeviceFactory deviceFactory{};
    auto device{ deviceFactory.Create(presentablePhysicalDevice, m_surface) };
    if (!device) {
        throw std::runtime_error("Could not create logical device");
    }

    m_device = device;
    m_device->Print();
}

void Engine::InitAllocator()
{
    m_allocator = std::make_shared<prev::core::memory::Allocator>(*m_device, *m_device->GetQueue(device::QueueType::GRAPHICS)); // Create "Vulkan Memory Aloocator"
    printf("Allocator created\n");
}
} // namespace prev::core