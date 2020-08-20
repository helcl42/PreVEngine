#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "../common/pattern/Singleton.h"
#include "../core/device/PhysicalDevices.h"
#include "../core/instance/Instance.h"
#include "../scene/Scene.h"
#include "../util/Utils.h"
#include "../window/Window.h"

#include "CoreEvents.h"
#include "device/Device.h"

#include <memory>
#include <string>

namespace prev::core {
struct EngineConfig {
    bool validation{ true };

    std::string appName{ "PreVEngine - Demo" };

    bool fullScreen{ false };

    prev::window::impl::Size windowSize{ 1920, 1080 };

    prev::window::impl::Position windowPosition{ 40, 40 };

    std::shared_ptr<prev::scene::SceneConfig> sceneConfig{ std::make_shared<prev::scene::SceneConfig>() };
};

template <typename NodeFlagsType>
class Engine;

class DeviceProvider final : public prev::common::pattern::Singleton<DeviceProvider> {
public:
    ~DeviceProvider() = default;

public:
    std::shared_ptr<prev::core::device::Device> GetDevice() const
    {
        return m_device;
    }

private:
    friend class prev::common::pattern::Singleton<DeviceProvider>;

    template <typename NodeFlagsType>
    friend class Engine;

private:
    void SetDevice(const std::shared_ptr<prev::core::device::Device>& device)
    {
        m_device = device;
    }

private:
    DeviceProvider() = default;

private:
    std::shared_ptr<prev::core::device::Device> m_device;
};

template <typename NodeFlagsType>
class Engine final {
public:
    Engine(const std::shared_ptr<EngineConfig>& config)
        : m_config(config)
    {
    }

    ~Engine() = default;

public:
    void Init()
    {
        InitTiming();
        InitInstance();
        InitWindow();
        InitSurface();
        InitDevice();

        DeviceProvider::Instance().SetDevice(m_device);
    }

    void InitScene()
    {
        m_scene = std::make_shared<prev::scene::Scene<NodeFlagsType> >(m_config->sceneConfig, m_device, m_surface);
        m_scene->Init();
    }

    void InitSceneGraph()
    {
        m_scene->InitSceneGraph();
    }

    void MainLoop()
    {
        m_clock->Reset();

        while (m_window->ProcessEvents()) // Main event loop, runs until window is closed.
        {
            prev::event::EventChannel::DispatchAll();

            m_clock->UpdateClock();
            const auto deltaTime = m_clock->GetDelta();

            prev::event::EventChannel::Broadcast(NewIterationEvent{ deltaTime, m_window->GetSize().width, m_window->GetSize().height });

            if (m_window->HasFocus()) {
                m_scene->Update(deltaTime);
                m_scene->Render();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            m_fpsService->Update(deltaTime);
        }
    }

    void ShutDown()
    {
        m_scene->ShutDownSceneGraph();
        m_scene->ShutDown();

        DeviceProvider::Instance().SetDevice(nullptr);
    }

    std::shared_ptr<prev::scene::IScene<NodeFlagsType> > GetScene() const
    {
        return m_scene;
    }

    void operator()(const prev::window::WindowChangeEvent& windowChangeEvent)
    {
        vkDeviceWaitIdle(*m_device);

        InitSurface();

        prev::event::EventChannel::Broadcast(prev::window::SurfaceChanged{ m_surface });
    }

private:
    void InitTiming()
    {
        m_clock = std::make_unique<prev::util::Clock<float> >();
        m_fpsService = std::make_unique<prev::util::FPSService>();
    }

    void InitInstance()
    {
        m_instance = std::make_unique<prev::core::instance::Instance>(m_config->validation);
    }

    void InitWindow()
    {
        if (m_config->fullScreen) {
            m_window = std::make_unique<prev::window::Window>(m_config->appName.c_str());
        } else {
            m_window = std::make_unique<prev::window::Window>(m_config->appName.c_str(), m_config->windowSize.width, m_config->windowSize.height);
            m_window->SetPosition(m_config->windowPosition);
        }
    }

    void InitSurface()
    {
        m_surface = m_window->GetSurface(*m_instance);
    }

    void InitDevice()
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

private:
    prev::event::EventHandler<Engine, prev::window::WindowChangeEvent> m_windowChangedHandler{ *this };

private:
    std::shared_ptr<EngineConfig> m_config;

    std::unique_ptr<prev::util::Clock<float> > m_clock;

    std::unique_ptr<prev::util::FPSService> m_fpsService;

    std::unique_ptr<prev::core::instance::Instance> m_instance;

    std::unique_ptr<prev::window::IWindow> m_window;

    std::shared_ptr<prev::core::device::Device> m_device;

    std::shared_ptr<prev::scene::IScene<NodeFlagsType> > m_scene;

    VkSurfaceKHR m_surface;
};
} // namespace prev::core

#endif // !__ENGINE_H__
