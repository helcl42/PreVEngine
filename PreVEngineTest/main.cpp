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

	// TODO add flags 
	// has boTangent data
	// has indices
	// has positions
	// has texture coords

	// TODO create something like dynamic vertexLayout
	//{
	//  map<VertexAttributeIndex, ItemType>
	//}

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
	const std::vector<Vertex>& GerVertices() const override
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

class QuadMesh : public IMesh
{
private:
	const std::vector<Vertex> m_vertices = {
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	};

	const std::vector<uint32_t> m_indices = {
		0, 1, 2, 2, 3, 0
	};

public:
	const std::vector<Vertex>& GerVertices() const override
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

class PlaneMesh : public IMesh
{
private:
	std::vector<Vertex> m_vertices;
	
	std::vector<uint32_t> m_indices;

public:
	PlaneMesh(const float xSize, const float zSize, const uint32_t xDivs, const uint32_t zDivs, const float sMax = 1.0f, const float tMax = 1.0f)
	{
		const float x2 = xSize / 2.0f;
		const float z2 = zSize / 2.0f;
		const float iFactor = (float)zSize / zDivs;
		const float jFactor = (float)xSize / xDivs;
		const float texi = (float)sMax / zDivs;
		const float texj = (float)tMax / xDivs;

		for (uint32_t i = 0; i <= zDivs; i++)
		{
			float z = iFactor * i - z2;

			for (uint32_t j = 0; j <= xDivs; j++)
			{
				float x = jFactor * j - x2;

				glm::vec3 vertex(x, 0.0f, z);
				glm::vec2 textureCoord(j * texi, i * texj);
				glm::vec3 normal(0.0f, 1.0f, 0.0f);

				m_vertices.emplace_back(Vertex{ vertex, textureCoord, normal });
			}
		}

		for (uint32_t i = 0; i < zDivs; i++)
		{
			const uint32_t rowStart = i * (xDivs + 1);
			const uint32_t nextRowStart = (i + 1) * (xDivs + 1);

			for (uint32_t j = 0; j < xDivs; j++)
			{
				const uint32_t indices[] =
				{
					rowStart + j,
					nextRowStart + j,
					nextRowStart + j + 1,
					rowStart + j,
					nextRowStart + j + 1,
					rowStart + j + 1
				};

				for (const auto index : indices)
				{
					m_indices.emplace_back(index);
				}
			}
		}
	}

	virtual ~PlaneMesh()
	{
	}

public:
	const std::vector<Vertex>& GerVertices() const override
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

class Material : public IMaterial
{
private:
	std::shared_ptr<Image> m_image;

	std::shared_ptr<ImageBuffer> m_imageBuffer;

public:
	Material(std::shared_ptr<Image> image, std::shared_ptr<ImageBuffer> imageBuffer)
		: m_image(image), m_imageBuffer(imageBuffer)
	{
	}

	virtual ~Material()
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

class Model : public IModel
{
private:
	std::shared_ptr<IMesh> m_mesh;

	std::shared_ptr<VBO> m_vbo;

	std::shared_ptr<IBO> m_ibo;

public:
	Model(std::shared_ptr<IMesh> mesh, std::shared_ptr<VBO> vbo, std::shared_ptr<IBO> ibo)
		: m_mesh(mesh), m_vbo(vbo), m_ibo(ibo)
	{
	}

	virtual ~Model()
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

class DefaultRenderComponent : public IRenderComponent
{
private:
	std::shared_ptr<IModel> m_model;

	std::shared_ptr<IMaterial> m_material;

public:
	DefaultRenderComponent(std::shared_ptr<IModel> model, std::shared_ptr<IMaterial> material)
		: m_model(model), m_material(material)
	{
	}

	virtual ~DefaultRenderComponent()
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

private:
	std::shared_ptr<Image> CreateImage(const std::string& textureFilename) const
	{
		// image
		std::shared_ptr<Image> image;
		if (s_imagesCache.find(textureFilename) != s_imagesCache.cend())
		{
			image = s_imagesCache[textureFilename];
		}
		else
		{
			ImageFactory imageFactory;
			image = imageFactory.CreateImage(textureFilename);
			s_imagesCache[textureFilename] = image;
		}
		return image;
	}

	std::shared_ptr<ImageBuffer> CreateImageBuffer(Allocator& allocator, const std::shared_ptr<Image> image, const bool repeatAddressMode) const
	{
		const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

		std::shared_ptr<ImageBuffer> imageBuffer = std::make_shared<ImageBuffer>(allocator);
		imageBuffer->Create(ImageBufferCreateInfo{ imageExtent, VK_FORMAT_R8G8B8A8_UNORM, true, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, image->GetBuffer() });

		return imageBuffer;
	}

	std::shared_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& textureFilename, bool repeatAddressMode) const
	{
		std::shared_ptr<Image> image = CreateImage(textureFilename);

		std::shared_ptr<ImageBuffer> imageBuffer = CreateImageBuffer(allocator, image, repeatAddressMode);

		return std::make_shared<Material>(image, imageBuffer);
	}

	std::shared_ptr<IModel> CreateModel(Allocator& allocator, const std::shared_ptr<IMesh> mesh) const
	{
		std::shared_ptr<VBO> vertexBuffer = std::make_shared<VBO>(allocator);
		vertexBuffer->Data((void*)mesh->GerVertices().data(), (uint32_t)mesh->GerVertices().size(), sizeof(Vertex));

		std::shared_ptr<IBO> indexBuffer = std::make_shared<IBO>(allocator);
		indexBuffer->Data(mesh->GerIndices().data(), (uint32_t)mesh->GerIndices().size());

		return std::make_shared<Model>(mesh, vertexBuffer, indexBuffer);
	}

public:
	std::shared_ptr<IRenderComponent> CreateCubeRenderComponent(Allocator& allocator, const std::string& textureFilename) const
	{
		std::shared_ptr<IMaterial> material = CreateMaterial(allocator, textureFilename, false);

		std::shared_ptr<IMesh> mesh = std::make_shared<CubeMesh>();
		std::shared_ptr<IModel> model = CreateModel(allocator, mesh);

		return std::make_shared<DefaultRenderComponent>(model, material);
	}

	std::shared_ptr<IRenderComponent> CreatePlaneRenderComponent(Allocator& allocator, const std::string& textureFilename) const
	{
		std::shared_ptr<IMaterial> material = CreateMaterial(allocator, textureFilename, true);

		std::shared_ptr<IMesh> mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 10, 10);
		std::shared_ptr<IModel> model = CreateModel(allocator, mesh);

		return std::make_shared<DefaultRenderComponent>(model, material);
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
	EventHandler<CubeRobot, KeyEvent> m_keyEvent{ *this };

	EventHandler<CubeRobot, MouseEvent> m_mouseEvent{ *this };

	EventHandler<CubeRobot, TouchEvent> m_touchEvent{ *this };

private:
	std::shared_ptr<CubeRobotPart> m_body;

	std::shared_ptr<CubeRobotPart> m_head;

	std::shared_ptr<CubeRobotPart> m_leftArm;

	std::shared_ptr<CubeRobotPart> m_rightArm;

	std::shared_ptr<CubeRobotPart> m_leftLeg;

	std::shared_ptr<CubeRobotPart> m_rightLeg;

private:
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

class Plane : public AbstractSceneNode
{
protected:
	std::shared_ptr<Allocator> m_allocator;

protected:
	glm::vec3 m_position;

	glm::quat m_orientation;

	const std::string m_texturePath;

public:
	Plane(std::shared_ptr<Allocator>& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
		: AbstractSceneNode(), m_allocator(allocator), m_position(position), m_orientation(orientation), m_texturePath(texturePath)
	{
		m_scaler = scale;
	}

	virtual ~Plane()
	{
	}

public:
	void Init() override
	{
		m_transform = MathUtil::CreateTransformationMatrix(m_position, m_orientation);

		RenderComponentFactory renderComponentFactory;
		auto renderComponent = renderComponentFactory.CreatePlaneRenderComponent(*m_allocator, m_texturePath);

		ComponentRepository<IRenderComponent>::GetInstance().Add(m_id, renderComponent);

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
	glm::mat4 CreateProjectionMatrix(const uint32_t w, const uint32_t h) const
	{
		const float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
		return CreateProjectionMatrix(aspectRatio);
	}

	glm::mat4 CreateProjectionMatrix(const float aspectRatio) const
	{
		glm::mat4 projectionMatrix = glm::perspective(m_fov, aspectRatio, m_nearClippingPlane, m_farClippingPlane);
		projectionMatrix[1][1] *= -1; // invert Y in clip coordinates

		return projectionMatrix;
	}

public:
	float GetFov() const // vertical
	{
		return m_fov;
	}

	float GetNearClippingPlane() const
	{
		return m_nearClippingPlane;
	}

	float GetFarClippingPlane() const
	{
		return m_farClippingPlane;
	}
};

// TODO create decorator -> ControllableCamera
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

	const float m_moveSpeed = 25.0f;

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

class Light : public AbstractSceneNode
{
private:
	glm::vec3 m_lookAtPosition{ 50.0f, 0.0f, 50.0f };

	glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

	ViewFrustum m_viewFrustum{ 70.0f, 1.0f, 1000.0f };

	glm::vec3 m_position;

public:
	Light(const glm::vec3& pos)
		: m_position(pos)
	{
	}

	~Light()
	{
	}

public:
	void Update(float deltaTime)
	{
		const float ROTATION_SPEED_DEG_PER_SEC = 7.5f;
		const float ROTATION_ANGLE = ROTATION_SPEED_DEG_PER_SEC * deltaTime;

		glm::mat4 transform(1.0f);
		transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), m_upDirection);
		transform = glm::translate(transform, m_position);

		m_position = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	}

public:
	glm::mat4 LookAt() const
	{
		return glm::lookAt(m_position, m_lookAtPosition, m_upDirection);
	}

	glm::mat4 GetProjectionMatrix() const
	{
		return m_viewFrustum.CreateProjectionMatrix(1.0f); // we expect that light will shine in square frustum(should be it more like a cone-like shape?)
	}

	glm::vec3 GetPosition() const
	{
		return m_position;
	}
};

class Shadows : public AbstractSceneNode
{
private:
	const VkFormat m_shadowMapFormat = VK_FORMAT_D24_UNORM_S8_UINT;

	const uint32_t m_shadowMapDimension = 4096;

	const VkFilter m_shadowMapFilter = VK_FILTER_NEAREST;

private:
	std::shared_ptr<Allocator> m_allocator;

	std::shared_ptr<Device> m_device;

private:
	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<DepthImageBuffer> m_depthBuffer;

	VkFramebuffer m_frameBuffer;

public:
	Shadows(std::shared_ptr<Allocator> allocator, std::shared_ptr<Device> dev)
		: m_allocator(allocator), m_device(dev)
	{
	}

	virtual ~Shadows()
	{
	}

private:
	void InitRenderPass()
	{
		std::vector<VkSubpassDependency> dependencies(2);
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		m_renderPass = std::make_shared<RenderPass>(*m_device);
		m_renderPass->AddDepthAttachment(m_shadowMapFormat);
		m_renderPass->AddSubpass({ 0 });
		m_renderPass->AddSubpassDependency(dependencies);
		m_renderPass->Create();
	}

	void ShutDownRenderPass()
	{
		m_renderPass->Destroy();
	}

	void InitFrameBuffer()
	{
		m_depthBuffer = std::make_shared<DepthImageBuffer>(*m_allocator);
		m_depthBuffer->Create(ImageBufferCreateInfo{ GetExtent(), m_shadowMapFormat });
		m_depthBuffer->CreateSampler();

		m_frameBuffer = VkUtils::CreateFrameBuffer(*m_device, *m_renderPass, { m_depthBuffer->GetImageView() }, GetExtent());
	}

	void ShutDownFrameBuffer()
	{
		vkDestroyFramebuffer(*m_device, m_frameBuffer, nullptr);

		m_depthBuffer->Destroy();
	}

public:
	void Init() override
	{
		InitRenderPass();
		InitFrameBuffer();

		AbstractSceneNode::Init();
	}

	void ShutDown() override
	{
		AbstractSceneNode::ShutDown();

		ShutDownFrameBuffer();
		ShutDownRenderPass();
	}

public:
	std::shared_ptr<RenderPass> GetRenderPass() const
	{
		return m_renderPass;
	}

	VkFramebuffer GetFrameBuffer() const
	{
		return m_frameBuffer;
	}

	VkExtent2D GetExtent() const
	{
		return { m_shadowMapDimension, m_shadowMapDimension };
	}

	std::shared_ptr<IImageBuffer> GetImageBuffer() const
	{
		return m_depthBuffer;
	}
};

class ShadowsRenderer : public IRenderer
{
private:
	struct Uniforms
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(16) glm::mat4 viewMatrix;
		alignas(16) glm::mat4 projectionMatrix;
	};

private:
	// Depth bias (and slope) are used to avoid shadowing artefacts Constant depth bias factor (always applied)
	const float m_depthBiasConstant = 1.25f;

	// Slope depth bias factor, applied depending on polygon's slope
	const float m_depthBiasSlope = 1.75f;

private:
	std::shared_ptr<Allocator> m_allocator;

	std::shared_ptr<Device> m_device;

	std::shared_ptr<Light> m_light;

	std::shared_ptr<Shadows> m_shadows;

private:
	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<IGraphicsPipeline> m_pipeline;

	std::shared_ptr<UBOPool<Uniforms>> m_uniformsPool;

public:
	ShadowsRenderer(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<Light> light, std::shared_ptr<Shadows> shadows)
		: m_allocator(alloc), m_device(dev), m_light(light), m_shadows(shadows)
	{
	}

	virtual ~ShadowsRenderer()
	{
	}

public:
	void Init() override
	{
		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(*m_device, {
			{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/shadows_vert.spv" }
			});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		printf("Shadows Shader created\n");

		m_pipeline = std::make_shared<ShadowsPipeline>(*m_device, *m_shadows->GetRenderPass(), *m_shader);
		m_pipeline->Init();

		printf("Shadows Pipeline created\n");

		m_uniformsPool = std::make_shared<UBOPool<Uniforms>>(m_allocator);
		m_uniformsPool->AdjustCapactity(10000);
	}

	void PreRender(RenderContext& renderContext) override
	{
		m_shadows->GetRenderPass()->Begin(m_shadows->GetFrameBuffer(), renderContext.defaultCommandBuffer, { { 0, 0 }, m_shadows->GetExtent() });

		VkRect2D scissor = { { 0, 0 }, m_shadows->GetExtent() };
		VkViewport viewport = { 0, 0, static_cast<float>(m_shadows->GetExtent().width), static_cast<float>(m_shadows->GetExtent().height), 0, 1 };

		vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);

		vkCmdSetDepthBias(renderContext.defaultCommandBuffer, m_depthBiasConstant, 0.0f, m_depthBiasSlope);
	}

	void Render(RenderContext& renderContext, std::shared_ptr<ISceneNode> node) override
	{
		if (ComponentRepository<IRenderComponent>::GetInstance().Contains(node->GetId()))
		{
			auto renderComponent = ComponentRepository<IRenderComponent>::GetInstance().Get(node->GetId());

			auto ubo = m_uniformsPool->GetNext();

			Uniforms uniforms;
			uniforms.projectionMatrix = m_light->GetProjectionMatrix();
			uniforms.viewMatrix = m_light->LookAt();
			uniforms.modelMatrix = node->GetWorldTransformScaled();
			ubo->Update(&uniforms);

			m_shader->Bind("ubo", *ubo);

			VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
			VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
			vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

			vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
		}

		for (auto child : node->GetChildren())
		{
			Render(renderContext, child);
		}
	}

	void PostRender(RenderContext& renderContext) override
	{
		m_shadows->GetRenderPass()->End(renderContext.defaultCommandBuffer);
	}

	void ShutDown() override
	{
		m_pipeline->ShutDown();

		m_shader->ShutDown();
	}
};

class QuadRenderer : public IRenderer
{
private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<IGraphicsPipeline> m_pipeline;

private:
	std::shared_ptr<IImageBuffer> m_imageBuffer;

	std::shared_ptr<IModel> m_quadModel;

public:
	QuadRenderer(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass, std::shared_ptr<IImageBuffer> imageBuffer)
		: m_device(dev), m_renderPass(renderPass), m_allocator(alloc), m_imageBuffer(imageBuffer)
	{
	}

	virtual ~QuadRenderer()
	{
	}

public:
	void Init() override
	{
		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(*m_device, {
			{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/quad_vert.spv" },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/quad_frag.spv" }
			});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		printf("Default Shader created\n");

		m_pipeline = std::make_shared<QuadPipeline>(*m_device, *m_renderPass, *m_shader);
		m_pipeline->Init();

		// create quad model
		std::shared_ptr<IMesh> quadMesh = std::make_shared<QuadMesh>();

		std::shared_ptr<VBO> vertexBuffer = std::make_shared<VBO>(*m_allocator);
		vertexBuffer->Data((void*)quadMesh->GerVertices().data(), (uint32_t)quadMesh->GerVertices().size(), sizeof(Vertex));

		std::shared_ptr<IBO> indexBuffer = std::make_shared<IBO>(*m_allocator);
		indexBuffer->Data(quadMesh->GerIndices().data(), (uint32_t)quadMesh->GerIndices().size());

		m_quadModel = std::make_shared<Model>(quadMesh, vertexBuffer, indexBuffer);
	}

	void PreRender(RenderContext& renderContext) override
	{
		VkRect2D renderRect;
		renderRect.extent.width = renderContext.fullExtent.width / 2;
		renderRect.extent.height = renderContext.fullExtent.height / 2;
		renderRect.offset.x = 0;
		renderRect.offset.y = 0;

		m_renderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, renderRect);

		VkRect2D scissor;
		scissor.extent.width = renderContext.fullExtent.width;
		scissor.extent.height = renderContext.fullExtent.height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;

		VkViewport viewport;
		viewport.width = static_cast<float>(renderContext.fullExtent.width);
		viewport.height = static_cast<float>(renderContext.fullExtent.height);
		viewport.x = -static_cast<float>(renderContext.fullExtent.width / 2.0f);
		viewport.y = -static_cast<float>(renderContext.fullExtent.height / 2.0f);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
	}

	// make a node with quad model & shadowMap texture ???
	void Render(RenderContext& renderContext, std::shared_ptr<ISceneNode> node) override
	{
		m_shader->Bind("textureSampler", *m_imageBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();

		VkBuffer vertexBuffers[] = { *m_quadModel->GetVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *m_quadModel->GetIndexBuffer(), 0, m_quadModel->GetIndexBuffer()->GetIndexType());
		vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

		vkCmdDrawIndexed(renderContext.defaultCommandBuffer, m_quadModel->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	void PostRender(RenderContext& renderContext) override
	{
		m_renderPass->End(renderContext.defaultCommandBuffer);
	}

	void ShutDown() override
	{
		m_shader->ShutDown();

		m_pipeline->ShutDown();
	}
};

class DefaultSceneRenderer : public IRenderer
{
private:
	struct Uniforms
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(16) glm::mat4 viewMatrix;
		alignas(16) glm::mat4 projectionMatrix;
		alignas(16) glm::mat4 normalMatrix;

		alignas(16) glm::mat4 lightViewProjectionMatrix;
		alignas(16) glm::vec3 lightPosition;
	};

private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_renderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<Shader> m_shader;

	std::shared_ptr<IGraphicsPipeline> m_pipeline;

	std::shared_ptr<UBOPool<Uniforms>> m_uniformsPool;

	std::shared_ptr<Camera> m_freeCamera;

	std::shared_ptr<Light> m_light;

	std::shared_ptr<Shadows> m_shadows;

	ViewFrustum m_viewFrustum{ 70.0f, 0.01f, 1000.0f };

public:
	DefaultSceneRenderer(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass, std::shared_ptr<Light> light, std::shared_ptr<Shadows> shadows, std::shared_ptr<Camera> camera)
		: m_device(dev), m_renderPass(renderPass), m_allocator(alloc), m_light(light), m_shadows(shadows), m_freeCamera(camera)
	{
	}

	virtual ~DefaultSceneRenderer()
	{
	}

public:
	void Init() override
	{
		ShaderFactory shaderFactory;
		m_shader = shaderFactory.CreateShaderFromFiles(*m_device, {
			{ VK_SHADER_STAGE_VERTEX_BIT, "shaders/scene_vert.spv" },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, "shaders/scene_frag.spv" }
		});
		m_shader->AdjustDescriptorPoolCapacity(10000);

		printf("Default Shader created\n");

		m_pipeline = std::make_shared<DefaultPipeline>(*m_device, *m_renderPass, *m_shader);
		m_pipeline->Init();

		printf("Default Pipeline created\n");

		m_uniformsPool = std::make_shared<UBOPool<Uniforms>>(m_allocator);
		m_uniformsPool->AdjustCapactity(10000);
	}

	void PreRender(RenderContext& renderContext) override
	{
		m_renderPass->Begin(renderContext.defaultFrameBuffer, renderContext.defaultCommandBuffer, { { 0, 0 }, renderContext.fullExtent });

		VkRect2D scissor = { { 0, 0 }, renderContext.fullExtent };
		VkViewport viewport = { 0, 0, static_cast<float>(renderContext.fullExtent.width), static_cast<float>(renderContext.fullExtent.height), 0, 1 };

		vkCmdBindPipeline(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);
		vkCmdSetViewport(renderContext.defaultCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderContext.defaultCommandBuffer, 0, 1, &scissor);
	}

	void Render(RenderContext& renderContext, std::shared_ptr<ISceneNode> node) override
	{
		if (ComponentRepository<IRenderComponent>::GetInstance().Contains(node->GetId()))
		{
			auto renderComponent = ComponentRepository<IRenderComponent>::GetInstance().Get(node->GetId());

			auto ubo = m_uniformsPool->GetNext();

			Uniforms uniforms;
			uniforms.projectionMatrix = m_viewFrustum.CreateProjectionMatrix(renderContext.fullExtent.width, renderContext.fullExtent.height);
			uniforms.viewMatrix = m_freeCamera->LookAt();
			uniforms.modelMatrix = node->GetWorldTransformScaled();
			uniforms.normalMatrix = glm::inverse(node->GetWorldTransformScaled());
			uniforms.lightViewProjectionMatrix = m_light->GetProjectionMatrix() * m_light->LookAt();
			uniforms.lightPosition = m_light->GetPosition();

			ubo->Update(&uniforms);

			m_shader->Bind("textureSampler", *renderComponent->GetMaterial()->GetImageBuffer(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_shader->Bind("depthSampler", *m_shadows->GetImageBuffer(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
			m_shader->Bind("ubo", *ubo);

			VkDescriptorSet descriptorSet = m_shader->UpdateNextDescriptorSet();
			VkBuffer vertexBuffers[] = { *renderComponent->GetModel()->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(renderContext.defaultCommandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(renderContext.defaultCommandBuffer, *renderComponent->GetModel()->GetIndexBuffer(), 0, renderComponent->GetModel()->GetIndexBuffer()->GetIndexType());
			vkCmdBindDescriptorSets(renderContext.defaultCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

			vkCmdDrawIndexed(renderContext.defaultCommandBuffer, renderComponent->GetModel()->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
		}

		for (auto child : node->GetChildren())
		{
			Render(renderContext, child);
		}
	}

	void PostRender(RenderContext& renderContext) override
	{
		m_renderPass->End(renderContext.defaultCommandBuffer);
	}

	void ShutDown() override
	{
		m_shader->ShutDown();

		m_pipeline->ShutDown();
	}
};

class RootSceneNode : public AbstractSceneNode
{
private:
	std::shared_ptr<Device> m_device;

	std::shared_ptr<RenderPass> m_shadowsRenderPass;

	std::shared_ptr<RenderPass> m_defaultRenderPass;

	std::shared_ptr<Allocator> m_allocator;

private:
	std::shared_ptr<IRenderer> m_shadowsRenderer;

	std::shared_ptr<IRenderer> m_defaultRenderer;

	std::shared_ptr<IRenderer> m_quadRenderer;

public:
	RootSceneNode(std::shared_ptr<Allocator> alloc, std::shared_ptr<Device> dev, std::shared_ptr<RenderPass> renderPass)
		: AbstractSceneNode(), m_device(dev), m_defaultRenderPass(renderPass), m_allocator(alloc)
	{
	}

	virtual ~RootSceneNode()
	{
	}

public:
	void Init() override
	{
		// Init scene nodes
		std::shared_ptr<Light> light = std::make_shared<Light>(glm::vec3(150.0f, 150.0f, 150.0f));
		AddChild(light);

		std::shared_ptr<Shadows> shadows = std::make_shared<Shadows>(m_allocator, m_device);
		AddChild(shadows);

		std::shared_ptr<Camera> freeCamera = std::make_shared<Camera>();
		AddChild(freeCamera);

		auto camRobot = std::make_shared<CubeRobot>(m_allocator, glm::vec3(1.0f, -0.4f, -1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), "texture.jpg");
		freeCamera->AddChild(camRobot);

		const int32_t MAX_GENERATED_HEIGHT = 1;
		const float DISTANCE = 40.0f;

		for (int32_t i = 0; i <= MAX_GENERATED_HEIGHT; i++)
		{
			for (int32_t j = 0; j <= MAX_GENERATED_HEIGHT; j++)
			{
				for (int32_t k = 0; k <= MAX_GENERATED_HEIGHT; k++)
				{
					auto robot = std::make_shared<CubeRobot>(m_allocator, glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), "texture.jpg");
					AddChild(robot);
				}
			}
		}

		auto groundPlane = std::make_shared<Plane>(m_allocator, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(12.0f), "cement.jpg");
		AddChild(groundPlane);

		for (auto child : m_children)
		{
			child->Init();
		}

		// Init renderera
		m_shadowsRenderer = std::make_shared<ShadowsRenderer>(m_allocator, m_device, light, shadows);
		m_shadowsRenderer->Init();

		m_defaultRenderer = std::make_shared<DefaultSceneRenderer>(m_allocator, m_device, m_defaultRenderPass, light, shadows, freeCamera);
		m_defaultRenderer->Init();

		m_quadRenderer = std::make_shared<QuadRenderer>(m_allocator, m_device, m_defaultRenderPass, shadows->GetImageBuffer());
		m_quadRenderer->Init();
	}

	void Update(float deltaTime) override
	{
		for (auto child : m_children)
		{
			child->Update(deltaTime);
		}
	}

	void Render(RenderContext& renderContext) override
	{
		// shadows
		m_shadowsRenderer->PreRender(renderContext);

		for (auto child : m_children)
		{
			m_shadowsRenderer->Render(renderContext, child);
		}

		m_shadowsRenderer->PostRender(renderContext);

		// Default
		m_defaultRenderer->PreRender(renderContext);

		for (auto child : m_children)
		{
			m_defaultRenderer->Render(renderContext, child);
		}

		m_defaultRenderer->PostRender(renderContext);

		// Debug quad with shadowMap
		m_quadRenderer->PreRender(renderContext);
		
		m_quadRenderer->Render(renderContext, nullptr);

		m_quadRenderer->PostRender(renderContext);
	}

	void ShutDown() override
	{
		for (auto child : m_children)
		{
			child->ShutDown();
		}

		m_quadRenderer->ShutDown();
		m_defaultRenderer->ShutDown();
		m_shadowsRenderer->ShutDown();
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
