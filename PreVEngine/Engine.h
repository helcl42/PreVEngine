#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Scene.h"

namespace PreVEngine
{
	struct EngineConfig
	{
		// general
		bool validation = true;

		// window
		std::string appName = "PreVEngine - Demo";

		bool fullScreen = false;

		Size windowSize = { 1280, 960 };

		Position windowPosition = { 40, 40 };

		std::shared_ptr<SceneConfig> sceneCongig = std::make_shared<SceneConfig>();
	};


	class Engine
	{
	private:
		std::shared_ptr<EngineConfig> m_config;

		std::shared_ptr<Clock<float>> m_clock;

		std::shared_ptr<FPSService> m_fpsService;

		std::shared_ptr<Instance> m_instance;

		std::shared_ptr<Window> m_window;

		std::shared_ptr<Device> m_device;

		std::shared_ptr<IScene> m_scene;

		VkSurfaceKHR m_surface;

	public:
		Engine(std::shared_ptr<EngineConfig> config)
			: m_config(config)
		{
		}

		virtual ~Engine()
		{
		}

	public:
		void Init()
		{
			m_clock = std::make_shared<Clock<float>>();
			m_fpsService = std::make_shared<FPSService>();

			m_instance = std::make_shared<Instance>(m_config->validation);

			if (m_config->fullScreen)
			{
				m_window = std::make_shared<Window>(m_config->appName.c_str());
			}
			else
			{
				m_window = std::make_shared<Window>(m_config->appName.c_str(), m_config->windowSize.width, m_config->windowSize.height);
				m_window->SetPosition(m_config->windowPosition);
			}

			auto physicalDevices = std::make_shared<PhysicalDevices>(*m_instance);
			physicalDevices->Print();

			m_surface = m_window->GetSurface(*m_instance);
			PhysicalDevice* physicalDevice = physicalDevices->FindPresentable(m_surface);					// get presenting GPU?
			if (!physicalDevice)
			{
				throw std::runtime_error("No GPU found?!");
			}

			m_device = std::make_shared<Device>(*physicalDevice);
			m_device->Print();
		}

		void InitScene()
		{
			m_scene = std::make_shared<Scene>(m_config->sceneCongig, m_device, m_surface);
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

				m_scene->Update(deltaTime);
				m_scene->Render();

				m_fpsService->Update(deltaTime);
			}
		}

		void ShutDown()
		{
			m_scene->ShutDownSceneGraph();
			m_scene->ShutDown();
		}

	public:
		std::shared_ptr<IScene> GetScene() const
		{
			return m_scene;
		}
	};
}

#endif // !__ENGINE_H__
