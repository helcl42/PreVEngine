#include <iostream>
#include <random>
#include <memory>
#include <cstring>
#include <array>

#include <App.h>
#include <Image.h>
#include <Shader.h>
#include <Inputs.h>
#include <Utils.h>

#include "Pipeline.h"

using namespace PreVEngine;

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

// TODO
//template <typename VertexType>
class IMesh
{
public:
	virtual const std::vector<Vertex>& GerVertices() const = 0;

	virtual const std::vector<uint32_t>& GerIndices() const = 0;

	virtual bool HasIndices() const = 0;

public:
	virtual ~IMesh()
	{
	}
};

class IMaterial
{
public:
	virtual std::shared_ptr<Image> GetImage() const = 0;

	virtual std::shared_ptr<ImageBuffer> GetImageBuffer() const = 0;

	virtual bool HasImage() const = 0;

public:
	virtual ~IMaterial() {}
};

class IModel
{
public:
	virtual std::shared_ptr<IMesh> GetMesh() const = 0;

	virtual std::shared_ptr<VBO> GetVertexBuffer() const = 0;

	virtual std::shared_ptr<IBO> GetIndexBuffer() const = 0;

public:
	virtual ~IModel() {}
};

class IRenderComponent
{
public:
	virtual std::shared_ptr<IModel> GetModel() const = 0;

	virtual std::shared_ptr<IMaterial> GetMaterial() const = 0;

public:
	virtual ~IRenderComponent() {}
};

class CubeMesh : public IMesh
{
private:
	const std::vector<Vertex> m_vertices = {
		// FROMT
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },

		// BACK
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
		{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
		{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
		{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },

		// TOP
		{ { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },

		// BOTTOM
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },

		// LEFT
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
		{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
		{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },

		// RIGHT
		{ { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }
	};

	const std::vector<uint32_t> m_indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

public:
	virtual const std::vector<Vertex>& GerVertices() const override
	{
		return m_vertices;
	}

	const std::vector<uint32_t>& GerIndices() const override
	{
		return m_indices;
	}

	bool HasIndices() const override
	{
		return true;
	}
};

class CubeMaterial : public IMaterial
{
private:
	std::shared_ptr<Image> m_image;

	std::shared_ptr<ImageBuffer> m_imageBuffer;

public:
	CubeMaterial(std::shared_ptr<Image> image, std::shared_ptr<ImageBuffer> imageBuffer)
		: m_image(image), m_imageBuffer(imageBuffer)
	{
	}

	virtual ~CubeMaterial()
	{
	}

public:
	std::shared_ptr<Image> GetImage() const override
	{
		return m_image;
	}

	std::shared_ptr<ImageBuffer> GetImageBuffer() const override
	{
		return m_imageBuffer;
	}

	bool HasImage() const override
	{
		return true;
	}
};

class CubeModel : public IModel
{
private:
	std::shared_ptr<IMesh> m_mesh;

	std::shared_ptr<VBO> m_vbo;

	std::shared_ptr<IBO> m_ibo;

public:
	CubeModel(std::shared_ptr<IMesh> mesh, std::shared_ptr<VBO> vbo, std::shared_ptr<IBO> ibo)
		: m_mesh(mesh), m_vbo(vbo), m_ibo(ibo)
	{
	}

	virtual ~CubeModel()
	{
	}

public:
	std::shared_ptr<IMesh> GetMesh() const override
	{
		return m_mesh;
	}

	std::shared_ptr<VBO> GetVertexBuffer() const override
	{
		return m_vbo;
	}

	std::shared_ptr<IBO> GetIndexBuffer() const override
	{
		return m_ibo;
	}
};

class CubeRenderComponent : public IRenderComponent
{
private:
	std::shared_ptr<IModel> m_model;

	std::shared_ptr<IMaterial> m_material;

public:
	CubeRenderComponent(std::shared_ptr<IModel> model, std::shared_ptr<IMaterial> material)
		: m_model(model), m_material(material)
	{
	}

	virtual ~CubeRenderComponent()
	{
	}

public:
	std::shared_ptr<IModel> GetModel() const override
	{
		return m_model;
	}

	std::shared_ptr<IMaterial> GetMaterial() const override
	{
		return m_material;
	}
};

class RenderComponentFactory
{
private:
	static std::map<std::string, std::shared_ptr<Image>> s_imagesCache;

public:
	std::shared_ptr<IRenderComponent> CreateCubeRenderComponent(Allocator& allocator, const std::string& textureFilename)
	{
		// image
		std::shared_ptr<Image> image;
		if(s_imagesCache.find(textureFilename) != s_imagesCache.cend())
		{
			image = s_imagesCache[textureFilename];
		}
		else
		{
			ImageFactory imageFactory;
			image = imageFactory.CreateImage(textureFilename);
			s_imagesCache[textureFilename] = image;
		}

		const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

		std::shared_ptr<ImageBuffer> imageBuffer = std::make_shared<ImageBuffer>(allocator);
		imageBuffer->Create(ImageBufferCreateInfo{ imageExtent, VK_FORMAT_R8G8B8A8_UNORM, true, image->GetBuffer() });

		std::shared_ptr<IMaterial> material = std::make_shared<CubeMaterial>(image, imageBuffer);

		// mesh
		std::shared_ptr<IMesh> mesh = std::make_shared<CubeMesh>();

		// model
		std::shared_ptr<VBO> vertexBuffer = std::make_shared<VBO>(allocator);
		vertexBuffer->Data((void*)mesh->GerVertices().data(), (uint32_t)mesh->GerVertices().size(), sizeof(Vertex));

		std::shared_ptr<IBO> indexBuffer = std::make_shared<IBO>(allocator);
		indexBuffer->Data(mesh->GerIndices().data(), (uint32_t)mesh->GerIndices().size());

		std::shared_ptr<IModel> model = std::make_shared<CubeModel>(mesh, vertexBuffer, indexBuffer);

		// render componene
		std::shared_ptr<IRenderComponent> cubeComponent = std::make_shared<CubeRenderComponent>(model, material);

		return cubeComponent;
	}
};

std::map<std::string, std::shared_ptr<Image>> RenderComponentFactory::s_imagesCache;

template <typename ItemType>
class ComponentRepository final : public Singleton<ComponentRepository<ItemType>>
{
private:
	friend class Singleton<ComponentRepository<ItemType>>;

private:
	std::map<uint64_t, std::shared_ptr<ItemType>> m_components;

private:
	ComponentRepository()
	{
	}

public:
	~ComponentRepository()
	{
	}

public:
	std::shared_ptr<ItemType> Get(const uint64_t id) const
	{
		if (!Contains(id))
		{
			throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
		}

		return m_components.at(id);
	}

	void Add(const uint64_t id, std::shared_ptr<ItemType> component)
	{
		if (Contains(id))
		{
			throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " already exist in this repository.");
		}

		m_components[id] = component;
	}

	void Remove(const uint64_t id)
	{
		if (!Contains(id))
		{
			throw std::runtime_error("Entitity sith id = " + std::to_string(id) + " does not exist in this repository.");
		}

		m_components.erase(id);
	}

	bool Contains(const uint64_t id) const
	{
		return m_components.find(id) != m_components.cend();
	}
};

class AbstractCubeRobotSceneNode : public AbstractSceneNode
{
protected:
	std::shared_ptr<Allocator> m_allocator;

protected:
	glm::vec3 m_position;

	glm::quat m_orientation;

	const std::string m_texturePath;

public:
	AbstractCubeRobotSceneNode(std::shared_ptr<Allocator>& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
		: AbstractSceneNode(), m_allocator(allocator), m_position(position), m_orientation(orientation), m_texturePath(texturePath)
	{
		m_scaler = scale;
	}

	virtual ~AbstractCubeRobotSceneNode()
	{
	}

public:
	void Init() override
	{
		m_transform = MathUtil::CreateTransformationMatrix(m_position, m_orientation, glm::vec3(1, 1, 1));

		RenderComponentFactory renderComponentFactory;
		auto cubeComponent = renderComponentFactory.CreateCubeRenderComponent(*m_allocator, m_texturePath);

		ComponentRepository<IRenderComponent>::GetInstance().Add(m_id, cubeComponent);

		AbstractSceneNode::Init();
	}

	void Update(float deltaTime) override
	{
		AbstractSceneNode::Update(deltaTime);
	}

	void ShutDown() override
	{
		ComponentRepository<IRenderComponent>::GetInstance().Remove(m_id);

		AbstractSceneNode::ShutDown();
	}
};

class CubeRobotPart : public AbstractCubeRobotSceneNode
{
public:
	CubeRobotPart(std::shared_ptr<Allocator>& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
		: AbstractCubeRobotSceneNode(allocator, position, orientation, scale, texturePath)
	{
	}

	virtual ~CubeRobotPart()
	{
	}
};

class CubeRobot : public AbstractCubeRobotSceneNode
{
private:
	std::shared_ptr<CubeRobotPart> m_body;

	std::shared_ptr<CubeRobotPart> m_head;

	std::shared_ptr<CubeRobotPart> m_leftArm;

	std::shared_ptr<CubeRobotPart> m_rightArm;

	std::shared_ptr<CubeRobotPart> m_leftLeg;

	std::shared_ptr<CubeRobotPart> m_rightLeg;

private:
	EventHandler<CubeRobot, KeyEvent> m_keyEvent{ *this };

	EventHandler<CubeRobot, MouseEvent> m_mouseEvent{ *this };

	EventHandler<CubeRobot, TouchEvent> m_touchEvent{ *this };

	glm::vec2 m_angularVelocity{ 0.1f, 0.1f };

	glm::vec2 m_prevMousePosition{ 0.0f, 0.0f };

public:
	CubeRobot(std::shared_ptr<Allocator>& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
		: AbstractCubeRobotSceneNode(allocator, position, orientation, scale, texturePath)
	{
	}

	virtual ~CubeRobot()
	{
	}

public:
	void Init() override
	{
		m_body = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(0, 35, 0), glm::quat(1, 0, 0, 0), glm::vec3(10, 15, 5), "vulkan.png");

		m_head = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(0, 10, 0), glm::quat(1, 0, 0, 0), glm::vec3(5, 5, 5), "texture.jpg");
		m_leftArm = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(-8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), "texture.jpg");
		m_rightArm = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), "texture.jpg");
		m_leftLeg = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(-4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), "texture.jpg");
		m_rightLeg = std::make_shared<CubeRobotPart>(m_allocator, glm::vec3(4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), "texture.jpg");

		m_body->AddChild(m_head);
		m_body->AddChild(m_leftArm);
		m_body->AddChild(m_rightArm);
		m_body->AddChild(m_leftLeg);
		m_body->AddChild(m_rightLeg);

		AddChild(m_body);

		AbstractCubeRobotSceneNode::Init();
	}

	void Update(float deltaTime) override
	{
		auto bodyTransform = m_body->GetTransform();
		bodyTransform = glm::rotate(bodyTransform, glm::radians(m_angularVelocity.x), glm::vec3(1.0f, 0.0f, 0.0f));
		bodyTransform = glm::rotate(bodyTransform, glm::radians(m_angularVelocity.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_body->SetTransform(bodyTransform);

		auto headTransform = m_head->GetTransform();
		headTransform = glm::rotate(headTransform, -glm::radians(25.0f) * deltaTime, glm::vec3(0, 1, 0));
		m_head->SetTransform(headTransform);

		auto leftArmTransform = m_leftArm->GetTransform();
		leftArmTransform = glm::translate(leftArmTransform, glm::vec3(0, -4.5, 0));
		leftArmTransform = leftArmTransform * glm::rotate(glm::mat4(1.0f), glm::radians(20.0f) * deltaTime, glm::vec3(1, 0, 0));
		leftArmTransform = glm::translate(leftArmTransform, glm::vec3(0, 4.5, 0));
		m_leftArm->SetTransform(leftArmTransform);

		AbstractCubeRobotSceneNode::Update(deltaTime);
	}

public:
	void operator() (const KeyEvent& keyEvent)
	{
		if (keyEvent.action == KeyActionType::PRESS)
		{
			if (keyEvent.keyCode == KeyCode::KEY_Left)
			{
				m_angularVelocity.y -= 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Right)
			{
				m_angularVelocity.y += 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Up)
			{
				m_angularVelocity.x += 0.1f;
			}

			if (keyEvent.keyCode == KeyCode::KEY_Down)
			{
				m_angularVelocity.x -= 0.1f;
			}
		}
	}

	void operator() (const MouseEvent& mouseEvent)
	{
		if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::RIGHT)
		{
			m_angularVelocity = glm::vec2(mouseEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - mouseEvent.position.y);
		}

		m_prevMousePosition = mouseEvent.position;
	}

	void operator() (const TouchEvent& touchEvent)
	{
		if (touchEvent.action == TouchActionType::MOVE)
		{
			m_angularVelocity = glm::vec2(touchEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - touchEvent.position.y);
		}

		m_prevMousePosition = touchEvent.position;
	}
};

class IRenderer
{
public:
	virtual void Init() = 0;

	virtual void PreRender(RenderContext& renderContext) = 0;

	virtual void Render(RenderContext& renderContext, std::shared_ptr<ISceneNode> node) = 0;

	virtual void PostRender(RenderContext& renderContext) = 0;

	virtual void ShutDown() = 0;

public:
	virtual ~IRenderer() = default;
};


template <typename ItemType>
class UBOPool
{
private:
	std::shared_ptr<Allocator> m_allocator;

	std::vector<std::shared_ptr<UBO>> m_uniformBuffers;

	uint32_t m_index = 0;

public:
	UBOPool(std::shared_ptr<Allocator> allocator)
		: m_allocator(allocator)
	{
	}

	virtual ~UBOPool()
	{
	}

public:
	void AdjustCapactity(uint32_t capacity)
	{
		m_index = 0;
		m_uniformBuffers.clear();

		for (uint32_t i = 0; i < capacity; i++)
		{
			auto ubo = std::make_shared<UBO>(*m_allocator);
			ubo->Allocate(sizeof(ItemType));
			m_uniformBuffers.emplace_back(ubo);
		}
	}

	std::shared_ptr<UBO> GetNext()
	{
		m_index = (m_index + 1) % m_uniformBuffers.size();
		return m_uniformBuffers.at(m_index);
	}
};

class ViewFrustum
{
private:
	float m_fov;

	float m_nearClippingPlane;

	float m_farClippingPlane;

public:
	ViewFrustum(float fov, float nCp, float fCp)
		: m_fov(fov), m_nearClippingPlane(nCp), m_farClippingPlane(fCp)
	{
	}

	~ViewFrustum()
	{
	}

public:
	glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h)
	{
		glm::mat4 projectionMatrix = glm::perspective(m_fov, static_cast<float>(w) / static_cast<float>(h), m_nearClippingPlane, m_farClippingPlane);
		projectionMatrix[1][1] *= -1; // invert Y in clip coordinates

		return projectionMatrix;
	}
};

class Camera : public AbstractSceneNode
{
private:
	EventHandler<Camera, MouseEvent> m_mouseHandler{ *this };

	EventHandler<Camera, TouchEvent> m_touchHandler{ *this };

	EventHandler<Camera, MouseScrollEvent> m_mouseScrollHandler{ *this };

	EventHandler<Camera, KeyEvent> m_keyHandler{ *this };

private:
	InputsFacade m_inputFacade;

private:
	const glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

	const float m_sensitivity = 0.05f;

	const float m_scrollSensitivity = 2.0f;

	const float m_moveSpeed = 5.0f;

private:
	glm::vec3 m_position;

	glm::vec3 m_positionDelta;

	glm::vec3 m_forwardDirection;

	glm::vec3 m_rightDirection;

	glm::vec3 m_pitchYawRoll;

	glm::vec3 m_pitchYawRollDelta;

	glm::mat4 m_viewMatrix;


	glm::vec2 m_prevTouchPosition;

public:
	Camera()
	{
		Reset();

		m_inputFacade.SetMouseLocked(true);
		m_inputFacade.SetMouseCursorVisible(false);
	}

	virtual ~Camera()
	{
	}

private:
	float NormalizeUpTo2Phi(float val)
	{
		const float TWO_PHI_IN_DEGS = 360.0f;
		if (val > TWO_PHI_IN_DEGS)
		{
			val -= TWO_PHI_IN_DEGS;
		}
		else if (val < -TWO_PHI_IN_DEGS)
		{
			val += TWO_PHI_IN_DEGS;
		}
		return val;
	}

	void UpdatePosition(float deltaTime)
	{
		m_position += m_positionDelta;

		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_W))
		{
			m_position += m_forwardDirection * deltaTime * m_moveSpeed;
		}
		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_S))
		{
			m_position -= m_forwardDirection * deltaTime * m_moveSpeed;
		}
		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_A))
		{
			m_position -= m_rightDirection * deltaTime * m_moveSpeed;
		}
		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_D))
		{
			m_position += m_rightDirection * deltaTime * m_moveSpeed;
		}
		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_Q))
		{
			m_position -= m_upDirection * deltaTime * m_moveSpeed;
		}
		if (m_inputFacade.IsKeyPressed(KeyCode::KEY_E))
		{
			m_position += m_upDirection * deltaTime * m_moveSpeed;
		}

		m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void UpdateOrientation(float deltaTime)
	{
		//compute quaternion for pitch based on the camera pitch angle
		glm::quat pitchQuat = glm::angleAxis(glm::radians(m_pitchYawRollDelta.x), m_rightDirection);

		//determine heading quaternion from the camera up vector and the heading angle
		glm::quat headingQuat = glm::angleAxis(glm::radians(m_pitchYawRollDelta.y), m_upDirection);

		//add the two quaternions
		glm::quat orientation = glm::normalize(pitchQuat * headingQuat);

		// update forward direction from the quaternion
		m_forwardDirection = glm::normalize(orientation * m_forwardDirection);

		// compute right direction from up and formward
		m_rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));

		// reset current iteration deltas
		m_pitchYawRollDelta = glm::vec3(0.0f, 0.0f, 0.0f);
	}

public:
	void Update(float deltaTime)
	{
		UpdatePosition(deltaTime);
		UpdateOrientation(deltaTime);

		m_viewMatrix = glm::lookAt(m_position, m_position + m_forwardDirection, m_upDirection);

		SetTransform(glm::inverse(m_viewMatrix));

		AbstractSceneNode::Update(deltaTime);
	}

	const glm::mat4& LookAt() const
	{
		return m_viewMatrix;
	}

	void Reset()
	{
		std::cout << "Resseting camera.." << std::endl;

		m_position = glm::vec3(0.0f, 0.0f, 60.0f);
		m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);

		m_pitchYawRoll = glm::vec3(0.0f, 0.0f, 0.0f);
		m_pitchYawRollDelta = glm::vec3(0.0f, 0.0f, 0.0f);
		
		m_viewMatrix = glm::mat4(1.0f);

		m_forwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		m_rightDirection = glm::cross(m_forwardDirection, m_upDirection);

		m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
	}

	void AddPitch(float amountInDegrees)
	{
		float newFinalPitch = m_pitchYawRoll.x + amountInDegrees;
		if (newFinalPitch > 89.0f || newFinalPitch < -89.0f)
		{
			return;
		}

		m_pitchYawRollDelta.x += amountInDegrees;
		m_pitchYawRoll.x += amountInDegrees;
	}

	void AddYaw(float amountInDegrees)
	{
		m_pitchYawRollDelta.y += amountInDegrees;
		m_pitchYawRoll.y += amountInDegrees;

		m_pitchYawRollDelta.y = NormalizeUpTo2Phi(m_pitchYawRollDelta.y);
	}

public:
	void operator() (const MouseEvent& mouseEvent)
	{
		if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::LEFT)
		{
			const glm::vec2 angleInDegrees = mouseEvent.position * m_sensitivity;

			AddPitch(angleInDegrees.y);
			AddYaw(angleInDegrees.x);
		}
	}

	void operator() (const TouchEvent& touchEvent)
	{
		if (touchEvent.action == TouchActionType::MOVE)
		{
			const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * m_sensitivity;

			AddPitch(angleInDegrees.y);
			AddYaw(angleInDegrees.x);
		}


		m_prevTouchPosition = touchEvent.position;
	}

	void operator() (const KeyEvent& keyEvent)
	{
		if (keyEvent.action == KeyActionType::PRESS)
		{
			if (keyEvent.keyCode == KeyCode::KEY_R)
			{
				Reset();
			}
			else if (keyEvent.keyCode == KeyCode::KEY_L)
			{
				m_inputFacade.SetMouseLocked(!m_inputFacade.IsMouseLocked());
				m_inputFacade.SetMouseCursorVisible(!m_inputFacade.IsMouseCursorVisible());
			}
		}
	}

	void operator() (const MouseScrollEvent& scrollEvent)
	{
		float scaledDelta = scrollEvent.delta * m_scrollSensitivity;
		m_positionDelta = m_forwardDirection * scaledDelta;
	}
};

class TestNodesRenderer : public IRenderer
{
private:
	struct Uniforms
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<Pipeline> m_pipeline;

	std::shared_ptr<UBOPool<Uniforms>> m_uniformsPool;

	std::shared_ptr<Camera> m_freeCamera;

	ViewFrustum m_viewFrustum{ 70.0f, 0.01f, 200.0f };

public:
	TestNodesRenderer(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass, std::shared_ptr<Camera> camera)
		: m_device(dev), m_renderPass(renderPass), m_allocator(alloc), m_freeCamera(camera)
	{
	}

	virtual ~TestNodesRenderer()
	{
	}

public:
	void Init() override
	{
		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(*m_device, {
			{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/vert.spv" },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/frag.spv" }
		});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		printf("Shader created\n");

		m_pipeline = std::make_shared<Pipeline>(*m_device, *m_renderPass, *m_shader);
		m_pipeline->CreateGraphicsPipeline();

		printf("Pipeline created\n");

		m_uniformsPool = std::make_shared<UBOPool<Uniforms>>(m_allocator);
		m_uniformsPool->AdjustCapactity(10000);
	}

	void PreRender(RenderContext& renderContext) override
	{
		VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
		VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

		vkCmdBindPipeline(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		vkCmdSetViewport(renderContext.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderContext.commandBuffer, 0, 1, &scissor);
	}

	void Render(RenderContext& renderContext, std::shared_ptr<ISceneNode> node) override
	{
		if (ComponentRepository<IRenderComponent>::GetInstance().Contains(node->GetId()))
		{
			auto renderComponent = ComponentRepository<IRenderComponent>::GetInstance().Get(node->GetId());

			auto ubo = m_uniformsPool->GetNext();

			Uniforms uniforms;
			uniforms.proj = m_viewFrustum.CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height);
			uniforms.view = m_freeCamera->LookAt();
			uniforms.model = node->GetWorldTransformScaled();
			ubo->Update(&uniforms);

			m_shader->Bind("texSampler", *renderComponent->GetMaterial()->GetImageBuffer());
			m_shader->Bind("ubo", *ubo);

			VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
			VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(renderContext.commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(renderContext.commandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
			vkCmdBindDescriptorSets(renderContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

			vkCmdDrawIndexed(renderContext.commandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
		}

		for (auto child : node->GetChildren())
		{
			Render(renderContext, child);
		}
	}

	void PostRender(RenderContext& renderContext) override
	{
	}

	void ShutDown() override
	{
	}
};

class RootSceneNode : public AbstractSceneNode
{
private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<IRenderer> m_renderer;

	std::shared_ptr<Camera> m_freeCamera;

public:
	RootSceneNode(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass)
		: AbstractSceneNode(), m_device(dev), m_renderPass(renderPass), m_allocator(alloc)
	{
	}

	virtual ~RootSceneNode()
	{
	}

public:
	void Init() override
	{
		m_freeCamera = std::make_shared<Camera>();

		auto camRobot = std::make_shared<CubeRobot>(m_allocator, glm::vec3(1.0f, -0.4f, -1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), "texture.jpg");
		m_freeCamera->AddChild(camRobot);

		AddChild(m_freeCamera);

		m_renderer = std::make_shared<TestNodesRenderer>(m_allocator, m_device, m_renderPass, m_freeCamera);
		m_renderer->Init();

		const int32_t CUBE_SIZE_HALF = 1;
		const float DISTANCE = 40.0f;

		for (int32_t i = -CUBE_SIZE_HALF; i <= CUBE_SIZE_HALF; i++)
		{
			for (int32_t j = -CUBE_SIZE_HALF; j <= CUBE_SIZE_HALF; j++)
			{
				for (int32_t k = -CUBE_SIZE_HALF; k <= CUBE_SIZE_HALF; k++)
				{
					auto robot = std::make_shared<CubeRobot>(m_allocator, glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), "texture.jpg");
					AddChild(robot);
				}
			}
		}

		for (auto child : m_children)
		{
			child->Init();
		}
	}

	void Update(float deltaTime) override
	{
		for (auto child : m_children)
		{
			child->Update(deltaTime);
		}
	}

	void Render(RenderContext& renderState) override
	{
		m_renderer->PreRender(renderState);

		for (auto child : m_children)
		{
			m_renderer->Render(renderState, child);
		}

		m_renderer->PostRender(renderState);
	}

	void ShutDown() override
	{
		for (auto child : m_children)
		{
			child->ShutDown();
		}

		m_renderer->ShutDown();
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

		auto rootNode = std::make_shared<RootSceneNode>(scene->GetAllocator(), scene->GetDevice(), scene->GetRenderPass());

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
