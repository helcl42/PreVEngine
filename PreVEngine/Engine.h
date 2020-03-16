#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Scene.h"

namespace PreVEngine {
struct EngineConfig {
    // general
    bool validation = true;

    // window
    std::string appName = "PreVEngine - Demo";

    bool fullScreen = false;

    Size windowSize = { 1920, 1080 };

    Position windowPosition = { 40, 40 };

    std::shared_ptr<SceneConfig> sceneCongig = std::make_shared<SceneConfig>();
};

template <typename NodeFlagsType>
class Engine;

class DeviceProvider final : public Singleton<DeviceProvider> {
private:
    friend class Singleton<DeviceProvider>;

    template <typename NodeFlagsType>
    friend class Engine;

private:
    std::shared_ptr<Device> m_device;

private:
    DeviceProvider() = default;

public:
    ~DeviceProvider() = default;

private:
    void SetDevice(const std::shared_ptr<Device>& device)
    {
        m_device = device;
    }

public:
    std::shared_ptr<Device> GetDevice() const
    {
        return m_device;
    }
};

template <typename NodeFlagsType>
class Engine {
private:
    EventHandler<Engine, WindowChangeEvent> m_windowChangedHandler{ *this };

private:
    std::shared_ptr<EngineConfig> m_config;

    std::shared_ptr<Clock<float> > m_clock;

    std::shared_ptr<FPSService> m_fpsService;

    std::shared_ptr<Instance> m_instance;

    std::shared_ptr<Window> m_window;

    std::shared_ptr<Device> m_device;

    std::shared_ptr<IScene<NodeFlagsType> > m_scene;

    VkSurfaceKHR m_surface;

public:
    Engine(const std::shared_ptr<EngineConfig>& config)
        : m_config(config)
    {
    }

    virtual ~Engine() = default;

private:
    void InitTiming()
    {
        m_clock = std::make_shared<Clock<float> >();
        m_fpsService = std::make_shared<FPSService>();
    }

    void InitInstance()
    {
        m_instance = std::make_shared<Instance>(m_config->validation);
    }

    void InitWindow()
    {
        if (m_config->fullScreen) {
            m_window = std::make_shared<Window>(m_config->appName.c_str());
        } else {
            m_window = std::make_shared<Window>(m_config->appName.c_str(), m_config->windowSize.width, m_config->windowSize.height);
            m_window->SetPosition(m_config->windowPosition);
        }
    }

    void InitSurface()
    {
        m_surface = m_window->GetSurface(*m_instance);
    }

    void InitDevice()
    {
        auto physicalDevices = std::make_shared<PhysicalDevices>(*m_instance);
        physicalDevices->Print();

        PhysicalDevice* physicalDevice = physicalDevices->FindPresentable(m_surface);
        if (!physicalDevice) {
            throw std::runtime_error("No suitable GPU found?!");
        }

        m_device = std::make_shared<Device>(*physicalDevice);
        m_device->Print();
    }

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
        m_scene = std::make_shared<Scene<NodeFlagsType> >(m_config->sceneCongig, m_device, m_surface);
        m_scene->Init();
    }

    void InitSceneGraph()
    {
        m_scene->InitSceneGraph();
    }

    void MainLoop()
    {
        while (m_window->ProcessEvents()) // Main event loop, runs until window is closed.
        {
            EventChannel::DispatchAll();

            m_clock->UpdateClock();
            float deltaTime = m_clock->GetDelta();

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

public:
    std::shared_ptr<IScene<NodeFlagsType> > GetScene() const
    {
        return m_scene;
    }

public:
    void operator()(const WindowChangeEvent& windowChangeEvent)
    {
        vkDeviceWaitIdle(*m_device);

        InitSurface();

        EventChannel::Broadcast(SurfaceChanged{ m_surface });
    }
};
} // namespace PreVEngine

#endif // !__ENGINE_H__
