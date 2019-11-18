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

//////////////////////////////////////
// Window
//////////////////////////////////////
struct WindowCreatedEvent
{
	IWindow* window;
};

struct WindowDestroyedEvent
{
	IWindow* window;
};

struct WindowResizeEvent
{
	IWindow* window;

	uint32_t width;

	uint32_t height;
};

struct WindowMovedEvent
{
	IWindow* window;

	glm::vec2 position;
};

struct WindowFocusChangeEvent
{
	IWindow* window;

	bool hasFocus;
};
//////////////////////////////////////


//////////////////////////////////////
// Keyboard
//////////////////////////////////////
enum class KeyActionType : uint32_t
{
	PRESS,
	RELEASE
};

struct KeyEvent
{
	KeyActionType action;

	KeyCode keyCode;
};
//////////////////////////////////////


//////////////////////////////////////
// Mouse
//////////////////////////////////////
enum class MouseActionType : uint32_t
{
	PRESS,
	RELEASE,
	MOVE
};

enum class MouseButtonType : uint32_t
{
	NONE = 0,
	LEFT,
	MIDDLE,
	RIGHT
};

struct MouseEvent
{
	MouseActionType action;

	MouseButtonType button;

	glm::vec2 position;
};

struct MouseScrollEvent
{
	int32_t delta;

	glm::vec2 position;
};
//////////////////////////////////////


//////////////////////////////////////
// Touch
//////////////////////////////////////
enum class TouchActionType
{
	DOWN,
	UP,
	MOVE
};

struct TouchEvent
{
	TouchActionType action;
	
	uint8_t pointerId;

	glm::vec2 position;
};
//////////////////////////////////////


//////////////////////////////////////
// Text
//////////////////////////////////////
struct TextEvent
{
	std::string text;
};
//////////////////////////////////////

class InputsMapping
{
public:
	static KeyActionType GetKeyActionType(const ActionType action)
	{
		switch (action)
		{
			case ActionType::DOWN:
				return KeyActionType::PRESS;
			case ActionType::UP:
				return KeyActionType::RELEASE;
			default:
				throw std::runtime_error("Invalid key action");
		}
	}

	static MouseActionType GetMouseActionType(const ActionType action)
	{
		switch (action)
		{
			case ActionType::DOWN:
				return MouseActionType::PRESS;
			case ActionType::UP:
				return MouseActionType::RELEASE;
			case ActionType::MOVE:
				return MouseActionType::MOVE;
			default:
				throw std::runtime_error("Invalid mouse button action");
		}
	}

	static MouseButtonType GetMouseButtonType(const ButtonType button)
	{
		switch (button)
		{
			case ButtonType::NONE:
				return MouseButtonType::NONE;
			case ButtonType::LEFT:
				return MouseButtonType::LEFT;
			case ButtonType::MIDDLE:
				return MouseButtonType::MIDDLE;
			case ButtonType::RIGHT:
				return MouseButtonType::RIGHT;
			default:
				throw std::runtime_error("Invalid mouse button type");
		}
	}

	static TouchActionType GetTouchActionType(const ActionType action)
	{
		switch (action)
		{
			case ActionType::DOWN:
				return TouchActionType::DOWN;
			case ActionType::UP:
				return TouchActionType::UP;
			case ActionType::MOVE:
				return TouchActionType::MOVE;
			default:
				throw std::runtime_error("Invalid touch action");
		}
	}
};


class IMouseActionListener
{
public:
	virtual void OnMouseAction(const MouseEvent& mouseAction) = 0;

public:
	virtual ~IMouseActionListener() = default;
};

class IMouseScrollListener
{
public:
	virtual void OnMouseScroll(const MouseScrollEvent& scroll) = 0;

public:
	virtual ~IMouseScrollListener() = default;
};

class MouseInputComponent final
{
private:
	EventHandler<MouseInputComponent, MouseEvent> m_mouseActionsHandler{ *this };

	EventHandler<MouseInputComponent, MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
	Observer<IMouseActionListener> m_mouseActionObservers;

	Observer<IMouseScrollListener> m_mouseScrollObservers;

private:
	mutable std::mutex m_mutex;

	std::set<MouseButtonType> m_pressedButtons;

	glm::vec2 m_mousePosition{ 0, 0 };

public:
	MouseInputComponent() = default;

	~MouseInputComponent() = default;

	MouseInputComponent(const MouseInputComponent& other) = default;

	MouseInputComponent& operator=(const MouseInputComponent& other) = default;

	MouseInputComponent(MouseInputComponent&& other) = default;

	MouseInputComponent& operator=(MouseInputComponent&& other) = default;

public:
	bool RegisterMouseActionListener(IMouseActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseActionObservers.RegisterListener(listener);
	}

	bool UnregisterMouseActionListener(IMouseActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseActionObservers.UnregisterListener(listener);
	}

	bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseActionObservers.IsListenerRegistered(listener);
	}

	bool RegisterMouseScrollListener(IMouseScrollListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseScrollObservers.RegisterListener(listener);
	}

	bool UnregisterMouseScrollListener(IMouseScrollListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseScrollObservers.UnregisterListener(listener);
	}

	bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mouseScrollObservers.IsListenerRegistered(listener);
	}

public:
	const std::set<MouseButtonType>& GetPressedButtons() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_pressedButtons;
	}

	glm::vec2 GetMousePosition() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_mousePosition;
	}

	bool IsButtonPressed(const MouseButtonType button) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_pressedButtons.find(button) != m_pressedButtons.cend();
	}

public:
	void operator() (const MouseEvent& action)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_mousePosition = action.position;

		if (action.action == MouseActionType::PRESS)
		{
			m_pressedButtons.insert(action.button);
		}
		else if (action.action == MouseActionType::RELEASE)
		{
			m_pressedButtons.erase(action.button);
		}

		for (auto listener : m_mouseActionObservers.GetObservers())
		{
			listener->OnMouseAction(action);
		}
	}

	void operator() (const MouseScrollEvent& scrollAction)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto& listener : m_mouseScrollObservers.GetObservers())
		{
			listener->OnMouseScroll(scrollAction);
		}
	}
};


class IKeyboardActionListener
{
public:
	virtual void OnKeyAction(const KeyEvent& keyEvent) = 0;

public:
	virtual ~IKeyboardActionListener() = default;
};

class ITextListener
{
public:
	virtual void OnText(const TextEvent& textEvent) = 0;

public:
	virtual ~ITextListener() = default;
};

class KeyboardInputComponnet final
{
private:
	EventHandler<KeyboardInputComponnet, KeyEvent> m_keyEventsHandler{ *this };

	EventHandler<KeyboardInputComponnet, TextEvent> m_textEventHandler{ *this };

private:
	Observer<IKeyboardActionListener> m_keyActionObservers;

	Observer<ITextListener> m_textObservers;

private:
	mutable std::mutex m_mutex;

	std::set<KeyCode> m_pressedKeys;

public:
	KeyboardInputComponnet() = default;

	~KeyboardInputComponnet() = default;

	KeyboardInputComponnet(const KeyboardInputComponnet& other) = default;

	KeyboardInputComponnet& operator=(const KeyboardInputComponnet& other) = default;

	KeyboardInputComponnet(KeyboardInputComponnet&& other) = default;

	KeyboardInputComponnet& operator=(KeyboardInputComponnet&& other) = default;

public:
	bool RegisterKeyboardActionListener(IKeyboardActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_keyActionObservers.RegisterListener(listener);
	}

	bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_keyActionObservers.UnregisterListener(listener);
	}

	bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_keyActionObservers.IsListenerRegistered(listener);
	}

	bool RegisterTextListener(ITextListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_textObservers.RegisterListener(listener);
	}

	bool UnregisterTextListener(ITextListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_textObservers.UnregisterListener(listener);
	}

	bool IsTextListenerRegistered(ITextListener& listener) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_textObservers.IsListenerRegistered(listener);
	}

public:
	bool IsKeyPressed(const KeyCode keyCode) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_pressedKeys.find(keyCode) != m_pressedKeys.cend();
	}

	const std::set<KeyCode> GetPressedKeys() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_pressedKeys;
	}

public:
	void operator() (const KeyEvent& keyEvent)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (keyEvent.action == KeyActionType::PRESS)
		{
			m_pressedKeys.insert(keyEvent.keyCode);
		}
		else if (keyEvent.action == KeyActionType::RELEASE)
		{
			m_pressedKeys.erase(keyEvent.keyCode);
		}

		for (auto& listener : m_keyActionObservers.GetObservers())
		{
			listener->OnKeyAction(keyEvent);
		}
	}

	void operator() (const TextEvent& textEvent)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto& listener : m_textObservers.GetObservers())
		{
			listener->OnText(textEvent);
		}
	}
};

struct Touch
{
	uint8_t pointerId;

	glm::vec2 position;
};

class ITouchActionListener
{
public:
	virtual void OnTouchAction(const TouchEvent& textEvent) = 0;

public:
	virtual ~ITouchActionListener() = default;
};

class TouchInputComponent final
{
private:
	EventHandler<TouchInputComponent, TouchEvent> m_touchEventsHandler{ *this };

private:
	Observer<ITouchActionListener> m_touchObservers;

private:
	mutable std::mutex m_mutex;

	std::map<uint8_t, Touch> m_touchedDownPointers;

public:
	TouchInputComponent() = default;

	~TouchInputComponent() = default;

	TouchInputComponent(const TouchInputComponent& other) = default;

	TouchInputComponent& operator=(const TouchInputComponent& other) = default;

	TouchInputComponent(TouchInputComponent&& other) = default;

	TouchInputComponent& operator=(TouchInputComponent&& other) = default;

public:
	bool RegisterTouchActionListener(ITouchActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_touchObservers.RegisterListener(listener);
	}

	bool UnregisterTouchActionListener(ITouchActionListener& listener)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_touchObservers.UnregisterListener(listener);
	}

	bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_touchObservers.IsListenerRegistered(listener);
	}

public:
	std::map<uint8_t, Touch> GetTouches() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_touchedDownPointers;
	}

	bool IsPointerTouched(const uint8_t pointerId) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_touchedDownPointers.find(pointerId) != m_touchedDownPointers.cend();
	}

public:
	void operator() (const TouchEvent& touchEvent)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (touchEvent.action == TouchActionType::DOWN)
		{
			m_touchedDownPointers[touchEvent.pointerId] = Touch{ touchEvent.pointerId, touchEvent.position };
		}
		else if (touchEvent.action == TouchActionType::UP)
		{
			m_touchedDownPointers.erase(touchEvent.pointerId);
		}

		for (auto& listener : m_touchObservers.GetObservers())
		{
			listener->OnTouchAction(touchEvent);
		}
	}
};

class InputsFacade
{
private:
	KeyboardInputComponnet m_keyboardInputComponent;

	MouseInputComponent m_mouseInputComponent;

	TouchInputComponent m_touchInuptComponent;

public:
	InputsFacade() = default;

	~InputsFacade() = default;

	InputsFacade(const InputsFacade& other) = default;

	InputsFacade& operator=(const InputsFacade& other) = default;

	InputsFacade(InputsFacade&& other) = default;

	InputsFacade& operator=(InputsFacade&& other) = default;

public:
	bool RegisterKeyboardActionListener(IKeyboardActionListener& listener)
	{
		return m_keyboardInputComponent.RegisterKeyboardActionListener(listener);
	}

	bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
	{
		return m_keyboardInputComponent.UnregisterKeyboardActionListener(listener);
	}

	bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
	{
		return m_keyboardInputComponent.IsKeyboardActionListenerRegistered(listener);
	}

	bool RegisterTextListener(ITextListener& listener)
	{
		return m_keyboardInputComponent.RegisterTextListener(listener);
	}

	bool UnregisterTextListener(ITextListener& listener)
	{
		return m_keyboardInputComponent.UnregisterTextListener(listener);
	}

	bool IsTextListenerRegistered(ITextListener& listener) const
	{
		return m_keyboardInputComponent.IsTextListenerRegistered(listener);
	}

	bool RegisterMouseActionListener(IMouseActionListener& listener)
	{
		return m_mouseInputComponent.RegisterMouseActionListener(listener);
	}

	bool UnregisterMouseActionListener(IMouseActionListener& listener)
	{
		return m_mouseInputComponent.UnregisterMouseActionListener(listener);
	}

	bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const
	{
		return m_mouseInputComponent.IsMouseActionListenerRegistered(listener);
	}

	bool RegisterMouseScrollListener(IMouseScrollListener& listener)
	{
		return m_mouseInputComponent.RegisterMouseScrollListener(listener);
	}

	bool UnregisterMouseScrollListener(IMouseScrollListener& listener)
	{
		return m_mouseInputComponent.UnregisterMouseScrollListener(listener);
	}

	bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const
	{
		return m_mouseInputComponent.IsMouseScrollListenerRegistered(listener);
	}

	bool RegisterTouchActionListener(ITouchActionListener& listener)
	{
		return m_touchInuptComponent.RegisterTouchActionListener(listener);
	}

	bool UnregisterTouchActionListener(ITouchActionListener& listener)
	{
		return m_touchInuptComponent.UnregisterTouchActionListener(listener);
	}

	bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const
	{
		return m_touchInuptComponent.IsTouchActionListenerRegistered(listener);
	}

public:
	bool IsKeyPressed(const KeyCode keyCode) const
	{
		return m_keyboardInputComponent.IsKeyPressed(keyCode);
	}

	const std::set<KeyCode> GetPressedKeys() const
	{
		return m_keyboardInputComponent.GetPressedKeys();
	}

	const std::set<MouseButtonType>& GetPressedButtons() const
	{
		return m_mouseInputComponent.GetPressedButtons();
	}

	glm::vec2 GetMousePosition() const
	{
		return m_mouseInputComponent.GetMousePosition();
	}

	bool IsButtonPressed(const MouseButtonType button) const
	{
		return m_mouseInputComponent.IsButtonPressed(button);
	}

	std::map<uint8_t, Touch> GetTouches() const
	{
		return m_touchInuptComponent.GetTouches();
	}

	bool IsPointerTouched(const uint8_t pointerId) const
	{
		return m_touchInuptComponent.IsPointerTouched(pointerId);
	}
};

class EngineWindow : public Window
{
public:
	EngineWindow(const char* title)
		: Window(title)
	{
	}

	EngineWindow(const char* title, const uint32_t width, const uint32_t height)
		: Window(title, width, height)
	{
	}

public:
	virtual void OnInitEvent()
	{
		EventChannel::Broadcast(WindowCreatedEvent{ this });
	}

	virtual void OnCloseEvent()
	{
		EventChannel::Broadcast(WindowDestroyedEvent{ this });
	}

	virtual void OnResizeEvent(uint16_t width, uint16_t height)
	{
		EventChannel::Broadcast(WindowResizeEvent{ this, width, height });
	}

	virtual void OnMoveEvent(int16_t x, int16_t y)
	{
		EventChannel::Broadcast(WindowMovedEvent{ this, glm::vec2(x, y) });
	}

	virtual void OnFocusEvent(bool hasFocus)
	{
		EventChannel::Broadcast(WindowFocusChangeEvent{ this, hasFocus });
	}

	void OnKeyEvent(ActionType action, KeyCode keyCode) override
	{
		EventChannel::Broadcast(KeyEvent{ InputsMapping::GetKeyActionType(action), keyCode });
	}

	void OnMouseEvent(ActionType action, int16_t x, int16_t y, ButtonType button) override
	{
		EventChannel::Broadcast(MouseEvent{ InputsMapping::GetMouseActionType(action), InputsMapping::GetMouseButtonType(button), glm::vec2(x, y) });
	}

	void OnMouseScrollEvent(int16_t delta, int16_t x, int16_t y) override
	{
		EventChannel::Broadcast(MouseScrollEvent{ delta, glm::vec2(x, y) });
	}

	void OnTouchEvent(ActionType action, float x, float y, uint8_t pointerId) override
	{
		EventChannel::Broadcast(TouchEvent{ InputsMapping::GetTouchActionType(action), pointerId, glm::vec2(x, y)});
	}

	void OnTextEvent(const char *str)
	{
		EventChannel::Broadcast(TextEvent{ str });
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
	EventHandler<Scene, WindowResizeEvent> m_windowResizeEvent{ *this };

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

private:
	// temp bullshit
	EventHandler<Scene, KeyEvent> m_keyEvent{ *this };

	EventHandler<Scene, MouseEvent> m_mouseEvent{ *this };

	EventHandler<Scene, TouchEvent> m_touchEvent{ *this };

	bool shouldAdd = false;

	bool shouldDelete = false;

	glm::vec2 d{ 0.1f, 0.1f };
	
	glm::vec2 m{ 0.0f, 0.0f };

public:
	Scene(Allocator& alloc, Device& dev, Swapchain& swapCh, RenderPass& renderPass)
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

		auto ubo = std::make_shared<UBO>(m_allocator);
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
		// This should be part of Scene InEngine

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

			// ~This should be part of Scene InEngine


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

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, *model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, model->indexBuffer->GetCount(), 1, 0, 0, 0);

				modelIndex++;
			}

			// This should be part of Scene InEngine

			m_swapchain.EndFrame();
		}

		// ~This should be part of Scene InEngine
	}

	void ShutDown()
	{
	}

public:
	void operator() (const WindowResizeEvent& resizeEvent)
	{
		m_swapchain.UpdateExtent();
	}

	void operator() (const KeyEvent& keyEvent)
	{
		if (keyEvent.action == KeyActionType::PRESS)
		{
			if (keyEvent.keyCode == KeyCode::KEY_A)
			{
				shouldAdd = true;
			}

			if (keyEvent.keyCode == KeyCode::KEY_D)
			{
				shouldDelete = true;
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

	std::shared_ptr<EngineWindow> m_window;

	PhysicalDevice *m_physicalDevice;

	std::shared_ptr<Device> m_device;

	Queue* m_presentQueue;

	Queue* m_graphicsQueue;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

	std::shared_ptr<Swapchain> m_swapchain;

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

		if (m_config.fullScreen)
		{
			m_window = std::make_shared<EngineWindow>(m_config.appName.c_str());
		}
		else
		{
			m_window = std::make_shared<EngineWindow>(m_config.appName.c_str(), m_config.windowSize.width, m_config.windowSize.height);
			m_window->SetPosition(m_config.windowPosition);
		}

		auto physicalDevices = std::make_shared<PhysicalDevices>(*m_instance);
		physicalDevices->Print();

		VkSurfaceKHR surface = m_window->GetSurface(*m_instance);
		m_physicalDevice = physicalDevices->FindPresentable(surface);					// get presenting GPU?
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

		m_allocator = std::make_shared<Allocator>(*m_graphicsQueue);                   // Create "Vulkan Memory Aloocator"
		printf("Allocator created\n");

		m_swapchain = std::make_shared<Swapchain>(*m_allocator, *m_renderPass, m_graphicsQueue, m_graphicsQueue);
		m_swapchain->SetPresentMode(m_config.VSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);
		m_swapchain->SetImageCount(m_config.framesInFlight);
		m_swapchain->Print();

		m_scene = std::make_shared<Scene>(*m_allocator, *m_device, *m_swapchain, *m_renderPass);
		m_scene->Init();
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
