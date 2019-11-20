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
#include <Events.h>
#include <Utils.h>

#include "matrix.h"
#include "Pipeline.h"
#include "../PreVEngine/VulkanWrapper/vulkan/vulkan.h"

using namespace PreVEngine;

////////////////////////////////////////////////////////////////////////
// Move to engine
////////////////////////////////////////////////////////////////////////

struct RenderState
{
	VkCommandBuffer commandBuffer;

	uint32_t frameInFlightIndex;

	VkExtent2D fullExtent;
};

class ISceneNode
{
public:
	virtual void Init() = 0;

	virtual void ShutDown() = 0;

	virtual void Update(float deltaTime) = 0;

	virtual void Render(RenderState& renderState) = 0;

	virtual const std::vector<ISceneNode*>& GetChildren() const = 0;

	virtual void AddChild(ISceneNode* child) = 0;

	virtual void SetParent(ISceneNode* parent) = 0;

	virtual ISceneNode* GetParent() const = 0;

	virtual glm::mat4 GetTransform() const = 0;

	virtual glm::mat4 GetWorldTransform() const = 0;

public:
	virtual ~ISceneNode() = default;
};

class AbstractSceneNode : public ISceneNode
{
protected:
	ISceneNode* m_parent;

	std::vector<ISceneNode*> m_children;

	glm::mat4 m_transform;

	glm::mat4 m_worldTransform;

public:
	AbstractSceneNode()
		: m_parent(nullptr), m_transform(glm::mat4(1.0f)), m_worldTransform(glm::mat4(1.0f))
	{
	}

	virtual ~AbstractSceneNode()
	{
		for (auto child : m_children)
		{
			delete child;
		}
	}

public:
	virtual void Init() override
	{
	}

	virtual void Update(float deltaTime) override
	{
		if (m_parent) //This node has a parent... 
		{
			m_worldTransform = m_parent->GetWorldTransform() * m_transform;
		}
		else //Root node, world transform is local transform! 
		{
			m_worldTransform = m_transform;
		}

		for (auto& child : m_children)
		{
			child->Update(deltaTime);
		}
	}

	virtual void Render(RenderState& renderState) override
	{
	}

	virtual void ShutDown() override
	{
	}

public:
	const std::vector<ISceneNode*>& GetChildren() const override
	{
		return m_children;
	}

	void AddChild(ISceneNode* child) override
	{
		child->SetParent(this);

		m_children.emplace_back(child);
	}

	void SetParent(ISceneNode* parent)
	{
		m_parent = parent;
	}

	ISceneNode* GetParent() const
	{
		return m_parent;
	}

	glm::mat4 GetTransform() const
	{
		return m_transform;
	}

	glm::mat4 GetWorldTransform() const
	{
		return m_worldTransform;
	}
};


struct SceneConfig
{
	// swapchain
	bool VSync = true;

	uint32_t framesInFlight = 3;
};


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


class IScene
{
public:
	virtual void Init() = 0;

	virtual void InitSceneGraph() = 0;

	virtual void Update(float deltaTime) = 0;

	virtual void Render() = 0;

	virtual void ShutDown() = 0;

	virtual std::shared_ptr<Device> GetDevice() = 0;

	virtual std::shared_ptr<Swapchain> GetSwapchain() = 0;

	virtual std::shared_ptr<RenderPass> GetRenderPass() = 0;

	virtual std::shared_ptr<Allocator> GetAllocator() = 0;

	virtual ISceneNode* GetRootNode() = 0; // TODO

	virtual void SetSceneRoot(ISceneNode* root) = 0; // TODO

public:
	virtual ~IScene() = default;
};

class Scene : public IScene
{
private:
	EventHandler<Scene, WindowResizeEvent> m_windowResizeEvent{ *this };

protected:
	std::shared_ptr<SceneConfig> m_config;

	std::shared_ptr<Device> m_device;

	Queue* m_presentQueue;

	Queue* m_graphicsQueue;

	VkSurfaceKHR m_surface;

protected:
	std::shared_ptr<Allocator> m_allocator;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Swapchain> m_swapchain;

	ISceneNode* m_rootNode;

public:
	Scene(std::shared_ptr<SceneConfig> sceneConfig, std::shared_ptr<Device> device, VkSurfaceKHR surface)
		: m_config(sceneConfig), m_device(device), m_surface(surface)
	{
	}

	virtual ~Scene()
	{
		delete m_rootNode;
	}

public:
	void Init()
	{
		m_presentQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT, m_surface);   // graphics + present-queue
		m_graphicsQueue = m_presentQueue;                                     // they might be the same or not
		if (!m_presentQueue)
		{
			m_presentQueue = m_device->AddQueue(0, m_surface);                          // create present-queue
			m_graphicsQueue = m_device->AddQueue(VK_QUEUE_GRAPHICS_BIT);             // create graphics queue
		}

		VkFormat colorFormat = m_device->GetGPU().FindSurfaceFormat(m_surface);
		VkFormat depthFormat = m_device->GetGPU().FindDepthFormat();

		m_renderPass = std::make_shared<RenderPass>(*m_device);
		m_renderPass->AddColorAttachment(colorFormat, { 0.0f, 0.0f, 0.3f, 1.0f });	// color buffer, clear to blue
		m_renderPass->AddDepthAttachment(depthFormat);
		m_renderPass->AddSubpass({ 0, 1 });

		m_allocator = std::make_shared<Allocator>(*m_graphicsQueue);                   // Create "Vulkan Memory Aloocator"
		printf("Allocator created\n");

		m_swapchain = std::make_shared<Swapchain>(*m_allocator, *m_renderPass, m_graphicsQueue, m_graphicsQueue);
		m_swapchain->SetPresentMode(m_config->VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
		m_swapchain->SetImageCount(m_config->framesInFlight);
		m_swapchain->Print();
	}

	void InitSceneGraph()
	{
		m_rootNode->Init();
	}

	void Update(float deltaTime)
	{
		m_rootNode->Update(deltaTime);
	}

	void Render()
	{
		VkCommandBuffer commandBuffer;
		uint32_t frameInFlightIndex;
		if (m_swapchain->BeginFrame(commandBuffer, frameInFlightIndex))
		{
			RenderState renderState;
			renderState.commandBuffer = commandBuffer;
			renderState.frameInFlightIndex = frameInFlightIndex;
			renderState.fullExtent = m_swapchain->GetExtent();

			m_rootNode->Render(renderState);

			m_swapchain->EndFrame();
		}
	}

	void ShutDownSceneGraph()
	{
		m_rootNode->ShutDown();
	}

	void ShutDown()
	{
	}

public:
	std::shared_ptr<Device> GetDevice() override
	{
		return m_device;
	}

	std::shared_ptr<Swapchain> GetSwapchain() override
	{
		return m_swapchain;
	}

	std::shared_ptr<RenderPass> GetRenderPass() override
	{
		return m_renderPass;
	}

	std::shared_ptr<Allocator> GetAllocator() override
	{
		return m_allocator;
	}

	ISceneNode* GetRootNode() override
	{
		return m_rootNode;
	}

	void SetSceneRoot(ISceneNode* root) override
	{
		m_rootNode = root;
	}

public:
	void operator() (const WindowResizeEvent& resizeEvent)
	{
		m_swapchain->UpdateExtent();
	}
};

class Engine
{
private:
	std::shared_ptr<EngineConfig> m_config;

	std::shared_ptr<IClock<float>> m_clock;

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
		m_scene->ShutDown();
	}

public:
	std::shared_ptr<IScene> GetScene() const
	{
		return m_scene;
	}
};

class App
{
protected:
	std::unique_ptr<Engine> m_engine;

public:
	App(std::shared_ptr<EngineConfig> config)
		: m_engine(std::make_unique<Engine>(config))
	{
	}

	virtual ~App()
	{
	}

protected:
	virtual void OnEngineInit() = 0;

	virtual void OnSceneInit() = 0;

	virtual void OnSceneGraphInit() = 0;

public:
	void Init()
	{
		m_engine->Init();

		OnEngineInit();

		m_engine->InitScene();

		OnSceneInit();

		m_engine->InitSceneGraph();

		OnSceneGraphInit();
	}

	void Run()
	{
		m_engine->MainLoop();
	}

	void ShutDown()
	{
		m_engine->ShutDown();
	}
};

////////////////////////////////////////////////////////////////////////

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

		VkExtent2D extent = { resultModel->texture->GetWidth(), resultModel->texture->GetHeight() };

		auto imageBuffer = std::make_shared<ImageBuffer>(allocator);
		imageBuffer->Create(ImageBufferCreateInfo{ extent, VK_FORMAT_R8G8B8A8_UNORM, true, resultModel->texture->GetBuffer() });
		resultModel->imageBuffer = imageBuffer;

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

//class Scene
//{
//private:
//	EventHandler<Scene, WindowResizeEvent> m_windowResizeEvent{ *this };
//
//private:
//	Device& m_device;
//
//	Swapchain& m_swapchain;
//
//	RenderPass& m_renderPass;
//
//	Allocator& m_allocator;
//
//private: // the rest should ne in derived class
//	std::shared_ptr<Shader> m_shader;
//
//	std::shared_ptr<Pipeline> m_pipeline;
//
//	// content
//	std::vector<std::shared_ptr<TexturedModel>> m_models;
//
//	std::vector<std::shared_ptr<UBO>> m_uniformBuffers;
//
//	// temp bullshit
//	EventHandler<Scene, KeyEvent> m_keyEvent{ *this };
//
//	EventHandler<Scene, MouseEvent> m_mouseEvent{ *this };
//
//	EventHandler<Scene, TouchEvent> m_touchEvent{ *this };
//
//	glm::vec2 d{ 0.1f, 0.1f };
//
//	glm::vec2 m{ 0.0f, 0.0f };
//
//public:
//	Scene(Allocator& alloc, Device& dev, Swapchain& swapCh, RenderPass& renderPass)
//		: m_device(dev), m_swapchain(swapCh), m_renderPass(renderPass), m_allocator(alloc)
//	{
//	}
//
//	virtual ~Scene()
//	{
//	}
//
//private:
//	void AddModel()
//	{
//		std::random_device r;
//		std::default_random_engine gen(r());
//		std::uniform_real_distribution<float> dist(-2.0, 2.0f);
//		std::uniform_real_distribution<float> scaleDist(0.1f, 1.0f);
//
//		ModelFactory modelFactory;
//
//		mat4 trasform;
//		trasform.Translate(dist(gen), dist(gen), dist(gen));
//		trasform.Scale(scaleDist(gen));
//
//		m_models.emplace_back(modelFactory.CreateTexturedModel(m_allocator, dist(gen) > 0.0f ? "vulkan.png" : "texture.jpg", trasform));
//
//		auto ubo = std::make_shared<UBO>(m_allocator);
//		ubo->Allocate(sizeof(Uniforms));
//
//		m_uniformBuffers.emplace_back(ubo);
//
//		printf("Model %zd created\n", m_models.size());
//	}
//
//	void DeleteModel()
//	{
//		if (m_models.size() > 0)
//		{
//			m_models.erase(m_models.begin());
//		}
//
//		if (m_uniformBuffers.size() > 0)
//		{
//			m_uniformBuffers.erase(m_uniformBuffers.begin(), m_uniformBuffers.begin() + 1);
//		}
//
//		printf("Model deleted %zd\n", m_models.size());
//	}
//
//public:
//	void Init()
//	{
//		const uint32_t COUNT_OF_MODELS = 25;
//		for (uint32_t i = 0; i < COUNT_OF_MODELS; i++)
//		{
//			AddModel();
//		}
//
//		ShaderFactory shaderFactory;
//		m_shader = shaderFactory.CreateShaderFromFiles(m_device, {
//												{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv" },
//												{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv" }
//			});
//		m_shader->AdjustDescriptorPoolCapacity(10000);
//
//		m_pipeline = std::make_shared<Pipeline>(m_device, m_renderPass, *m_shader);
//		m_pipeline->CreateGraphicsPipeline();
//
//		printf("Pipeline created\n");
//	}
//
//	void Update(float deltaTime)
//	{
//	}
//
//	void Render()
//	{
//		// This should be part of Scene InEngine
//
//		VkExtent2D extent = m_swapchain.GetExtent();
//		VkRect2D scissor = { {0, 0}, extent };
//		VkViewport viewport = { 0, 0, (float)extent.width, (float)extent.height, 0, 1 };
//
//		float aspect = (float)extent.width / (float)extent.height;
//
//		VkCommandBuffer commandBuffer;
//		uint32_t frameInFlightIndex;
//		if (m_swapchain.BeginFrame(commandBuffer, frameInFlightIndex))
//		{
//			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
//			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//
//			// ~This should be part of Scene InEngine
//
//
//			size_t modelIndex = 0;
//			for (const auto& model : m_models)
//			{
//				VkBuffer vertexBuffers[] = { *model->vertexBuffer };
//				VkDeviceSize offsets[] = { 0 };
//
//				auto& ubo = m_uniformBuffers.at(modelIndex);
//
//				Uniforms uniforms;
//				uniforms.proj.SetProjection(aspect, 66.0f, 0.01f, 100.0f);
//				uniforms.view.Translate(0.0f, 0.0f, -4.0f);
//				uniforms.model = model->transform;
//				uniforms.model.RotateX(d.x);
//				uniforms.model.RotateY(d.y);
//				model->transform = uniforms.model;
//				ubo->Update(&uniforms);
//
//				m_shader->Bind("texSampler", *model->imageBuffer);
//				m_shader->Bind("ubo", *ubo);
//				VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
//
//				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//				vkCmdBindIndexBuffer(commandBuffer, *model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
//				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
//
//				vkCmdDrawIndexed(commandBuffer, model->indexBuffer->GetCount(), 1, 0, 0, 0);
//
//				modelIndex++;
//			}
//
//			// This should be part of Scene InEngine
//
//			m_swapchain.EndFrame();
//		}
//
//		// ~This should be part of Scene InEngine
//	}
//
//	void ShutDown()
//	{
//	}
//
//public:
//	void operator() (const WindowResizeEvent& resizeEvent)
//	{
//		m_swapchain.UpdateExtent();
//	}
//
//	void operator() (const KeyEvent& keyEvent)
//	{
//		if (keyEvent.action == KeyActionType::PRESS)
//		{
//			if (keyEvent.keyCode == KeyCode::KEY_A)
//			{
//				AddModel();
//			}
//
//			if (keyEvent.keyCode == KeyCode::KEY_D)
//			{
//				DeleteModel();
//			}
//
//			if (keyEvent.keyCode == KeyCode::KEY_Left)
//			{
//				d.y -= 0.1f;
//			}
//
//			if (keyEvent.keyCode == KeyCode::KEY_Right)
//			{
//				d.y += 0.1f;
//			}
//
//			if (keyEvent.keyCode == KeyCode::KEY_Up)
//			{
//				d.x += 0.1f;
//			}
//
//			if (keyEvent.keyCode == KeyCode::KEY_Down)
//			{
//				d.x -= 0.1f;
//			}
//		}
//	}
//
//	void operator() (const MouseEvent& mouseEvent)
//	{
//		if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::LEFT)
//		{
//			d = glm::vec2(mouseEvent.position.x - m.x, m.y - mouseEvent.position.y);
//		}
//
//		m = mouseEvent.position;
//	}
//
//	void operator() (const TouchEvent& touchEvent)
//	{
//		if (touchEvent.action == TouchActionType::MOVE)
//		{
//			d = glm::vec2(touchEvent.position.x - m.x, m.y - touchEvent.position.y);
//		}
//
//		m = touchEvent.position;
//	}
//};

class TestRootSceneNode : public AbstractSceneNode
{
private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<Pipeline> m_pipeline;

	// content
	std::vector<std::shared_ptr<TexturedModel>> m_models;

	std::vector<std::shared_ptr<UBO>> m_uniformBuffers;

	EventHandler<TestRootSceneNode, KeyEvent> m_keyEvent{ *this };

	EventHandler<TestRootSceneNode, MouseEvent> m_mouseEvent{ *this };

	EventHandler<TestRootSceneNode, TouchEvent> m_touchEvent{ *this };

	glm::vec2 d{ 0.1f, 0.1f };

	glm::vec2 m{ 0.0f, 0.0f };

public:
	TestRootSceneNode(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass)
		: m_device(dev), m_renderPass(renderPass), m_allocator(alloc)
	{
	}

	virtual ~TestRootSceneNode()
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

		m_models.emplace_back(modelFactory.CreateTexturedModel(*m_allocator, dist(gen) > 0.0f ? "vulkan.png" : "texture.jpg", trasform));

		auto ubo = std::make_shared<UBO>(*m_allocator);
		ubo->Allocate(sizeof(Uniforms));

		m_uniformBuffers.emplace_back(ubo);

		printf("Model %zd created\n", m_models.size());
	}

	void DeleteModel()
	{
		if (m_models.size() > 0)
		{
			m_models.erase(m_models.begin());
		}

		if (m_uniformBuffers.size() > 0)
		{
			m_uniformBuffers.erase(m_uniformBuffers.begin(), m_uniformBuffers.begin() + 1);
		}

		printf("Model deleted %zd\n", m_models.size());
	}

public:
	void Init() override
	{
		const uint32_t COUNT_OF_MODELS = 25;
		for (uint32_t i = 0; i < COUNT_OF_MODELS; i++)
		{
			AddModel();
		}

		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(*m_device, {
			{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv" },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv" }
			});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		m_pipeline = std::make_shared<Pipeline>(*m_device, *m_renderPass, *m_shader);
		m_pipeline->CreateGraphicsPipeline();

		printf("Pipeline created\n");
	}

	void Update(float deltaTime) override
	{
		// TODO
	}

	void Render(RenderState& renderState) override
	{
		VkRect2D scissor = { { 0, 0 }, renderState.fullExtent };
		VkViewport viewport = { 0, 0, (float)renderState.fullExtent.width, (float)renderState.fullExtent.height, 0, 1 };

		float aspect = (float)renderState.fullExtent.width / (float)renderState.fullExtent.height;

		vkCmdBindPipeline(renderState.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		vkCmdSetViewport(renderState.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderState.commandBuffer, 0, 1, &scissor);

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
			uniforms.model.RotateX(d.x);
			uniforms.model.RotateY(d.y);
			model->transform = uniforms.model;
			ubo->Update(&uniforms);

			m_shader->Bind("texSampler", *model->imageBuffer);
			m_shader->Bind("ubo", *ubo);
			VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

			vkCmdBindVertexBuffers(renderState.commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(renderState.commandBuffer, *model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(renderState.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

			vkCmdDrawIndexed(renderState.commandBuffer, model->indexBuffer->GetCount(), 1, 0, 0, 0);

			modelIndex++;
		}
	}

	void ShutDown() override
	{
		m_pipeline->ShutDown();

		m_shader->ShutDown();
		// TODO cleanup
	}

public:	
	void operator() (const KeyEvent& keyEvent)
	{
		if (keyEvent.action == KeyActionType::PRESS)
		{
			if (keyEvent.keyCode == KeyCode::KEY_A)
			{
				AddModel();
			}

			if (keyEvent.keyCode == KeyCode::KEY_D)
			{
				DeleteModel();
			}

			if (keyEvent.keyCode == KeyCode::KEY_Left)
			{
				d.y -= 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Right)
			{
				d.y += 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Up)
			{
				d.x += 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Down)
			{
				d.x -= 0.1f;
			}
		}
	}

	void operator() (const MouseEvent& mouseEvent)
	{
		if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::LEFT)
		{
			d = glm::vec2(mouseEvent.position.x - m.x, m.y - mouseEvent.position.y);
		}

		m = mouseEvent.position;
	}

	void operator() (const TouchEvent& touchEvent)
	{
		if (touchEvent.action == TouchActionType::MOVE)
		{
			d = glm::vec2(touchEvent.position.x - m.x, m.y - touchEvent.position.y);
		}

		m = touchEvent.position;
	}
};

class TestApp : public App
{
public:
	TestApp(std::shared_ptr<EngineConfig> config)
		: App(config)
	{
	}

	virtual ~TestApp()
	{
	}

protected:
	void OnEngineInit() override
	{
	}

	void OnSceneInit() override
	{
		std::shared_ptr<IScene> scene = m_engine->GetScene();

		ISceneNode* rootNode = new TestRootSceneNode(scene->GetAllocator(), scene->GetDevice(), scene->GetRenderPass());

		// add all other scene nodes here or inside?

		scene->SetSceneRoot(rootNode);
	}

	void OnSceneGraphInit() override
	{
	}
};


int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

	auto config = std::make_shared<EngineConfig>();
	TestApp app(config);
	app.Init();
	app.Run();
	app.ShutDown();

	return 0;
}
