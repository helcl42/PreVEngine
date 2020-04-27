#include <array>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>

#include <App.h>
#include <Common.h>
#include <Image.h>
#include <Inputs.h>
#include <SceneGraph.h>
#include <Shader.h>
#include <Utils.h>

#include "Animation.h"
#include "Camera.h"
#include "Culling.h"
#include "Font.h"
#include "General.h"
#include "LensFlare.h"
#include "Light.h"
#include "Mesh.h"
#include "Pipeline.h"
#include "Particles.h"
#include "RayCasting.h"
#include "Shadows.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "Water.h"

#include "Renderer.h"

class DefaultRenderComponent : public IRenderComponent {
private:
    std::shared_ptr<IModel> m_model;

    std::shared_ptr<IMaterial> m_material;

    bool m_castsShadows;

    bool m_isCastedByShadows;

public:
    DefaultRenderComponent(const std::shared_ptr<IModel>& model, const std::shared_ptr<IMaterial>& material, const bool castsShadows, const bool isCastedByShadows)
        : m_model(model)
        , m_material(material)
        , m_castsShadows(castsShadows)
        , m_isCastedByShadows(isCastedByShadows)
    {
    }

    virtual ~DefaultRenderComponent() = default;

public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    bool CastsShadows() const override
    {
        return m_castsShadows;
    }

    bool IsCastedByShadows() const override
    {
        return m_isCastedByShadows;
    }
};

class DefaultAnimationRenderComponent : public IAnimationRenderComponent {
private:
    std::shared_ptr<IModel> m_model;

    std::shared_ptr<IMaterial> m_material;

    std::shared_ptr<IAnimation> m_animation;

    bool m_castsShadows;

    bool m_isCastedByShadows;

public:
    DefaultAnimationRenderComponent(const std::shared_ptr<IModel>& model, const std::shared_ptr<IMaterial>& material, const std::shared_ptr<IAnimation>& animation, const bool castsShadows, const bool isCastedByShadows)
        : m_model(model)
        , m_material(material)
        , m_animation(animation)
        , m_castsShadows(castsShadows)
        , m_isCastedByShadows(isCastedByShadows)
    {
    }

    virtual ~DefaultAnimationRenderComponent() = default;

public:
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }

    std::shared_ptr<IMaterial> GetMaterial() const override
    {
        return m_material;
    }

    std::shared_ptr<IAnimation> GetAnimation() const override
    {
        return m_animation;
    }

    bool CastsShadows() const override
    {
        return m_castsShadows;
    }

    bool IsCastedByShadows() const override
    {
        return m_isCastedByShadows;
    }
};

class RenderComponentFactory {
private:
    static std::map<std::string, std::shared_ptr<Image> > s_imagesCache;

private:
    std::shared_ptr<Image> CreateImage(const std::string& textureFilename) const
    {
        std::shared_ptr<Image> image;
        if (s_imagesCache.find(textureFilename) != s_imagesCache.cend()) {
            image = s_imagesCache[textureFilename];
        } else {
            ImageFactory imageFactory;
            image = imageFactory.CreateImage(textureFilename);
            s_imagesCache[textureFilename] = image;
        }
        return image;
    }

    std::unique_ptr<ImageBuffer> CreateImageBuffer(Allocator& allocator, const std::shared_ptr<Image>& image, const bool filtering, const bool repeatAddressMode) const
    {
        const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, filtering, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });

        return imageBuffer;
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& texturePath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
    {
        auto image = CreateImage(texturePath);
        auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

        return std::make_unique<Material>(image, std::move(imageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
    {
        auto image = CreateImage(texturePath);
        auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

        auto normalImage = CreateImage(normalMapPath);
        auto normalImageBuffer = CreateImageBuffer(allocator, normalImage, false, repeatAddressMode);

        return std::make_unique<Material>(image, std::move(imageBuffer), normalImage, std::move(normalImageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
    {
        auto image = CreateImage(texturePath);
        auto imageBuffer = CreateImageBuffer(allocator, image, true, repeatAddressMode);

        auto normalImage = CreateImage(normalMapPath);
        auto normalImageBuffer = CreateImageBuffer(allocator, normalImage, false, repeatAddressMode);

        auto heightImage = CreateImage(heightMapPath);
        auto heightImageBuffer = CreateImageBuffer(allocator, heightImage, false, repeatAddressMode);

        return std::make_unique<Material>(image, std::move(imageBuffer), normalImage, std::move(normalImageBuffer), heightImage, std::move(heightImageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IModel> CreateModel(Allocator& allocator, const std::shared_ptr<IMesh>& mesh) const
    {
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertexData(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), (uint32_t)mesh->GerIndices().size());

        return std::make_unique<Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
    }

public:
    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, false, 10.0f, 1.0f);

        auto mesh = std::make_unique<CubeMesh>();
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 10.0f, 10.0f, false);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateNormalMappedPlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateParallaxMappedPlaneRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& parallaxMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, parallaxMapPath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 1.0f, 1.0f, true);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateSphereRenderComponent(const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<SphereMesh>(1.0f, 64, 64, 360.0f, 180.0f, false);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateNormalMappedSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<SphereMesh>(1.0f, 64, 64, 360.0f, 180.0f, true);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateParallaxMappedSphereRenderComponent(const std::string& texturePath, const std::string& normalMapPath, const std::string& parallaxMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, parallaxMapPath, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<SphereMesh>(1.0f, 64, 64, 360.0f, 180.0f, true);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, true, 2.0f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath);
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateModelNormalMappedRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 10.0f, 0.7f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT });
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateModelParallaxMappedRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightMapPath, true, 3.0f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT });
        auto model = CreateModel(*allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, true, 1.5f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION });
        auto model = CreateModel(*allocator, std::move(mesh));

        AnimationFactory animationFactory{};
        auto animation = animationFactory.CreateAnimation(modelPath);

        return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedNormalMappedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, true, 1.5f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION | MeshFactory::AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT });
        auto model = CreateModel(*allocator, std::move(mesh));

        AnimationFactory animationFactory{};
        auto animation = animationFactory.CreateAnimation(modelPath);

        return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedParallaxMappedModelRenderComponent(const std::string& modelPath, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        auto material = CreateMaterial(*allocator, texturePath, normalMapPath, heightMapPath, true, 1.5f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION | MeshFactory::AssimpMeshFactoryCreateFlags::TANGENT_BITANGENT });
        auto model = CreateModel(*allocator, std::move(mesh));

        AnimationFactory animationFactory{};
        auto animation = animationFactory.CreateAnimation(modelPath);

        return std::make_unique<DefaultAnimationRenderComponent>(std::move(model), std::move(material), std::move(animation), castsShadows, isCastedByShadows);
    }
};

std::map<std::string, std::shared_ptr<Image> > RenderComponentFactory::s_imagesCache;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SCENE
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class AbstractCubeRobotSceneNode : public AbstractSceneNode<SceneNodeFlags> {
public:
    AbstractCubeRobotSceneNode(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractSceneNode()
        , m_initialPosition(position)
        , m_initialOrientation(orientation)
        , m_initialScale(scale)
        , m_texturePath(texturePath)
    {
    }

    virtual ~AbstractCubeRobotSceneNode() = default;

public:
    void Init() override
    {
        RenderComponentFactory renderComponentFactory{};
        std::shared_ptr<IRenderComponent> renderComponent = renderComponentFactory.CreateCubeRenderComponent(m_texturePath, true, true);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IRenderComponent>(GetThis(), renderComponent, SceneNodeFlags::RENDER_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(renderComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        const auto selectableComponent = std::make_shared<SelectableComponent>();
        NodeComponentHelper::AddComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), selectableComponent, SceneNodeFlags::SELECTABLE_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_transformComponent->Update(deltaTime);
        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), SceneNodeFlags::SELECTABLE_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IRenderComponent>(GetThis(), SceneNodeFlags::RENDER_COMPONENT);
    }

protected:
    const std::string m_texturePath;

    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

private:
    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class CubeRobotPart : public AbstractCubeRobotSceneNode {
public:
    CubeRobotPart(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractCubeRobotSceneNode(position, orientation, scale, texturePath)
    {
    }

    virtual ~CubeRobotPart() = default;
};

class CubeRobot : public AbstractCubeRobotSceneNode {
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
    CubeRobot(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractCubeRobotSceneNode(position, orientation, scale, texturePath)
    {
    }

    virtual ~CubeRobot() = default;

public:
    void Init() override
    {
        m_body = std::make_shared<CubeRobotPart>(glm::vec3(0, 35, 0), glm::quat(1, 0, 0, 0), glm::vec3(10, 15, 5), AssetManager::Instance().GetAssetPath("Textures/vulkan.png"));

        m_head = std::make_shared<CubeRobotPart>(glm::vec3(0, 10, 0), glm::quat(1, 0, 0, 0), glm::vec3(5, 5, 5), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        m_leftArm = std::make_shared<CubeRobotPart>(glm::vec3(-8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        m_rightArm = std::make_shared<CubeRobotPart>(glm::vec3(8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        m_leftLeg = std::make_shared<CubeRobotPart>(glm::vec3(-4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        m_rightLeg = std::make_shared<CubeRobotPart>(glm::vec3(4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));

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
        auto bodyTransformComponent = ComponentRepository<ITransformComponent>::Instance().Get(m_body->GetId());
        bodyTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.x), glm::vec3(1.0f, 0.0f, 0.0f)));
        bodyTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.y), glm::vec3(0.0f, 1.0f, 0.0f)));

        auto headTransformComponent = ComponentRepository<ITransformComponent>::Instance().Get(m_head->GetId());
        headTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), -glm::radians(25.0f) * deltaTime, glm::vec3(0, 1, 0)));

        auto leftArmTransformComponent = ComponentRepository<ITransformComponent>::Instance().Get(m_leftArm->GetId());
        leftArmTransformComponent->Translate(glm::vec3(0, -4.5, 0));
        leftArmTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f) * deltaTime, glm::vec3(1, 0, 0)));
        leftArmTransformComponent->Translate(glm::vec3(0, 4.5, 0));

        AbstractCubeRobotSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractCubeRobotSceneNode::ShutDown();
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_Left) {
                m_angularVelocity.y -= 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Right) {
                m_angularVelocity.y += 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Up) {
                m_angularVelocity.x += 0.1f;
            }

            if (keyEvent.keyCode == KeyCode::KEY_Down) {
                m_angularVelocity.x -= 0.1f;
            }
        }
    }

    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::RIGHT) {
            m_angularVelocity = glm::vec2(mouseEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - mouseEvent.position.y);
        }

        m_prevMousePosition = mouseEvent.position;
    }

    void operator()(const TouchEvent& touchEvent)
    {
        if (touchEvent.action == TouchActionType::MOVE) {
            m_angularVelocity = glm::vec2(touchEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - touchEvent.position.y);
        }

        m_prevMousePosition = touchEvent.position;
    }
};

class PlaneNode : public AbstractSceneNode<SceneNodeFlags> {
public:
    PlaneNode(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath, const std::string& normalMapPath, const std::string& heightMapPath, const float heightScale)
        : AbstractSceneNode()
        , m_initialPosition(position)
        , m_initialOrientation(orientation)
        , m_initialScale(scale)
        , m_texturePath(texturePath)
        , m_normalMapPath(normalMapPath)
        , m_heightMapPath(heightMapPath)
        , m_heightScale(heightScale)
    {
    }

    virtual ~PlaneNode() = default;

public:
    void Init() override
    {
        RenderComponentFactory renderComponentFactory{};
        std::shared_ptr<IRenderComponent> renderComponent = renderComponentFactory.CreateParallaxMappedPlaneRenderComponent(m_texturePath, m_normalMapPath, m_heightMapPath, false, true);
        renderComponent->GetMaterial()->SetHeightScale(m_heightScale);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IRenderComponent>(GetThis(), renderComponent, SceneNodeFlags::RENDER_PARALLAX_MAPPED_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(renderComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_transformComponent->Update(deltaTime);
        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IRenderComponent>(GetThis(), SceneNodeFlags::RENDER_PARALLAX_MAPPED_COMPONENT);
    }

protected:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    const std::string m_texturePath;

    const std::string m_normalMapPath;

    const std::string m_heightMapPath;

    const float m_heightScale;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class Terrain : public AbstractSceneNode<SceneNodeFlags> {
public:
    Terrain(const int x, const int z)
        : AbstractSceneNode()
        , m_xIndex(x)
        , m_zIndex(z)
    {
    }

    virtual ~Terrain() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        TerrainComponentFactory terrainComponentFactory{};
        m_terrainComponent = terrainComponentFactory.CreateRandomTerrainParallaxMapped(m_xIndex, m_zIndex, TERRAIN_SIZE);
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITerrainComponenet>(GetThis(), m_terrainComponent, SceneNodeFlags::TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_terrainComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        m_transformComponent->SetPosition(m_terrainComponent->GetPosition());

        m_terrainManagerComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });
        if (auto manager = m_terrainManagerComponent.lock()) {
            manager->AddTerrainComponent(m_terrainComponent);
        }

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_transformComponent->Update(deltaTime);
        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransform());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        if (auto manager = m_terrainManagerComponent.lock()) {
            manager->RemoveTerrain(m_terrainComponent);
        }

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITerrainComponenet>(GetThis(), SceneNodeFlags::TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    const int m_xIndex;

    const int m_zIndex;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<ITerrainComponenet> m_terrainComponent;

    std::weak_ptr<ITerrainManagerComponent> m_terrainManagerComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class TerrainManager : public AbstractSceneNode<SceneNodeFlags> {
private:
    const uint32_t m_gridMaxX;

    const uint32_t m_gridMaxZ;

public:
    TerrainManager(const uint32_t maxX, const uint32_t maxZ)
        : AbstractSceneNode()
        , m_gridMaxX(maxX)
        , m_gridMaxZ(maxZ)
    {
    }

    virtual ~TerrainManager() = default;

public:
    void Init() override
    {
        std::shared_ptr<ITerrainManagerComponent> terrainManagerComponent = TerrainManagerComponentFactory{}.Create();
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITerrainManagerComponent>(GetThis(), terrainManagerComponent, SceneNodeFlags::TERRAIN_MANAGER_COMPONENT);

        auto selectableComponent = std::make_shared<SelectableComponent>();
        NodeComponentHelper::AddComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), selectableComponent, SceneNodeFlags::SELECTABLE_COMPONENT);

        for (uint32_t x = 0; x < m_gridMaxX; x++) {
            for (uint32_t z = 0; z < m_gridMaxZ; z++) {
                auto terrain = std::make_shared<Terrain>(static_cast<int>(x), static_cast<int>(z));
                AddChild(terrain);
            }
        }

        AbstractSceneNode::Init();

        float minHeight = std::numeric_limits<float>::max();
        float maxHeight = std::numeric_limits<float>::min();
        auto terrains = NodeComponentHelper::FindAll<SceneNodeFlags, ITerrainComponenet>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_RENDER_COMPONENT | SceneNodeFlags::TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT | SceneNodeFlags::TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT });
        for (const auto& terrain : terrains) {
            auto heightInfo = terrain->GetHeightMapInfo();
            if (minHeight > heightInfo->minHeight) {
                minHeight = heightInfo->minHeight;
            }
            if (maxHeight < heightInfo->maxHeight) {
                maxHeight = heightInfo->maxHeight;
            }
        }

        for (auto& terrain : terrains) {
            auto heightInfo = terrain->GetHeightMapInfo();
            heightInfo->globalMinHeight = minHeight;
            heightInfo->globalMaxHeight = maxHeight;
        }
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), SceneNodeFlags::SELECTABLE_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITerrainManagerComponent>(GetThis(), SceneNodeFlags::TERRAIN_MANAGER_COMPONENT);
    }
};

class InputsHelper : public AbstractSceneNode<SceneNodeFlags> {
public:
    InputsHelper()
        : AbstractSceneNode()
    {
        m_inputFacade.SetMouseLocked(true);
        m_inputFacade.SetMouseCursorVisible(false);
    }

    virtual ~InputsHelper() = default;

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_L) {
                m_inputFacade.SetMouseLocked(!m_inputFacade.IsMouseLocked());
                m_inputFacade.SetMouseCursorVisible(!m_inputFacade.IsMouseCursorVisible());
            }
        }
    }

private:
    InputsFacade m_inputFacade;

    EventHandler<InputsHelper, KeyEvent> m_keyboardEventsHandler{ *this };
};

class Goblin : public AbstractSceneNode<SceneNodeFlags> {
public:
    Goblin(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : AbstractSceneNode()
        , m_initialPosition(position)
        , m_initialOrientation(orientation)
        , m_initialScale(scale)
    {
    }

    virtual ~Goblin() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        RenderComponentFactory renderComponentFactory{};
        m_animatonRenderComponent = renderComponentFactory.CreateAnimatedParallaxMappedModelRenderComponent(AssetManager::Instance().GetAssetPath("Models/Goblin/goblin.dae"), AssetManager::Instance().GetAssetPath("Models/Goblin/goblin_texture.png"), AssetManager::Instance().GetAssetPath("Models/Goblin/goblin_normal_texture_2.png"), AssetManager::Instance().GetAssetPath("Models/Goblin/goblin_height_texture.png"), true, true);
        m_animatonRenderComponent->GetMaterial()->SetHeightScale(0.005f);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IAnimationRenderComponent>(GetThis(), m_animatonRenderComponent, SceneNodeFlags::ANIMATION_PARALLAX_MAPPED_RENDER_COMPONENT);

        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
        NodeComponentHelper::AddComponent<SceneNodeFlags, ICameraComponent>(GetThis(), m_cameraComponent, SceneNodeFlags::CAMERA_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_animatonRenderComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        m_animatonRenderComponent->GetAnimation()->SetIndex(0);
        m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
        m_animatonRenderComponent->GetAnimation()->SetSpeed(1.0f);

        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), m_cameraComponent->GetUpDirection())));
        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), m_cameraComponent->GetRightDirection())));

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto terrain = NodeComponentHelper::FindOne<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });

        if ((m_shouldGoForward || m_shouldGoBackward || m_shouldGoLeft || m_shouldGoRight) && !m_isInTheAir) {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
            m_animatonRenderComponent->GetAnimation()->Update(m_shouldGoBackward ? -deltaTime : deltaTime);

            glm::vec3 positionOffset{ 0.0f };
            if (m_shouldGoForward) {
                positionOffset -= deltaTime * MathUtil::GetUpVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoBackward) {
                positionOffset += deltaTime * MathUtil::GetUpVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoLeft) {
                positionOffset += deltaTime * MathUtil::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoRight) {
                positionOffset -= deltaTime * MathUtil::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
            }
            m_transformComponent->Translate(positionOffset);
        } else {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::PAUSED);
            m_animatonRenderComponent->GetAnimation()->Update(deltaTime);
        }

        auto currentPosition = m_transformComponent->GetPosition();
        float height = 0.0f;
        terrain->GetHeightAt(currentPosition, height);
        const auto currentY = height + MIN_Y_POS;

        if (m_isInTheAir) {
            m_upwardSpeed += GRAVITY_Y * deltaTime;
            m_transformComponent->Translate(glm::vec3(0.0f, m_upwardSpeed, 0.0f));
            if (currentPosition.y < currentY) {
                m_transformComponent->SetPosition(glm::vec3(currentPosition.x, currentY, currentPosition.z));
                m_upwardSpeed = 0.0f;
                m_isInTheAir = false;
            }
        } else {
            m_transformComponent->SetPosition(glm::vec3(currentPosition.x, currentY, currentPosition.z));
        }

        if (m_shouldRotate) {
            const float yawAmount = YAW_TURN_SPEED * m_rotationAroundY * deltaTime;
            const float pitchAmount = PITCH_TURN_SPEED * m_pitchDiff * deltaTime;

            m_transformComponent->Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(yawAmount), glm::vec3(0.0f, 0.0f, 1.0f))));

            m_cameraComponent->AddYaw(yawAmount);
            m_cameraComponent->AddPitch(pitchAmount);

            m_rotationAroundY = 0.0f;
            m_pitchDiff = 0.0f;
        }

        m_transformComponent->Update(deltaTime);

        const glm::vec3 cameraPosition = m_transformComponent->GetPosition() + (-m_cameraComponent->GetForwardDirection() * m_distanceFromPerson) + glm::vec3(0.0f, 8.0f, 0.0f);
        m_cameraComponent->SetPosition(cameraPosition);

        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ICameraComponent>(GetThis(), SceneNodeFlags::CAMERA_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IAnimationRenderComponent>(GetThis(), SceneNodeFlags::ANIMATION_PARALLAX_MAPPED_RENDER_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_W) {
                m_shouldGoForward = true;
            }
            if (keyEvent.keyCode == KeyCode::KEY_S) {
                m_shouldGoBackward = true;
            }
            if (keyEvent.keyCode == KeyCode::KEY_A) {
                m_shouldGoLeft = true;
            }
            if (keyEvent.keyCode == KeyCode::KEY_D) {
                m_shouldGoRight = true;
            }
            if (keyEvent.keyCode == KeyCode::KEY_Space) {
                if (!m_isInTheAir) {
                    m_upwardSpeed = JUMP_POWER;
                    m_isInTheAir = true;
                }
            }
        } else if (keyEvent.action == KeyActionType::RELEASE) {
            if (keyEvent.keyCode == KeyCode::KEY_W) {
                m_shouldGoForward = false;
            }
            if (keyEvent.keyCode == KeyCode::KEY_S) {
                m_shouldGoBackward = false;
            }
            if (keyEvent.keyCode == KeyCode::KEY_A) {
                m_shouldGoLeft = false;
            }
            if (keyEvent.keyCode == KeyCode::KEY_D) {
                m_shouldGoRight = false;
            }
        }
    }

    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.button == MouseButtonType::LEFT) {
            if (mouseEvent.action == MouseActionType::PRESS) {
                m_shouldRotate = true;
            } else if (mouseEvent.action == MouseActionType::RELEASE) {
                m_shouldRotate = false;
            } else if (m_shouldRotate && mouseEvent.action == MouseActionType::MOVE) {
                m_rotationAroundY = mouseEvent.position.x;
                m_pitchDiff = mouseEvent.position.y;
            }
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
#if defined(__ANDROID__)
        if (touchEvent.action == TouchActionType::DOWN) {
            const float MAX_RATIO_FOR_JUMP_CONTROL = 0.25f;
            const auto MAX_X = touchEvent.extent.x * MAX_RATIO_FOR_JUMP_CONTROL;
            const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_JUMP_CONTROL;
            if (touchEvent.position.x < MAX_X && touchEvent.position.y < MAX_Y) {
                if (!m_isInTheAir) {
                    m_upwardSpeed = JUMP_POWER;
                    m_isInTheAir = true;
                }
            }
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            const float MAX_RATIO_FOR_MOVE_CONTROL = 0.35f;
            const auto MIN_X = touchEvent.extent.x - touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MIN_Y = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            if (touchEvent.position.x > MIN_X && touchEvent.position.y < MAX_Y) {
                m_shouldGoForward = true;
            }

            if (touchEvent.position.x > MIN_X && touchEvent.position.y > MIN_Y) {
                m_shouldGoBackward = true;
            }
        } else {
            m_shouldGoForward = false;
            m_shouldGoBackward = false;
            return;
        }
#endif
        if (touchEvent.action == TouchActionType::MOVE) {
            const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * 0.1f;

            m_transformComponent->Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees.x), glm::vec3(0.0f, 0.0f, 1.0f))));

            m_cameraComponent->AddYaw(angleInDegrees.x);
            m_cameraComponent->AddPitch(angleInDegrees.y);
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            m_prevTouchPosition = touchEvent.position;
        }
    }

    void operator()(const MouseScrollEvent& scrollEvent)
    {
        m_distanceFromPerson += scrollEvent.delta;
    }

private:
    const float RUN_SPEED{ 14.0f };

    const float YAW_TURN_SPEED{ 3.0f };

    const float PITCH_TURN_SPEED{ 0.5f };

    const float GRAVITY_Y{ -5.0f };

    const float JUMP_POWER{ 2.5f };

    const float MIN_Y_POS{ 9.0f };

private:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    bool m_shouldGoForward{ false };

    bool m_shouldGoBackward{ false };

    bool m_shouldGoLeft{ false };

    bool m_shouldGoRight{ false };

    bool m_shouldRotate{ false };

    float m_upwardSpeed{ 0.0f };

    float m_rotationAroundY{ 0.0f };

    float m_pitchDiff{ 0.0f };

    bool m_isInTheAir{ false };

    float m_cameraPitch{ -20.0f };

    float m_distanceFromPerson{ 50.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    EventHandler<Goblin, KeyEvent> m_keyboardEventsHandler{ *this };

    EventHandler<Goblin, MouseEvent> m_mouseEventsHandler{ *this };

    EventHandler<Goblin, TouchEvent> m_touchEventsHandler{ *this };

    EventHandler<Goblin, MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<IAnimationRenderComponent> m_animatonRenderComponent;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class Camera : public AbstractSceneNode<SceneNodeFlags> {
public:
    Camera()
        : AbstractSceneNode()
    {
    }

    virtual ~Camera() = default;

private:
    void Reset()
    {
        m_cameraComponent->Reset();

        m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
#if defined(__ANDROID__)
        m_autoMoveForward = false;
        m_autoMoveBackward = false;
#endif
    }

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
        NodeComponentHelper::AddComponent<SceneNodeFlags, ICameraComponent>(GetThis(), m_cameraComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        AbstractSceneNode::Init();

        Reset();
    }

    void Update(float deltaTime) override
    {
        glm::vec3 positionDelta{ 0.0f, 0.0f, 0.0f };
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_W)) {
            positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_S)) {
            positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_A)) {
            positionDelta -= m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_D)) {
            positionDelta += m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_Q)) {
            positionDelta -= m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(KeyCode::KEY_E)) {
            positionDelta += m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }

#if defined(__ANDROID__)
        if (m_autoMoveForward) {
            positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }

        if (m_autoMoveBackward) {
            positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
#endif

        m_cameraComponent->AddPosition(positionDelta);

        glm::mat4 viewMatrix = m_cameraComponent->LookAt();
        glm::mat4 cameraTransformInWorldSpace = glm::inverse(viewMatrix);

        m_transformComponent->SetPosition(MathUtil::ExtractTranslation(cameraTransformInWorldSpace));
        m_transformComponent->SetOrientation(MathUtil::ExtractOrientation(cameraTransformInWorldSpace));

        m_transformComponent->Update(deltaTime);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ICameraComponent>(GetThis(), SceneNodeFlags::CAMERA_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

public:
    void operator()(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.action == MouseActionType::MOVE && mouseEvent.button == MouseButtonType::LEFT) {
            const glm::vec2 angleInDegrees = mouseEvent.position * m_sensitivity;

            const float newPitch = m_pitchAngle + angleInDegrees.y;
            if (newPitch > -m_absMinMaxPitch && newPitch < m_absMinMaxPitch) {
                m_cameraComponent->AddPitch(angleInDegrees.y);
                m_pitchAngle += angleInDegrees.y;
            }
            m_cameraComponent->AddYaw(angleInDegrees.x);
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
#if defined(__ANDROID__)
        const float MAX_RATIO_FOR_MOVE_CONTROL = 0.25; //
        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            const auto MAX_X_COORD_TO_CONTROL = touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MAX_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            const auto MIN_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
            if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y < MAX_Y_COORD_TO_BACKWARD_CONTROL) {
                m_autoMoveForward = true;
            }

            if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y > MIN_Y_COORD_TO_BACKWARD_CONTROL) {
                m_autoMoveBackward = true;
            }
        } else {
            m_autoMoveForward = false;
            m_autoMoveBackward = false;
            return;
        }
#endif
        if (touchEvent.action == TouchActionType::MOVE) {
            const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * m_sensitivity;

            m_cameraComponent->AddPitch(angleInDegrees.y);
            m_cameraComponent->AddYaw(angleInDegrees.x);
        }

        if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
            m_prevTouchPosition = touchEvent.position;
        }
    }

    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_R) {
                Reset();
            }
        }
    }

private:
    EventHandler<Camera, MouseEvent> m_mouseHandler{ *this };

    EventHandler<Camera, TouchEvent> m_touchHandler{ *this };

    EventHandler<Camera, KeyEvent> m_keyHandler{ *this };

private:
    const float m_sensitivity{ 0.05f };

    const float m_moveSpeed{ 25.0f };

    const float m_absMinMaxPitch{ 89.0f };

    float m_pitchAngle{ 0.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    InputsFacade m_inputFacade;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

#if defined(__ANDROID__)
    bool m_autoMoveForward = false;

    bool m_autoMoveBackward = false;
#endif
};

class Text : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<IFontRenderComponent> m_fontComponent;

    FPSService m_fpsService{ 1.0f, false };

public:
    Text()
        : AbstractSceneNode()
    {
    }

    ~Text() = default;

public:
    void Init() override
    {
        FontRenderComponentsFactory factory{};
        m_fontComponent = factory.Create(AssetManager::Instance().GetAssetPath("Fonts/verdana.fnt"), AssetManager::Instance().GetAssetPath("Fonts/verdana.png"), 16.0f / 9.0f);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IFontRenderComponent>(GetThis(), m_fontComponent, SceneNodeFlags::FONT_RENDER_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if (m_fpsService.Update(deltaTime)) {
            m_fontComponent->Reset();

            std::stringstream fpsString;
            fpsString << std::setprecision(1) << std::fixed;
            fpsString << m_fpsService.GetAverageFPS() << " FPS";

            auto fancyText = std::make_shared<FancyText>(fpsString.str(), 1.6f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0), glm::vec2(0.4f, -0.4f), 1.0f, true, 0.5f, 0.05f);
            m_fontComponent->AddText(fancyText);
        }

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IFontRenderComponent>(GetThis(), SceneNodeFlags::FONT_RENDER_COMPONENT);
    }
};

class MainLight : public AbstractSceneNode<SceneNodeFlags> {
public:
    MainLight(const glm::vec3& pos)
        : AbstractSceneNode()
        , m_initialPosition(pos)
    {
    }

    ~MainLight() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition);
        NodeComponentHelper::AddComponent<SceneNodeFlags, ILightComponent>(GetThis(), m_lightComponent, SceneNodeFlags::LIGHT_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const float ROTATION_SPEED_DEG_PER_SEC = 5.0f;
        const float ROTATION_ANGLE = ROTATION_SPEED_DEG_PER_SEC * deltaTime;

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::translate(transform, m_lightComponent->GetPosition());

        glm::vec3 position{ transform[3][0], transform[3][1], transform[3][2] };

        m_lightComponent->SetPosition(position);

        auto lightTransformInWorldSpace = glm::inverse(m_lightComponent->LookAt());

        m_transformComponent->SetPosition(MathUtil::ExtractTranslation(lightTransformInWorldSpace));
        m_transformComponent->SetOrientation(MathUtil::ExtractOrientation(lightTransformInWorldSpace));

        m_transformComponent->Update(deltaTime);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ILightComponent>(GetThis(), SceneNodeFlags::LIGHT_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;
};

class Light : public AbstractSceneNode<SceneNodeFlags> {
public:
    Light(const glm::vec3& position, const glm::vec3& color)
        : AbstractSceneNode()
        , m_initialPosition(position)
        , m_color(color)
    {
    }

    ~Light() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition, m_color, glm::vec3(0.1f, 0.005f, 0.001f));
        NodeComponentHelper::AddComponent<SceneNodeFlags, ILightComponent>(GetThis(), m_lightComponent, SceneNodeFlags::LIGHT_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_transformComponent->SetPosition(m_lightComponent->GetPosition());
        m_transformComponent->Update(deltaTime);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ILightComponent>(GetThis(), SceneNodeFlags::LIGHT_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    const glm::vec3 m_initialPosition;

    const glm::vec3 m_color;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<ILightComponent> m_lightComponent;
};

class SkyBox : public AbstractSceneNode<SceneNodeFlags> {
public:
    SkyBox()
        : AbstractSceneNode()
    {
    }

    ~SkyBox() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        SkyBoxComponentFactory factory{};
        m_skyBoxComponent = factory.Create();
        NodeComponentHelper::AddComponent<SceneNodeFlags, ISkyBoxComponent>(GetThis(), m_skyBoxComponent, SceneNodeFlags::SKYBOX_RENDER_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>(TagSet{ TAG_MAIN_CAMERA });

        const float ROTATION_ANGLE = ROTATION_SPEED_DEGS_PER_SEC * deltaTime;

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));

        m_transformComponent->Rotate(glm::quat_cast(transform));
        m_transformComponent->SetPosition(cameraComponent->GetPosition());
        m_transformComponent->SetScale(glm::vec3(SKY_BOX_SIZE));

        m_transformComponent->Update(deltaTime);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ISkyBoxComponent>(GetThis(), SceneNodeFlags::SKYBOX_RENDER_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<ISkyBoxComponent> m_skyBoxComponent;

    static const inline float ROTATION_SPEED_DEGS_PER_SEC = 0.5f;
};

class WaterReflection : public AbstractSceneNode<SceneNodeFlags> {
public:
    WaterReflection()
        : AbstractSceneNode()
    {
    }

    ~WaterReflection() = default;

public:
    void Init() override
    {
        m_viewPortSize = m_previousViewPortSize;
        CreateReflectionComponent();

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if (m_viewPortSize != m_previousViewPortSize) {
            DestroyReflectionComponent();
            CreateReflectionComponent();
            m_previousViewPortSize = m_viewPortSize;
        }

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        DestroyReflectionComponent();
    }

public:
    void operator()(const NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

private:
    void CreateReflectionComponent()
    {
        WaterComponentFactory componentFactory{};
        m_reflectionComponent = std::move(componentFactory.CreateOffScreenComponent(m_viewPortSize.x / REFLECTION_EXTENT_DIVIDER, m_viewPortSize.y / REFLECTION_EXTENT_DIVIDER));
        m_reflectionComponent->Init();
        NodeComponentHelper::AddComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(GetThis(), m_reflectionComponent, SceneNodeFlags::WATER_REFLECTION_RENDER_COMPONENT);
    }

    void DestroyReflectionComponent()
    {
        if (m_reflectionComponent) {
            NodeComponentHelper::RemoveComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(GetThis(), SceneNodeFlags::WATER_REFLECTION_RENDER_COMPONENT);
            m_reflectionComponent->ShutDown();
        }
    }

private:
    std::shared_ptr<IWaterOffscreenRenderPassComponent> m_reflectionComponent;

    glm::uvec2 m_viewPortSize{ 0, 0 };

    glm::uvec2 m_previousViewPortSize{ 1920, 1080 };

private:
    EventHandler<WaterReflection, NewIterationEvent> m_newIterationHandler{ *this };
};

class WaterRefraction : public AbstractSceneNode<SceneNodeFlags> {
public:
    WaterRefraction()
        : AbstractSceneNode()
    {
    }

    ~WaterRefraction() = default;

public:
    void Init() override
    {
        m_viewPortSize = m_previousViewPortSize;
        CreateRefractionComponent();

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if (m_viewPortSize != m_previousViewPortSize) {
            DestroyRefractionComponent();
            CreateRefractionComponent();
            m_previousViewPortSize = m_viewPortSize;
        }

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        DestroyRefractionComponent();
    }

public:
    void operator()(const NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

private:
    void CreateRefractionComponent()
    {
        WaterComponentFactory componentFactory{};
        m_refractionComponent = std::move(componentFactory.CreateOffScreenComponent(m_viewPortSize.x / REFRACTION_EXTENT_DIVIDER, m_viewPortSize.y / REFRACTION_EXTENT_DIVIDER));
        m_refractionComponent->Init();
        NodeComponentHelper::AddComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(GetThis(), m_refractionComponent, SceneNodeFlags::WATER_REFRACTION_RENDER_COMPONENT);
    }

    void DestroyRefractionComponent()
    {
        if (m_refractionComponent) {
            NodeComponentHelper::RemoveComponent<SceneNodeFlags, IWaterOffscreenRenderPassComponent>(GetThis(), SceneNodeFlags::WATER_REFRACTION_RENDER_COMPONENT);
            m_refractionComponent->ShutDown();
        }
    }

private:
    std::shared_ptr<IWaterOffscreenRenderPassComponent> m_refractionComponent;

    glm::uvec2 m_viewPortSize{ 0, 0 };

    glm::uvec2 m_previousViewPortSize{ 1920, 1080 };

private:
    EventHandler<WaterRefraction, NewIterationEvent> m_newIterationHandler{ *this };
};

class Water : public AbstractSceneNode<SceneNodeFlags> {
public:
    Water(const int x, const int z)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::WATER_RENDER_COMPONENT | SceneNodeFlags::TRANSFORM_COMPONENT | SceneNodeFlags::BOUNDING_VOLUME_COMPONENT })
        , m_x(x)
        , m_z(z)
    {
    }

    ~Water() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create();
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        WaterComponentFactory componentFactory{};
        m_waterComponent = std::move(componentFactory.Create(m_x, m_z));
        NodeComponentHelper::AddComponent<SceneNodeFlags, IWaterComponent>(GetThis(), m_waterComponent, SceneNodeFlags::WATER_RENDER_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_waterComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_waterComponent->Update(deltaTime);

        m_transformComponent->SetPosition(m_waterComponent->GetPosition());
        m_transformComponent->SetScale(glm::vec3(WATER_TILE_SIZE));

        m_transformComponent->Update(deltaTime);

        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IWaterComponent>(GetThis(), SceneNodeFlags::WATER_RENDER_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    const int m_x;

    const int m_z;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<IWaterComponent> m_waterComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class WaterManager : public AbstractSceneNode<SceneNodeFlags> {
private:
    const int m_gridMaxX;

    const int m_gridMaxZ;

public:
    WaterManager(const int maxX, const int maxZ)
        : AbstractSceneNode()
        , m_gridMaxX(maxX)
        , m_gridMaxZ(maxZ)
    {
    }

    virtual ~WaterManager() = default;

public:
    void Init() override
    {
        for (int x = 0; x < m_gridMaxX; x++) {
            for (int z = 0; z < m_gridMaxZ; z++) {
                auto terrain = std::make_shared<Water>(x, z);
                AddChild(terrain);
            }
        }

        auto waterReflection = std::make_shared<WaterReflection>();
        AddChild(waterReflection);

        auto waterRefraction = std::make_shared<WaterRefraction>();
        AddChild(waterRefraction);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();
    }
};

class LensFlare : public AbstractSceneNode<SceneNodeFlags> {
public:
    LensFlare()
        : AbstractSceneNode()
    {
    }

    ~LensFlare() = default;

public:
    void Init() override
    {
        LensFlareComponentFactory componentFactory{};
        m_lensFlareComponent = std::move(componentFactory.Create());
        NodeComponentHelper::AddComponent<SceneNodeFlags, ILensFlareComponent>(GetThis(), m_lensFlareComponent, SceneNodeFlags::LENS_FLARE_RENDER_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        m_lensFlareComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ILensFlareComponent>(GetThis(), SceneNodeFlags::LENS_FLARE_RENDER_COMPONENT);
    }

public:
    void operator()(const NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

private:
    std::shared_ptr<ILensFlareComponent> m_lensFlareComponent;

    glm::vec2 m_viewPortSize;

private:
    EventHandler<LensFlare, NewIterationEvent> m_newIterationHandler{ *this };
};

class Sun : public AbstractSceneNode<SceneNodeFlags> {
public:
    Sun()
        : AbstractSceneNode()
    {
    }

    ~Sun() = default;

public:
    void Init() override
    {
        SunComponentFactory componentFactory{};
        m_sunComponent = std::move(componentFactory.Create());
        NodeComponentHelper::AddComponent<SceneNodeFlags, ISunComponent>(GetThis(), m_sunComponent, SceneNodeFlags::SUN_RENDER_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        m_sunComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ISunComponent>(GetThis(), SceneNodeFlags::SUN_RENDER_COMPONENT);
    }

public:
    void operator()(const NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

private:
    std::shared_ptr<ISunComponent> m_sunComponent;

    glm::vec2 m_viewPortSize;

private:
    EventHandler<Sun, NewIterationEvent> m_newIterationHandler{ *this };
};

class Stone : public AbstractSceneNode<SceneNodeFlags> {
public:
    Stone(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : AbstractSceneNode()
        , m_initialPosition(position)
        , m_initialOrientation(orientation)
        , m_initialScale(scale)
    {
    }

    ~Stone() = default;

public:
    void Init() override
    {
        TrasnformComponentFactory transformComponentFactory{};
        m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, ITransformComponent>(GetParent())) {
            m_transformComponent->SetParent(NodeComponentHelper::GetComponent<SceneNodeFlags, ITransformComponent>(GetParent()));
        }
        NodeComponentHelper::AddComponent<SceneNodeFlags, ITransformComponent>(GetThis(), m_transformComponent, SceneNodeFlags::TRANSFORM_COMPONENT);

        RenderComponentFactory componentFactory{};
        std::shared_ptr<IRenderComponent> renderComponent = componentFactory.CreateModelParallaxMappedRenderComponent(AssetManager::Instance().GetAssetPath("Models/Boulder/boulder.dae"), AssetManager::Instance().GetAssetPath("Models/Boulder/boulder.png"), AssetManager::Instance().GetAssetPath("Models/Boulder/boulder_normal.png"), AssetManager::Instance().GetAssetPath("Models/Boulder/boulder_height.png"), true, true);
        renderComponent->GetMaterial()->SetHeightScale(0.01f);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IRenderComponent>(GetThis(), renderComponent, SceneNodeFlags::RENDER_PARALLAX_MAPPED_COMPONENT);

        BoundingVolumeComponentFactory bondingVolumeFactory{};
        m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(renderComponent->GetModel()->GetMesh()->GetVertices());
        NodeComponentHelper::AddComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);

        const auto selectableComponent = std::make_shared<SelectableComponent>();
        NodeComponentHelper::AddComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), selectableComponent, SceneNodeFlags::SELECTABLE_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto terrain = NodeComponentHelper::FindOne<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });

        auto currentPosition = m_transformComponent->GetPosition();

        float height = 0.0f;
        terrain->GetHeightAt(currentPosition, height);

        m_transformComponent->SetPosition(glm::vec3(currentPosition.x, height - 2.0f, currentPosition.z));

        m_transformComponent->Update(deltaTime);

        m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ISelectableComponent>(GetThis(), SceneNodeFlags::SELECTABLE_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IBoundingVolumeComponent>(GetThis(), SceneNodeFlags::BOUNDING_VOLUME_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IRenderComponent>(GetThis(), SceneNodeFlags::RENDER_PARALLAX_MAPPED_COMPONENT);
        NodeComponentHelper::RemoveComponent<SceneNodeFlags, ITransformComponent>(GetThis(), SceneNodeFlags::TRANSFORM_COMPONENT);
    }

private:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    std::shared_ptr<ITransformComponent> m_transformComponent;

    std::shared_ptr<IBoundingVolumeComponent> m_boundingVolumeComponent;
};

class Shadows : public AbstractSceneNode<SceneNodeFlags> {
public:
    Shadows()
        : AbstractSceneNode()
    {
    }

    virtual ~Shadows() = default;

public:
    void Init() override
    {
        ShadowsComponentFactory shadowsFactory{};
        m_shadowsCompoent = shadowsFactory.Create();
        m_shadowsCompoent->Init();
        NodeComponentHelper::AddComponent<SceneNodeFlags, IShadowsComponent>(GetThis(), m_shadowsCompoent, SceneNodeFlags::SHADOWS_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        m_shadowsCompoent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum().GetNearClippingPlane(), lightComponent->GetViewFrustum().GetFarClippingPlane(), lightComponent->GetViewFrustum().CreateProjectionMatrix(1.0f), cameraComponent->LookAt());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IShadowsComponent>(GetThis(), SceneNodeFlags::SHADOWS_COMPONENT);

        m_shadowsCompoent->ShutDown();
    }

private:
    std::shared_ptr<IShadowsComponent> m_shadowsCompoent;
};

class RayCasterNode : public AbstractSceneNode<SceneNodeFlags> {
public:
    RayCasterNode()
        : AbstractSceneNode()
    {
    }

    ~RayCasterNode() = default;

public:
    void Init() override
    {
        RayCasterComponentFactory raycasterFactory{};
        m_rayCasterComponent = raycasterFactory.CreateRayCaster();
        m_mouseRayCasterComponent = raycasterFactory.CreateMouseRayCaster();

        AddRayCastComponent(m_inputFacade.IsMouseLocked());

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });
        const auto playerTransformComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ITransformComponent>({ TAG_PLAYER });

        if (m_inputFacade.IsMouseLocked()) {
            m_rayCasterComponent->SetRayLength(RAY_LENGTH);
            m_rayCasterComponent->SetRayStartPosition(playerTransformComponent->GetPosition() + glm::vec3(0.0f, 12.0f, 0.0f));
            m_rayCasterComponent->SetRayDirection(cameraComponent->GetForwardDirection());
            m_rayCasterComponent->SetOrientationOffsetAngles({ -12.0f, 0.0f });
            m_rayCasterComponent->Update(deltaTime);
        } else {
            m_mouseRayCasterComponent->SetRayLength(RAY_LENGTH);
            m_mouseRayCasterComponent->SetViewPortDimensions(m_viewPortSize);
            m_mouseRayCasterComponent->SetProjectionMatrix(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y));
            m_mouseRayCasterComponent->SetViewMatrix(cameraComponent->LookAt());
            m_mouseRayCasterComponent->SetRayStartPosition(cameraComponent->GetPosition());
            m_mouseRayCasterComponent->Update(deltaTime);
        }
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        RemoveRayCastComponnet();
    }

private:
    void RemoveRayCastComponnet()
    {
        if (NodeComponentHelper::HasComponent<SceneNodeFlags, IRayCasterComponent>(GetThis())) {
            NodeComponentHelper::RemoveComponent<SceneNodeFlags, IRayCasterComponent>(GetThis(), SceneNodeFlags::RAYCASTER_COMPONENT);
        }
    }

    void AddRayCastComponent(const bool mouseLocked)
    {
        if (mouseLocked) {
            NodeComponentHelper::AddComponent<SceneNodeFlags, IRayCasterComponent>(GetThis(), m_rayCasterComponent, SceneNodeFlags::RAYCASTER_COMPONENT);
        } else {
            NodeComponentHelper::AddComponent<SceneNodeFlags, IRayCasterComponent>(GetThis(), m_mouseRayCasterComponent, SceneNodeFlags::RAYCASTER_COMPONENT);
        }
    }

public:
    void operator()(const NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

    void operator()(const MouseLockRequest& lockRequest)
    {
        RemoveRayCastComponnet();
        AddRayCastComponent(lockRequest.lock);
    }

private:
    const float RAY_LENGTH = 200.0f;

    std::shared_ptr<IRayCasterComponent> m_rayCasterComponent;

    std::shared_ptr<IMouseRayCasterComponent> m_mouseRayCasterComponent;

    glm::vec2 m_viewPortSize;

    InputsFacade m_inputFacade;

private:
    EventHandler<RayCasterNode, NewIterationEvent> m_newIterationHandler{ *this };

    EventHandler<RayCasterNode, MouseLockRequest> m_mouseLockHandler{ *this };
};

class RayCastObserverNode : public AbstractSceneNode<SceneNodeFlags> {
private:
    enum class IntersectionType {
        NONE,
        TERRAIN,
        OBJECT
    };

public:
    RayCastObserverNode()
        : AbstractSceneNode()
    {
    }

    ~RayCastObserverNode() = default;

public:
    void Init() override
    {
        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if (!m_currentRay.IsNull()) {

            // reset any selectable nodes here -> eventhough it does not have bounding volume flag
            auto selectableNodes = GetSelectableNodes();
            for (auto selectableNode : selectableNodes) {
                auto selectableComponent = ComponentRepository<ISelectableComponent>::Instance().Get(selectableNode->GetId());
                selectableComponent->Reset();
            }

            IntersectionType intersectionType = IntersectionType::NONE;

            const auto currentRayValue = m_currentRay.GetValue();
            const auto currentTerrainIntersectionPoint = FindTheClosestTerrainIntersection(currentRayValue);
            const auto closestIntersectingObject = FindTheClosestIntersectingNode(currentRayValue);
            if (!currentTerrainIntersectionPoint.IsNull() && !closestIntersectingObject.IsNull()) {
                const float terrainRayLength = glm::distance(currentTerrainIntersectionPoint.GetValue(), currentRayValue.origin);
                const float objectRayLength = std::get<1>(closestIntersectingObject.GetValue()).t;
                if (terrainRayLength < objectRayLength) {
                    intersectionType = IntersectionType::TERRAIN;
                } else {
                    intersectionType = IntersectionType::OBJECT;
                }
            } else {
                if (!currentTerrainIntersectionPoint.IsNull()) {
                    intersectionType = IntersectionType::TERRAIN;
                } else if (!closestIntersectingObject.IsNull()) {
                    intersectionType = IntersectionType::OBJECT;
                }
            }

            if (intersectionType == IntersectionType::TERRAIN) {
                auto terrainManagerNode = GraphTraversal<SceneNodeFlags>::Instance().FindOneWithFlags(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT | SceneNodeFlags::SELECTABLE_COMPONENT }, LogicOperation::AND);
                auto selectableComponent = ComponentRepository<ISelectableComponent>::Instance().Get(terrainManagerNode->GetId());
                selectableComponent->SetSelected(true);
                selectableComponent->SetPosition(currentTerrainIntersectionPoint.GetValue());
                //std::cout << "Terrain Intersection At: " << currentTerrainIntersectionPoint.GetValue().x << ", " << currentTerrainIntersectionPoint.GetValue().y << ", " << currentTerrainIntersectionPoint.GetValue().z << std::endl;
            } else if (intersectionType == IntersectionType::OBJECT) {
                const auto node = std::get<0>(closestIntersectingObject.GetValue());
                const auto rayCastResult = std::get<1>(closestIntersectingObject.GetValue());
                auto selectableComponent = ComponentRepository<ISelectableComponent>::Instance().Get(node->GetId());
                selectableComponent->SetSelected(true);
                selectableComponent->SetPosition(rayCastResult.point);
            }

            // TODO
            // render node collision points -> it is stored in selectableComponent
        }

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();
    }

private:
    // Terrain
    Nullable<glm::vec3> FindTheClosestTerrainIntersection(const Ray& ray) const
    {
        Nullable<glm::vec3> currentTerrainIntersectionPoint{};
        if (IntersectsInRange(0.0f, ray.length, ray)) {
            currentTerrainIntersectionPoint = BinarySearch(0, 0, ray.length, ray);
        } else {
            currentTerrainIntersectionPoint = GetFirstPositionUnderAlongRay(ray);
        }
        return currentTerrainIntersectionPoint;
    }

    Nullable<glm::vec3> GetFirstPositionUnderAlongRay(const Ray& ray) const
    {
        const auto segmentPositions = GenerateSegmentPositions(ray);
        for (const auto& segmentPosition : segmentPositions) {
            const auto terrain = GetTerrain(segmentPosition);
            if (terrain != nullptr) {
                float currentTerrainHeight = 0.0f;
                if (!terrain->GetHeightAt(segmentPosition, currentTerrainHeight)) {
                    continue;
                }
                if (segmentPosition.y < currentTerrainHeight) {
                    return Nullable<glm::vec3>({ segmentPosition.x, currentTerrainHeight, segmentPosition.z });
                }
            }
        }
        return Nullable<glm::vec3>();
    }

    std::vector<glm::vec3> GenerateSegmentPositions(const Ray& ray) const
    {
        const float distanceBetweenNodes = ray.length / RECURSION_COUNT;
        const glm::vec3 start = ray.GetStartPoint();
        const glm::vec3 end = ray.GetEndPoint();

        std::vector<glm::vec3> result;
        float currentDistance = 0.0f;
        while (currentDistance < ray.length) {
            glm::vec3 newNodePoint = ray.GetPointAtDistances(currentDistance);
            result.push_back(newNodePoint);
            currentDistance += distanceBetweenNodes;
        }
        return result;
    }

    Nullable<glm::vec3> BinarySearch(const uint32_t count, const float start, const float finish, const Ray& ray) const
    {
        const float half = start + ((finish - start) / 2.0f);
        if (count >= RECURSION_COUNT) {
            const glm::vec3 endPoint = ray.GetPointAtDistances(half);
            const auto terrain = GetTerrain(endPoint);
            if (terrain != nullptr) {
                return Nullable<glm::vec3>(endPoint);
            } else {
                return Nullable<glm::vec3>();
            }
        }

        if (IntersectsInRange(start, half, ray)) {
            return BinarySearch(count + 1, start, half, ray);
        } else {
            return BinarySearch(count + 1, half, finish, ray);
        }
    }

    bool IntersectsInRange(const float start, const float finish, const Ray& ray) const
    {
        const glm::vec3 startPoint = ray.GetPointAtDistances(start);
        const glm::vec3 endPoint = ray.GetPointAtDistances(finish);

        const auto terrainAtStart = GetTerrain(startPoint);
        if (!terrainAtStart) {
            return false;
        }

        const auto terrainAtEnd = GetTerrain(endPoint);
        if (!terrainAtEnd) {
            return false;
        }

        if (!IsUnderGround(terrainAtStart, startPoint, false) && IsUnderGround(terrainAtEnd, endPoint, true)) {
            return true;
        }
        return false;
    }

    bool IsUnderGround(const std::shared_ptr<ITerrainComponenet>& terrain, const glm::vec3& testPoint, bool shouldReturn) const
    {
        float height = 0.0f;
        if (!terrain->GetHeightAt(testPoint, height)) {
            return !shouldReturn;
        }
        if (testPoint.y < height) {
            return true;
        }
        return false;
    }

    std::shared_ptr<ITerrainComponenet> GetTerrain(const glm::vec3& position) const
    {
        const auto terrainManager = NodeComponentHelper::FindOne<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });
        return terrainManager->GetTerrainAt(position);
    }

    // Objects
    std::vector<std::shared_ptr<ISceneNode<SceneNodeFlags> > > GetSelectableNodes() const
    {
        return GraphTraversal<SceneNodeFlags>::Instance().FindAllWithFlags(FlagSet<SceneNodeFlags>{ SceneNodeFlags::SELECTABLE_COMPONENT });
    }

    Nullable<std::tuple<std::shared_ptr<ISceneNode<SceneNodeFlags> >, RayCastResult> > FindTheClosestIntersectingNode(const Ray& ray) const
    {
        Nullable<std::tuple<std::shared_ptr<ISceneNode<SceneNodeFlags> >, RayCastResult> > theClosestNode;
        float minDistance = std::numeric_limits<float>::max();

        auto selectableNodes = GraphTraversal<SceneNodeFlags>::Instance().FindAllWithFlags(FlagSet<SceneNodeFlags>{ SceneNodeFlags::SELECTABLE_COMPONENT | SceneNodeFlags::BOUNDING_VOLUME_COMPONENT }, LogicOperation::AND);
        for (auto selectable : selectableNodes) {
            const auto boundingVolume = ComponentRepository<IBoundingVolumeComponent>::Instance().Get(selectable->GetId());

            RayCastResult rayCastResult{};
            if (boundingVolume->Intersects(ray, rayCastResult)) {
                if (rayCastResult.t < minDistance) {
                    theClosestNode = Nullable<std::tuple<std::shared_ptr<ISceneNode<SceneNodeFlags> >, RayCastResult> >({ selectable, rayCastResult });
                    minDistance = rayCastResult.t;
                }
            }
        }
        return theClosestNode;
    }

public:
    void operator()(const RayEvent& rayEvt)
    {
        m_currentRay = Nullable<Ray>{ rayEvt.ray };
    }

private:
    const uint32_t RECURSION_COUNT = 200;

    Nullable<Ray> m_currentRay; // make it nullable???

    EventHandler<RayCastObserverNode, RayEvent> m_rayHandler{ *this };
};

class Fire final : public AbstractSceneNode<SceneNodeFlags>{
public:
    Fire(const glm::vec3& initPosition)
        : m_initialPosition(initPosition)
    {
    }

public:
    void Init() override
    {
        ParticleSystemComponentFactory particleSystemComponentFactory{};
        m_particleSystemComponent = particleSystemComponentFactory.CreateRandomInCone(glm::vec3(0.0f, 1.0f, 0.0f), 15.0f);
        NodeComponentHelper::AddComponent<SceneNodeFlags, IParticleSystemComponent>(GetThis(), m_particleSystemComponent, SceneNodeFlags::PARTICLE_SYSTEM_COMPONENT);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto cameraComponent = NodeComponentHelper::FindOne<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });
        const auto terrain = NodeComponentHelper::FindOne<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::TERRAIN_MANAGER_COMPONENT });

        float height = 0.0f;
        terrain->GetHeightAt(m_initialPosition, height);

        m_particleSystemComponent->Update(deltaTime, glm::vec3(m_initialPosition.x, height - 4.0f, m_initialPosition.z));

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        NodeComponentHelper::RemoveComponent<SceneNodeFlags, IParticleSystemComponent>(GetThis(), SceneNodeFlags::PARTICLE_SYSTEM_COMPONENT);
    }

private:
    const glm::vec3 m_initialPosition;

    std::shared_ptr<IParticleSystemComponent> m_particleSystemComponent;
};

class RootSceneNode : public AbstractSceneNode<SceneNodeFlags> {
public:
    RootSceneNode(const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<Swapchain>& swapchain)
        : AbstractSceneNode()
        , m_masterRenderer(std::make_unique<MasterRenderer>(renderPass, swapchain))
    {
    }

    virtual ~RootSceneNode() = default;

public:
    void Init() override
    {
        // Init scene nodes
        auto inputsHelper = std::make_shared<InputsHelper>();
        AddChild(inputsHelper);

        auto rayCaster = std::make_shared<RayCasterNode>();
        AddChild(rayCaster);

        auto rayCastObserver = std::make_shared<RayCastObserverNode>();
        AddChild(rayCastObserver);

        auto skyBox = std::make_shared<SkyBox>();
        AddChild(skyBox);

        auto sunLight = std::make_shared<MainLight>(glm::vec3(150.0f, 50.0f, 150.0f));
        sunLight->SetTags({ TAG_MAIN_LIGHT, TAG_LIGHT });
        AddChild(sunLight);

        //auto light1 = std::make_shared<Light>(glm::vec3(30.0f, 20.0f, 35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //light1->SetTags({ TAG_LIGHT });
        //AddChild(light1);
        //
        //auto light2 = std::make_shared<Light>(glm::vec3(-30.0f, 20.0f, 35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //light2->SetTags({ TAG_LIGHT });
        //AddChild(light2);
        //
        //auto light3 = std::make_shared<Light>(glm::vec3(0.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //light3->SetTags({ TAG_LIGHT });
        //AddChild(light3);

        auto shadows = std::make_shared<Shadows>();
        shadows->SetTags({ TAG_SHADOW });
        AddChild(shadows);

        //auto freeCamera = std::make_shared<Camera>();
        //freeCamera->SetTags({ TAG_MAIN_CAMERA });
        //AddChild(freeCamera);

        //auto camRobot = std::make_shared<CubeRobot>(glm::vec3(1.0f, -0.4f, -1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        //freeCamera->AddChild(camRobot);

        const int32_t MAX_GENERATED_HEIGHT = 1;
        const float DISTANCE = 40.0f;

        for (int32_t i = 0; i <= MAX_GENERATED_HEIGHT; i++) {
            for (int32_t j = 0; j <= MAX_GENERATED_HEIGHT; j++) {
                for (int32_t k = 0; k <= MAX_GENERATED_HEIGHT; k++) {
                    auto robot = std::make_shared<CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
                    AddChild(robot);
                }
            }
        }

        auto goblin = std::make_shared<Goblin>(glm::vec3(0.0f), glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f))), glm::vec3(0.005f));
        goblin->SetTags({ TAG_MAIN_CAMERA, TAG_PLAYER });
        AddChild(goblin);

        auto text = std::make_shared<Text>();
        AddChild(text);

        const uint32_t TERRAIN_GRID_MAX_X = 3;
        const uint32_t TERRAIN_GRID_MAX_Z = 3;

        auto terrainManager = std::make_shared<TerrainManager>(TERRAIN_GRID_MAX_X, TERRAIN_GRID_MAX_Z);
        AddChild(terrainManager);

        auto water = std::make_shared<WaterManager>(TERRAIN_GRID_MAX_X, TERRAIN_GRID_MAX_Z);
        AddChild(water);

        auto sun = std::make_shared<Sun>();
        AddChild(sun);

        auto lensFlare = std::make_shared<LensFlare>();
        AddChild(lensFlare);

        const float ITEMS_TERRAIN_BORDER_PADDING = 10.0f;

        std::random_device r;
        std::default_random_engine positionRandom{ r() };
        std::uniform_real_distribution<float> positionDistribution(ITEMS_TERRAIN_BORDER_PADDING, TERRAIN_SIZE * TERRAIN_GRID_MAX_X - ITEMS_TERRAIN_BORDER_PADDING);

        std::default_random_engine scaleRandom{ r() };
        std::uniform_real_distribution<float> scaleDistribution(0.5f, 1.5f);

        const uint32_t STONES_COUNT = 12;
        for (uint32_t i = 0; i < STONES_COUNT; i++) {
            const auto x = positionDistribution(positionRandom);
            const auto z = positionDistribution(positionRandom);
            auto stone = std::make_shared<Stone>(glm::vec3(x, 0.0f, z), glm::quat(glm::vec3(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)))), glm::vec3(scaleDistribution(scaleRandom)));
            AddChild(stone);
        }

        auto fire = std::make_shared<Fire>(glm::vec3(30.0f, 0.0f, 100.0f));
        AddChild(fire);

        //auto mainPlane = std::make_shared<PlaneNode>(glm::vec3(-25.0f, 0.0f, -25.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(1.0f), AssetManager::Instance().GetAssetPath("Textures/rock.png"), AssetManager::Instance().GetAssetPath("Textures/rock_normal.png"), AssetManager::Instance().GetAssetPath("Textures/rock_height.png"), 0.03f);
        //AddChild(mainPlane);

        //auto mainPlane2 = std::make_shared<PlaneNode>(glm::vec3(-60.0f, 0.0f, -60.0f), glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f))), glm::vec3(1.0f), AssetManager::Instance().GetAssetPath("Textures/fungus.png"), AssetManager::Instance().GetAssetPath("Textures/fungus_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/fungus_height.png"), 0.000005f);
        //AddChild(mainPlane2);
        //
        //auto mainPlane3 = std::make_shared<PlaneNode>(glm::vec3(-100.0f, 0.0f, -60.0f), glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f))), glm::vec3(1.0f), AssetManager::Instance().GetAssetPath("Textures/sand_grass.png"), AssetManager::Instance().GetAssetPath("Textures/sand_grass_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/sand_grass_height.png"), 0.005f);
        //AddChild(mainPlane3);

        //auto mainPlane4 = std::make_shared<PlaneNode>(glm::vec3(-30.0f, 0.0f, -80.0f), glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f))), glm::vec3(1.0f), AssetManager::Instance().GetAssetPath("Textures/sand.png"), AssetManager::Instance().GetAssetPath("Textures/sand_normal_2.png"), AssetManager::Instance().GetAssetPath("Textures/sand_height.png"), 0.005f);
        //AddChild(mainPlane4);

        AbstractSceneNode::Init();

        m_masterRenderer->Init();
    }

    void Update(float deltaTime) override
    {
        for (auto child : m_children) {
            child->Update(deltaTime);
        }
    }

    void Render(RenderContext& renderContext) override
    {
        m_masterRenderer->BeforeRender(renderContext);

        m_masterRenderer->PreRender(renderContext);

        m_masterRenderer->Render(renderContext, GetThis());

        m_masterRenderer->PostRender(renderContext);

        m_masterRenderer->AfterRender(renderContext);
    }

    void ShutDown() override
    {
        m_masterRenderer->ShutDown();

        for (auto child : m_children) {
            child->ShutDown();
        }
    }

public:
    void operator()(const KeyEvent& keyEvent)
    {
        if (keyEvent.action == KeyActionType::PRESS) {
            if (keyEvent.keyCode == KeyCode::KEY_J) {
                RemoveNode();
            } else if (keyEvent.keyCode == KeyCode::KEY_K) {
                AddNode();
            }
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
        if (touchEvent.action == TouchActionType::DOWN) {
            //AddNode();
        } else if (touchEvent.action == TouchActionType::UP) {
            //RemoveNode();
        }
    }

private:
    void AddNode()
    {
        const float DISTANCE = 40.0f;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 4);
        const auto i = dis(gen);
        const auto j = dis(gen);
        const auto k = dis(gen);

        auto robot = std::make_shared<CubeRobot>(glm::vec3(i * DISTANCE, j * DISTANCE, k * DISTANCE), glm::quat(1, 0, 0, 0), glm::vec3(1, 1, 1), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        robot->Init();
        AddChild(robot);
    }

    void RemoveNode()
    {
        if (m_children.size() > 0) {
            auto& child = m_children.back();
            child->ShutDown();
            RemoveChild(child);
        }
    }

private:
    EventHandler<RootSceneNode, KeyEvent> m_keyEventHnadler{ *this };

    EventHandler<RootSceneNode, TouchEvent> m_touchEventHnadler{ *this };

private:
    std::unique_ptr<IRenderer<DefaultRenderContextUserData> > m_masterRenderer;
};

template <typename NodeFlagsType>
class TestApp final : public App<NodeFlagsType> {
public:
    TestApp(const std::shared_ptr<EngineConfig>& config)
        : App<NodeFlagsType>(config)
    {
    }

    ~TestApp() = default;

protected:
    void OnEngineInit() override
    {
    }

    void OnSceneInit() override
    {
        auto scene = this->m_engine->GetScene();

        auto rootNode = std::make_shared<RootSceneNode>(scene->GetRenderPass(), scene->GetSwapchain());

        scene->SetSceneRoot(rootNode);
    }

    void OnSceneGraphInit() override
    {
    }
};

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // avoid buffering

    auto config = std::make_shared<EngineConfig>();

    TestApp<SceneNodeFlags> app(config);
    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}
