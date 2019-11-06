#include <iostream>
#include <random>
#include <memory>
#include <cstring>

#include <Window.h>
#include <Devices.h>
#include <RenderPass.h>
#include <Swapchain.h>
#include <Buffers.h>
#include <Image.h>
#include <Shader.h>
#include <Utils.h>

#include "matrix.h"
#include "Pipeline.h"
#include "../PreVEngine/VulkanWrapper/vulkan/vulkan.h"

using namespace PreVEngine;

uint16_t windowWidth = 1280;
uint16_t windowHeight = 960;
bool windowResized = false;
bool shouldAdd = false;
bool shouldDelete = false;

float dx = 0.1f;
float dy = 0.2f;


struct Vertex
{
	vec3 pos;
	vec2 tc;
};

struct Mesh
{
	const std::vector<Vertex> vertices = {
		//front
		{{-0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
		//back
		{{ 0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{-0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{-0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
		//left
		{{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{-0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
		//right
		{{ 0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
		//top
		{{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}},
		{{-0.5f,-0.5f, 0.5f}, {0.0f, 1.0f}},
		//bottom
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		8, 9,10, 10,11, 8,
	   12,13,14, 14,15,12,
	   16,17,18, 18,19,16,
	   20,21,22, 22,23,20
	};
};

struct Uniforms
{
	mat4 model;
	mat4 view;
	mat4 proj;
};

struct TexturedModel
{
	std::shared_ptr<Mesh> mesh;

	std::shared_ptr<Image> texture;

	std::shared_ptr<ImageBuffer> imageBuffer;

	std::shared_ptr<VBO> vertexBuffer;

	std::shared_ptr<IBO> indexBuffer;

	mat4 transform;
};

class ModelFactory
{
public:
	std::shared_ptr<TexturedModel> CreateTexturedModel(Allocator& allocator, const std::string& textureFilename, const mat4& transform)
	{
		ImageFactory imageFactory;

		auto resultModel = std::make_shared<TexturedModel>();

		resultModel->texture = imageFactory.CreateImage(textureFilename);

		VkExtent3D extent1 = { resultModel->texture->GetWidth(), resultModel->texture->GetHeight(), 1 };

		resultModel->imageBuffer = std::make_shared<ImageBuffer>(allocator);
		resultModel->imageBuffer->Data(resultModel->texture->GetBuffer(), extent1, VK_FORMAT_R8G8B8A8_UNORM, true);

		resultModel->mesh = std::make_shared<Mesh>();

		resultModel->vertexBuffer = std::make_shared<VBO>(allocator);                                                          // Create vertex buffer
		resultModel->vertexBuffer->Data((void*)resultModel->mesh->vertices.data(), (uint32_t)resultModel->mesh->vertices.size(), sizeof(Vertex));  // load vertex data
		printf("VBO created\n");

		resultModel->indexBuffer = std::make_shared<IBO>(allocator);
		resultModel->indexBuffer->Data(resultModel->mesh->indices.data(), (uint32_t)resultModel->mesh->indices.size());
		printf("IBO created\n");

		resultModel->transform = transform;

		return resultModel;
	}
};

class FancyWindow : public Window
{
private:
	float mx = 0;

	float my = 0;

public:
	FancyWindow(const char* title)
		: Window(title)
	{
	}

	FancyWindow(const char* title, const uint32_t width, const uint32_t height)
		: Window(title, width, height)
	{
	}

private:
	void Update(float deltaTime)
	{
	}

	void Render()
	{
	}

public:
	void Run()
	{

	}

public:
	void OnResizeEvent(uint16_t width, uint16_t height) override
	{
		printf("OnResizeEvent: %d x %d\n", width, height);
		windowResized = true;
		windowWidth = width;
		windowHeight = height;
	}

	void OnKeyEvent(ActionType action, KeyCode keyCode) override
	{
		if (action == ActionType::DOWN)
		{
			if (keyCode == KeyCode::KEY_Left)
			{
				dy -= 0.1f;
			}

			if (keyCode == KeyCode::KEY_Right)
			{
				dy += 0.1f;
			}

			if (keyCode == KeyCode::KEY_Up)
			{
				dx += 0.1f;
			}

			if (keyCode == KeyCode::KEY_Down)
			{
				dx -= 0.1f;
			}

			if (keyCode == KeyCode::KEY_A)
			{
				shouldAdd = true;
			}

			if (keyCode == KeyCode::KEY_D)
			{
				shouldDelete = true;
			}
		}
	}

	void OnMouseEvent(ActionType action, int16_t x, int16_t y, MouseButtonType button) override
	{
		if (action == ActionType::MOVE && button == MouseButtonType::LEFT)
		{
			dy = x - mx;
			dx = my - y;
		}

		mx = x;
		my = y;
	}

	void OnTouchEvent(ActionType action, float x, float y, uint8_t id) override
	{
		if (action == ActionType::MOVE)
		{
			dy = x - mx;
			dx = my - y;
		}

		mx = x;
		my = y;
	}

	void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) override
	{
		std::cout << "Mouse scroll: " << delta << std::endl;
	}

	void OnFocusEvent(bool hasFocus) override
	{
		std::cout << "Window Focus changed to: " << (hasFocus ? "YES" : "NO") << std::endl;
	}

	void OnInitEvent() override
	{
		std::cout << "Window initialized" << std::endl;
	}

	void OnCloseEvent() override
	{
		std::cout << "Window closed" << std::endl;
	}
};

class ISceneNode
{
public:
	virtual void Init() = 0;

	virtual void Update(float DeltaTime) = 0;

	virtual void Render() = 0;

	virtual void ShutDown() = 0;

	virtual std::vector<std::shared_ptr<ISceneNode>> GetChildren() const = 0;

	virtual void AddChild(const std::shared_ptr<ISceneNode> child) = 0;

public:
	virtual ~ISceneNode()
	{
	}
};

class AbstractSceneNode : public ISceneNode
{
protected:
	std::vector<std::shared_ptr<ISceneNode>> m_children;

public:
	std::vector<std::shared_ptr<ISceneNode>> GetChildren() const override
	{
		return m_children;
	}

	void AddChild(const std::shared_ptr<ISceneNode> child) override
	{
		m_children.emplace_back(child);
	}
};

class RootSceneNode : public AbstractSceneNode
{
public:
	void Init() override
	{
	}

	void Update(float deltaTime) override
	{
	}

	void Render() override
	{
	}

	void ShutDown() override
	{
	}
};

class Scene
{
private:
	Device& m_device;

	Swapchain& m_swapchain;

    RenderPass& m_renderPass;

	Allocator& m_allocator;

	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<Pipeline> m_pipeline;

	// content
	std::vector<std::shared_ptr<TexturedModel>> m_models;

	std::vector<std::shared_ptr<UBO>> m_uniformBuffers;

public:
	Scene(Device& dev, Swapchain& swapCh, RenderPass& renderPass, Allocator& alloc)
		: m_device(dev), m_swapchain(swapCh), m_renderPass(renderPass), m_allocator(alloc)
	{
	}

	virtual ~Scene()
	{
	}

private:
	void AddModel()
	{
		std::random_device r;
		std::default_random_engine gen(r());
		std::uniform_real_distribution<float> dist(-2.0, 2.0f);
		std::uniform_real_distribution<float> scaleDist(0.1f, 1.0f);

		ModelFactory modelFactory;

		mat4 trasform;
		trasform.Translate(dist(gen), dist(gen), dist(gen));
		trasform.Scale(scaleDist(gen));

		m_models.emplace_back(modelFactory.CreateTexturedModel(m_allocator, dist(gen) > 0.0f ? "vulkan.png" : "texture.jpg", trasform));

		for (size_t i = 0; i < m_swapchain.GetmageCount(); i++)
		{
			auto ubo = std::make_shared<UBO>(m_allocator);
			ubo->Allocate(sizeof(Uniforms));

			m_uniformBuffers.emplace_back(ubo);
		}

		printf("Model %zd created\n", m_models.size());
	}

	void DeleteModel()
	{
		if (m_models.size() > 0)
		{
			m_models.erase(m_models.begin());
		}

		if (m_uniformBuffers.size() >= m_swapchain.GetmageCount())
		{
			m_uniformBuffers.erase(m_uniformBuffers.begin(), m_uniformBuffers.begin() + m_swapchain.GetmageCount());
		}

		printf("Model deleted %zd\n", m_models.size());
	}

public:
	void Init()
	{
		const uint32_t COUNT_OF_MODELS = 25;
		for (uint32_t i = 0; i < COUNT_OF_MODELS; i++)
		{
			AddModel();
		}

		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(m_device, {
												{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv" },
												{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv" }
			});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		m_pipeline = std::make_shared<Pipeline>(m_device, m_renderPass, *m_shader);
		m_pipeline->CreateGraphicsPipeline();
		printf("Pipeline created\n");
	}

	void Update(float deltaTime)
	{
		if (shouldAdd)
		{
			AddModel();
			shouldAdd = false;
		}

		if (shouldDelete)
		{
			DeleteModel();
			shouldDelete = false;
		}
	}

	void Render()
	{
		if (windowResized)
		{
			m_swapchain.UpdateExtent();
			windowResized = false;
		}

		VkExtent2D ext = m_swapchain.GetExtent();
		VkRect2D scissor = { {0, 0}, ext };
		VkViewport viewport = { 0, 0, (float)ext.width, (float)ext.height, 0, 1 };

		float aspect = (float)ext.width / (float)ext.height;
		
		VkCommandBuffer commandBuffer;
		uint32_t frameInFlightIndex;
		if (m_swapchain.BeginFrame(commandBuffer, frameInFlightIndex))
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			size_t modelIndex = 0;
			for (const auto& model : m_models)
			{
				VkBuffer vertexBuffers[] = { *model->vertexBuffer };
				VkDeviceSize offsets[] = { 0 };

				auto& ubo = m_uniformBuffers.at(modelIndex);

				Uniforms uniforms;
				uniforms.proj.SetProjection(aspect, 66.0f, 0.01f, 100.0f);
				uniforms.view.Translate(0.0f, 0.0f, -4.0f);
				uniforms.model = model->transform;
				uniforms.model.RotateX(dx);
				uniforms.model.RotateY(dy);
				model->transform = uniforms.model;
				ubo->Update(&uniforms);

				m_shader->Bind("texSampler", *model->imageBuffer);
				m_shader->Bind("ubo", *ubo);
				VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, *model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, model->indexBuffer->GetCount(), 1, 0, 0, 0);

				modelIndex++;
			}

			m_swapchain.EndFrame();
		}
	}

	void ShutDown()
	{
	}
};

struct EngineConfig
{
	// general
	bool validation = true;

	// swapchain
	bool VSync = true;

	uint32_t framesInFlight = 3;

	// window
	std::string appName = "PreVEngine - Demo";

	bool fullScreen = false;

	Size windowSize = { 1280, 960 };

	Position windowPosition = { 40, 40 };
};

class Engine
{
private:
	EngineConfig m_config;

	std::shared_ptr<IClock<float>> m_clock;

	std::shared_ptr<FPSService> m_fpsService;

	std::shared_ptr<Instance> m_instance;

	std::shared_ptr<FancyWindow> m_window;

	std::shared_ptr<PhysicalDevices> m_physicalDevices;

	PhysicalDevice *m_physicalDevice;

	std::shared_ptr<Device> m_device;

	Queue* m_presentQueue;

	Queue* m_graphicsQueue;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Swapchain> m_swapchain;

	std::shared_ptr<Allocator> m_allocator;

	std::shared_ptr<Scene> m_scene;

public:
	Engine(const EngineConfig& config)
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

		m_instance = std::make_shared<Instance>(m_config.validation);

		m_window = std::make_shared<FancyWindow>(m_config.appName.c_str(), m_config.windowSize.width, m_config.windowSize.height);
		m_window->SetPosition(m_config.windowPosition);

		m_physicalDevices = std::make_shared<PhysicalDevices>(*m_instance);
		m_physicalDevices->Print();

		VkSurfaceKHR surface = m_window->GetSurface(*m_instance);
		m_physicalDevice = m_physicalDevices->FindPresentable(surface);					// get presenting GPU?
		if (!m_physicalDevice)
		{
			throw std::runtime_error("No GPU found?!");
		}

		m_device = std::make_shared<Device>(*m_physicalDevice);

		m_presentQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT, surface);   // graphics + present-queue
		m_graphicsQueue = m_presentQueue;                                     // they might be the same or not
		if (!m_presentQueue)
		{
			m_presentQueue = m_device->AddQueue(0, surface);                          // create present-queue
			m_graphicsQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT);             // create graphics queue
		}

		m_device->Print();

		VkFormat colorFormat = m_physicalDevice->FindSurfaceFormat(surface);
		VkFormat depthFormat = m_physicalDevice->FindDepthFormat();

		m_renderPass = std::make_shared<RenderPass>(*m_device);
		m_renderPass->AddColorAttachment(colorFormat, { 0.0f, 0.0f, 0.3f, 1.0f });	// color buffer, clear to blue
		m_renderPass->AddDepthAttachment(depthFormat);
		m_renderPass->AddSubpass({ 0, 1 });

		m_swapchain = std::make_shared<Swapchain>(*m_renderPass, m_graphicsQueue, m_graphicsQueue);
		m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
		m_swapchain->SetImageCount(m_config.framesInFlight);
		m_swapchain->Print();

		m_allocator = std::make_shared<Allocator>(*m_graphicsQueue);                   // Create "Vulkan Memory Aloocator"
		printf("Allocator created\n");

		m_scene = std::make_shared<Scene>(*m_device, *m_swapchain, *m_renderPass, *m_allocator);
		m_scene->Init();
	}

	void MainLoop()
	{
		while (m_window->ProcessEvents()) // Main event loop, runs until window is closed.
		{
			m_clock->UpdateClock();
			float deltaTime = m_clock->GetDelta();

			m_scene->Update(deltaTime);
			m_scene->Render();

			m_fpsService->Update(deltaTime);
		}
	}

	void ShutDown()
	{
		m_scene->ShutDown();
	}
};

class App
{
private:
	std::unique_ptr<Engine> m_engine;

public:
	App(const EngineConfig& config)
		: m_engine(std::make_unique<Engine>(config))
	{
	}

	virtual ~App()
	{
	}

public:
	void Init()
	{
	}

	void Run()
	{
		m_engine->Init();
		m_engine->MainLoop();
		m_engine->ShutDown();
	}
};


int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

	EngineConfig config;
	App app(config);
	app.Init();
	app.Run();

	return 0;
}
