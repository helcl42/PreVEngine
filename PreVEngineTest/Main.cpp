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

#include "Font.h"
#include "General.h"
#include "Mesh.h"
#include "Pipeline.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "Water.h"
#include "Shadows.h"
#include "Light.h"
#include "Camera.h"
#include "LensFlare.h"

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
        // image
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

    std::unique_ptr<ImageBuffer> CreateImageBuffer(Allocator& allocator, const std::shared_ptr<Image>& image, const bool repeatAddressMode) const
    {
        const VkExtent2D imageExtent = { image->GetWidth(), image->GetHeight() };

        auto imageBuffer = std::make_unique<ImageBuffer>(allocator);
        imageBuffer->Create(ImageBufferCreateInfo{ imageExtent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 0, true, VK_IMAGE_VIEW_TYPE_2D, 1, repeatAddressMode ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, (uint8_t*)image->GetBuffer() });

        return imageBuffer;
    }

    std::unique_ptr<IMaterial> CreateMaterial(Allocator& allocator, const std::string& textureFilename, const bool repeatAddressMode, const float shineDamper, const float reflectivity) const
    {
        auto image = CreateImage(textureFilename);

        auto imageBuffer = CreateImageBuffer(allocator, image, repeatAddressMode);

        return std::make_unique<Material>(image, std::move(imageBuffer), shineDamper, reflectivity);
    }

    std::unique_ptr<IModel> CreateModel(Allocator& allocator, const std::shared_ptr<IMesh>& mesh) const
    {
        auto vertexBuffer = std::make_unique<VBO>(allocator);
        vertexBuffer->Data(mesh->GetVertices(), mesh->GerVerticesCount(), mesh->GetVertexLayout().GetStride());

        auto indexBuffer = std::make_unique<IBO>(allocator);
        indexBuffer->Data(mesh->GerIndices().data(), (uint32_t)mesh->GerIndices().size());

        return std::make_unique<Model>(mesh, std::move(vertexBuffer), std::move(indexBuffer));
    }

public:
    std::unique_ptr<IRenderComponent> CreateCubeRenderComponent(Allocator& allocator, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, false, 10.0f, 1.0f);

        auto mesh = std::make_unique<CubeMesh>();
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreatePlaneRenderComponent(Allocator& allocator, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 2.0f, 0.3f);

        auto mesh = std::make_shared<PlaneMesh>(40.0f, 40.0f, 1, 1, 10.0f, 10.0f);
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IRenderComponent> CreateModelRenderComponent(Allocator& allocator, const std::string& modelPath, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 2.0f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath);
        auto model = CreateModel(allocator, std::move(mesh));

        return std::make_unique<DefaultRenderComponent>(std::move(model), std::move(material), castsShadows, isCastedByShadows);
    }

    std::unique_ptr<IAnimationRenderComponent> CreateAnimatedModelRenderComponent(Allocator& allocator, const std::string& modelPath, const std::string& textureFilename, const bool castsShadows, const bool isCastedByShadows) const
    {
        auto material = CreateMaterial(allocator, textureFilename, true, 1.5f, 0.3f);

        MeshFactory meshFactory{};
        auto mesh = meshFactory.CreateMesh(modelPath, FlagSet<MeshFactory::AssimpMeshFactoryCreateFlags>{ MeshFactory::AssimpMeshFactoryCreateFlags::ANIMATION });
        auto model = CreateModel(allocator, std::move(mesh));

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
protected:
    const std::string m_texturePath;

public:
    AbstractCubeRobotSceneNode(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT }, position, orientation, scale)
        , m_texturePath(texturePath)
    {
    }

    virtual ~AbstractCubeRobotSceneNode() = default;

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        auto cubeComponent = renderComponentFactory.CreateCubeRenderComponent(*allocator, m_texturePath, true, true);

        ComponentRepository<IRenderComponent>::Instance().Add(m_id, std::move(cubeComponent));

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IRenderComponent>::Instance().Remove(m_id);
    }
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
        m_body->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.x), glm::vec3(1.0f, 0.0f, 0.0f)));
        m_body->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.y), glm::vec3(0.0f, 1.0f, 0.0f)));

        m_head->Rotate(glm::rotate(glm::mat4(1.0f), -glm::radians(25.0f) * deltaTime, glm::vec3(0, 1, 0)));

        m_leftArm->Translate(glm::vec3(0, -4.5, 0));
        m_leftArm->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f) * deltaTime, glm::vec3(1, 0, 0)));
        m_leftArm->Translate(glm::vec3(0, 4.5, 0));

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

class Plane : public AbstractSceneNode<SceneNodeFlags> {
protected:
    const std::string m_texturePath;

public:
    Plane(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_RENDER_COMPONENT }, position, orientation, scale)
        , m_texturePath(texturePath)
    {
    }

    virtual ~Plane() = default;

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        auto renderComponent = renderComponentFactory.CreatePlaneRenderComponent(*allocator, m_texturePath, false, true);

        ComponentRepository<IRenderComponent>::Instance().Add(m_id, std::move(renderComponent));

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IRenderComponent>::Instance().Remove(m_id);
    }
};

class Terrain : public AbstractSceneNode<SceneNodeFlags> {
private:
    const int m_xIndex;

    const int m_zIndex;

    std::shared_ptr<ITerrainComponenet> m_terrainComponent;

    std::weak_ptr<ITerrainManagerComponent> m_terrainManagerComponent;

public:
    Terrain(const int x, const int z)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_RENDER_COMPONENT }, glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
        , m_xIndex(x)
        , m_zIndex(z)
    {
    }

    virtual ~Terrain() = default;

public:
    void Init() override
    {
        TerrainComponentFactory terrainComponentFactory{};
        m_terrainComponent = std::move(terrainComponentFactory.CreateRandomTerrain(m_xIndex, m_zIndex, TERRAIN_SIZE));
        SetPosition(m_terrainComponent->GetPosition());

        ComponentRepository<ITerrainComponenet>::Instance().Add(m_id, m_terrainComponent);

        m_terrainManagerComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_COMPONENT });
        if (auto manager = m_terrainManagerComponent.lock()) {
            manager->AddTerrainComponent(m_terrainComponent);
        }

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        if (auto manager = m_terrainManagerComponent.lock()) {
            manager->RemoveTerrain(m_terrainComponent);
        }

        ComponentRepository<ITerrainComponenet>::Instance().Remove(m_id);
    }
};

class TerrainManager : public AbstractSceneNode<SceneNodeFlags> {
private:
    const int m_gridMaxX;

    const int m_gridMaxZ;

public:
    TerrainManager(const int maxX, const int maxZ)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_COMPONENT }, glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
        , m_gridMaxX(maxX)
        , m_gridMaxZ(maxZ)
    {
    }

    virtual ~TerrainManager() = default;

public:
    void Init() override
    {
        auto terrainManager = TerrainManagerComponentFactory{}.Create();
        ComponentRepository<ITerrainManagerComponent>::Instance().Add(m_id, std::move(terrainManager));

        for (int x = 0; x < m_gridMaxX; x++) {
            for (int z = 0; z < m_gridMaxZ; z++) {
                auto terrain = std::make_shared<Terrain>(x, z);
                AddChild(terrain);
            }
        }

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ITerrainManagerComponent>::Instance().Remove(m_id);
    }
};

class Goblin : public AbstractSceneNode<SceneNodeFlags> {
private:
    const float RUN_SPEED{ 14.0f };

    const float YAW_TURN_SPEED{ 3.0f };

    const float PITCH_TURN_SPEED{ 0.5f };

    const float GRAVITY_Y{ -5.0f };

    const float JUMP_POWER{ 2.5f };

    const float MIN_Y_POS{ 9.0f };

private:
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

    float m_distanceFromPerson{ 45.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    EventHandler<Goblin, KeyEvent> m_keyboardEventsHandler{ *this };

    EventHandler<Goblin, MouseEvent> m_mouseEventsHandler{ *this };

    EventHandler<Goblin, TouchEvent> m_touchEventsHandler{ *this };

    EventHandler<Goblin, MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    std::shared_ptr<IAnimationRenderComponent> m_animatonRenderComponent;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

public:
    Goblin(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_ANIMATION_RENDER_COMPONENT }, position, orientation, scale)
    {
    }

    virtual ~Goblin() = default;

public:
    void Init() override
    {
        auto allocator = AllocatorProvider::Instance().GetAllocator();

        RenderComponentFactory renderComponentFactory{};
        m_animatonRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(*allocator, AssetManager::Instance().GetAssetPath("Models/Goblin/goblin.dae"), AssetManager::Instance().GetAssetPath("Models/Goblin/goblin_texture.png"), true, true);
        ComponentRepository<IAnimationRenderComponent>::Instance().Add(m_id, m_animatonRenderComponent);

        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
        ComponentRepository<ICameraComponent>::Instance().Add(m_id, m_cameraComponent);

        m_animatonRenderComponent->GetAnimation()->SetIndex(0);
        m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
        m_animatonRenderComponent->GetAnimation()->SetSpeed(1.0f);

        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), m_cameraComponent->GetUpDirection())));
        m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), m_cameraComponent->GetRightDirection())));

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto terrain = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ITerrainManagerComponent>(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_TERRAIN_COMPONENT });

        if ((m_shouldGoForward || m_shouldGoBackward || m_shouldGoLeft || m_shouldGoRight) && !m_isInTheAir) {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::RUNNING);
            m_animatonRenderComponent->GetAnimation()->Update(m_shouldGoBackward ? -deltaTime : deltaTime);

            glm::vec3 positionOffset{ 0.0f };
            if (m_shouldGoForward) {
                positionOffset -= deltaTime * MathUtil::GetUpVector(GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoBackward) {
                positionOffset += deltaTime * MathUtil::GetUpVector(GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoLeft) {
                positionOffset += deltaTime * MathUtil::GetRightVector(GetOrientation()) * RUN_SPEED;
            }
            if (m_shouldGoRight) {
                positionOffset -= deltaTime * MathUtil::GetRightVector(GetOrientation()) * RUN_SPEED;
            }
            Translate(positionOffset);
        } else {
            m_animatonRenderComponent->GetAnimation()->SetState(AnimationState::PAUSED);
            m_animatonRenderComponent->GetAnimation()->Update(deltaTime);
        }

        auto currentPosition = GetPosition();
        const float height = terrain->GetHeightAt(currentPosition);
        const auto currentY = height + MIN_Y_POS;

        if (m_isInTheAir) {
            m_upwardSpeed += GRAVITY_Y * deltaTime;
            Translate(glm::vec3(0.0f, m_upwardSpeed, 0.0f));
            if (currentPosition.y < currentY) {
                SetPosition(glm::vec3(currentPosition.x, currentY, currentPosition.z));
                m_upwardSpeed = 0.0f;
                m_isInTheAir = false;
            }
        } else {
            SetPosition(glm::vec3(currentPosition.x, currentY, currentPosition.z));
        }

        if (m_shouldRotate) {
            const float yawAmount = YAW_TURN_SPEED * m_rotationAroundY * deltaTime;
            const float pitchAmount = PITCH_TURN_SPEED * m_pitchDiff * deltaTime;

            Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(yawAmount), glm::vec3(0.0f, 0.0f, 1.0f))));

            m_cameraComponent->AddYaw(yawAmount);
            m_cameraComponent->AddPitch(pitchAmount);

            m_rotationAroundY = 0.0f;
            m_pitchDiff = 0.0f;
        }

        const glm::vec3 cameraPosition = GetPosition() + (-m_cameraComponent->GetForwardDirection() * m_distanceFromPerson) + glm::vec3(0.0f, 5.0f, 0.0f);
        m_cameraComponent->SetPosition(cameraPosition);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IAnimationRenderComponent>::Instance().Remove(m_id);
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

            Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees.x), glm::vec3(0.0f, 0.0f, 1.0f))));

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
};

class Camera : public AbstractSceneNode<SceneNodeFlags> {
private:
    EventHandler<Camera, MouseEvent> m_mouseHandler{ *this };

    EventHandler<Camera, TouchEvent> m_touchHandler{ *this };

    EventHandler<Camera, KeyEvent> m_keyHandler{ *this };

private:
    const float m_sensitivity = 0.05f;

    const float m_moveSpeed = 25.0f;

    const float m_absMinMaxPitch{ 89.0f };

    float m_pitchAngle = 0.0f;

private:
    InputsFacade m_inputFacade;

    std::shared_ptr<ICameraComponent> m_cameraComponent;

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

#if defined(__ANDROID__)
    bool m_autoMoveForward = false;

    bool m_autoMoveBackward = false;
#endif
public:
    Camera()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_CAMERA_COMPONENT })
    {
        m_inputFacade.SetMouseLocked(true);
        m_inputFacade.SetMouseCursorVisible(false);
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
        CameraComponentFactory cameraFactory{};
        m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));

        ComponentRepository<ICameraComponent>::Instance().Add(m_id, m_cameraComponent);

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

        SetPosition(MathUtil::ExtractTranslation(cameraTransformInWorldSpace));
        SetOrientation(MathUtil::ExtractOrientation(cameraTransformInWorldSpace));

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ICameraComponent>::Instance().Remove(m_id);
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
            } else if (keyEvent.keyCode == KeyCode::KEY_L) {
                m_inputFacade.SetMouseLocked(!m_inputFacade.IsMouseLocked());
                m_inputFacade.SetMouseCursorVisible(!m_inputFacade.IsMouseCursorVisible());
            }
        }
    }
};

class Text : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<IFontRenderComponent> m_fontComponent;

    FPSService m_fpsService{ 1.0f, false };

public:
    Text()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_FONT_RENDER_COMPONENT })
    {
    }

    ~Text() = default;

public:
    void Init() override
    {
        FontRenderComponentsFactory factory{};
        m_fontComponent = factory.Create(AssetManager::Instance().GetAssetPath("Fonts/verdana.fnt"), AssetManager::Instance().GetAssetPath("Fonts/verdana.png"));

        ComponentRepository<IFontRenderComponent>::Instance().Add(m_id, m_fontComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        if (m_fpsService.Update(deltaTime)) {
            m_fontComponent->Reset();

            std::stringstream fpsString;
            fpsString << std::setprecision(1) << std::fixed;
            fpsString << m_fpsService.GetAverageFPS() << " FPS";

            auto fancyText = std::make_shared<FancyText>(fpsString.str(), 1.6f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0), glm::vec2(0.4f, -0.4f), 1.0f, true, 0.5f, 0.1f);
            m_fontComponent->AddText(fancyText);
        }

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IFontRenderComponent>::Instance().Remove(m_id);
    }
};

class MainLight : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;

public:
    MainLight(const glm::vec3& pos)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LIGHT_COMPONENT })
        , m_initialPosition(pos)
    {
    }

    ~MainLight() = default;

public:
    void Init() override
    {
        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition);

        ComponentRepository<ILightComponent>::Instance().Add(m_id, m_lightComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const float ROTATION_SPEED_DEG_PER_SEC = 7.5f;
        const float ROTATION_ANGLE = ROTATION_SPEED_DEG_PER_SEC * deltaTime;

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::translate(transform, m_lightComponent->GetPosition());

        glm::vec3 position{ transform[3][0], transform[3][1], transform[3][2] };

        m_lightComponent->SetPosition(position);

        auto lightTransformInWorldSpace = glm::inverse(m_lightComponent->LookAt());

        SetPosition(MathUtil::ExtractTranslation(lightTransformInWorldSpace));
        SetOrientation(MathUtil::ExtractOrientation(lightTransformInWorldSpace));

        SetPosition(position);

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ILightComponent>::Instance().Remove(m_id);
    }
};

class Light : public AbstractSceneNode<SceneNodeFlags> {
private:
    std::shared_ptr<ILightComponent> m_lightComponent;

    glm::vec3 m_initialPosition;

    glm::vec3 m_color;

public:
    Light(const glm::vec3& position, const glm::vec3& color)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LIGHT_COMPONENT })
        , m_initialPosition(position)
        , m_color(color)
    {
    }

    ~Light() = default;

public:
    void Init() override
    {
        LightComponentFactory lightFactory{};
        m_lightComponent = lightFactory.CreateLightCompoennt(m_initialPosition, m_color, glm::vec3(0.1f, 0.005f, 0.001f));

        ComponentRepository<ILightComponent>::Instance().Add(m_id, m_lightComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        SetPosition(m_lightComponent->GetPosition());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ILightComponent>::Instance().Remove(m_id);
    }
};

class SkyBox : public AbstractSceneNode<SceneNodeFlags> {
public:
    SkyBox()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SKYBOX_RENDER_COMPONENT })
    {
    }

    ~SkyBox() = default;

public:
    void Init() override
    {
        SkyBoxComponentFactory factory{};
        m_skyBoxComponent = factory.Create();

        ComponentRepository<ISkyBoxComponent>::Instance().Add(m_id, m_skyBoxComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>(TagSet{ TAG_MAIN_CAMERA });

        const float ROTATION_ANGLE = ROTATION_SPEED_DEGS_PER_SEC * deltaTime;

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));

        Rotate(glm::quat_cast(transform));
        SetPosition(cameraComponent->GetPosition());
        SetScale(glm::vec3(750.0f));

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ISkyBoxComponent>::Instance().Remove(m_id);
    }

private:
    std::shared_ptr<ISkyBoxComponent> m_skyBoxComponent;

    static const inline float ROTATION_SPEED_DEGS_PER_SEC = 0.5f;
};

class WaterReflection : public AbstractSceneNode<SceneNodeFlags> {
public:
    WaterReflection()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFLECTION_RENDER_COMPONENT })
    {
    }

    ~WaterReflection() = default;

public:
    void Init() override
    {
        WaterComponentFactory componentFactory{};
        m_reflectionComponent = std::move(componentFactory.CreateOffScreenComponent(REFLECTION_MEASURES.x, REFLECTION_MEASURES.y));
        m_reflectionComponent->Init();

        ComponentRepository<IWaterOffscreenRenderPassComponent>::Instance().Add(m_id, m_reflectionComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IWaterOffscreenRenderPassComponent>::Instance().Remove(m_id);

        m_reflectionComponent->ShutDown();
    }

private:
    std::shared_ptr<IWaterOffscreenRenderPassComponent> m_reflectionComponent;
};

class WaterRefraction : public AbstractSceneNode<SceneNodeFlags> {
public:
    WaterRefraction()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_REFRACTION_RENDER_COMPONENT })
    {
    }

    ~WaterRefraction() = default;

public:
    void Init() override
    {
        WaterComponentFactory componentFactory{};
        m_refractionComponent = std::move(componentFactory.CreateOffScreenComponent(REFRACTION_MEASURES.x, REFRACTION_MEASURES.y));
        m_refractionComponent->Init();

        ComponentRepository<IWaterOffscreenRenderPassComponent>::Instance().Add(m_id, m_refractionComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IWaterOffscreenRenderPassComponent>::Instance().Remove(m_id);

        m_refractionComponent->ShutDown();
    }

private:
    std::shared_ptr<IWaterOffscreenRenderPassComponent> m_refractionComponent;
};

class Water : public AbstractSceneNode<SceneNodeFlags> {
public:
    Water(const int x, const int z)
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_WATER_RENDER_COMPONENT })
        , m_x(x)
        , m_z(z)
    {
    }

    ~Water() = default;

public:
    void Init() override
    {
        WaterComponentFactory componentFactory{};
        m_waterComponent = std::move(componentFactory.Create(m_x, m_z));

        ComponentRepository<IWaterComponent>::Instance().Add(m_id, m_waterComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        m_waterComponent->Update(deltaTime);

        SetPosition(m_waterComponent->GetPosition());
        SetScale(glm::vec3(WATER_TILE_SIZE));

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IWaterComponent>::Instance().Remove(m_id);
    }

private:
    const int m_x;

    const int m_z;

    std::shared_ptr<IWaterComponent> m_waterComponent;
};

class WaterManager : public AbstractSceneNode<SceneNodeFlags> {
private:
    const int m_gridMaxX;

    const int m_gridMaxZ;

public:
    WaterManager(const int maxX, const int maxZ)
        : AbstractSceneNode(glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
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
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_LENS_FLARE_RENDER_COMPONENT })
    {
    }

    ~LensFlare() = default;

public:
    void Init() override
    {
        LensFlareComponentFactory componentFactory{};
        m_lensFlareComponent = std::move(componentFactory.Create());

        ComponentRepository<ILensFlareComponent>::Instance().Add(m_id, m_lensFlareComponent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        // TODO fix aspect ratio -> is should not be hardcoded
        m_lensFlareComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(1920.0f / 1080.0f), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<ILensFlareComponent>::Instance().Remove(m_id);
    }

private:
    std::shared_ptr<ILensFlareComponent> m_lensFlareComponent;
};

class Shadows : public AbstractSceneNode<SceneNodeFlags> {
public:
    Shadows()
        : AbstractSceneNode(FlagSet<SceneNodeFlags>{ SceneNodeFlags::HAS_SHADOWS_COMPONENT })
    {
    }

    virtual ~Shadows() = default;

public:
    void Init() override
    {
        ShadowsComponentFactory shadowsFactory{};
        m_shadowsCompoent = shadowsFactory.Create();
        m_shadowsCompoent->Init();

        ComponentRepository<IShadowsComponent>::Instance().Add(m_id, m_shadowsCompoent);

        AbstractSceneNode::Init();
    }

    void Update(float deltaTime) override
    {
        const auto lightComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ILightComponent>({ TAG_MAIN_LIGHT });
        const auto cameraComponent = GraphTraversalHelper::GetNodeComponent<SceneNodeFlags, ICameraComponent>({ TAG_MAIN_CAMERA });

        m_shadowsCompoent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum().GetNearClippingPlane(), lightComponent->GetViewFrustum().GetFarClippingPlane(), lightComponent->GetViewFrustum().CreateProjectionMatrix(1.0f), cameraComponent->LookAt());

        AbstractSceneNode::Update(deltaTime);
    }

    void ShutDown() override
    {
        AbstractSceneNode::ShutDown();

        ComponentRepository<IShadowsComponent>::Instance().Remove(m_id);

        m_shadowsCompoent->ShutDown();
    }

private:
    std::shared_ptr<IShadowsComponent> m_shadowsCompoent;
};

class RootSceneNode : public AbstractSceneNode<SceneNodeFlags> {
private:
    EventHandler<RootSceneNode, KeyEvent> m_keyEventHnadler{ *this };

    EventHandler<RootSceneNode, TouchEvent> m_touchEventHnadler{ *this };

private:
    std::unique_ptr<IRenderer<DefaultRenderContextUserData>> m_masterRenderer;

public:
    RootSceneNode(const std::shared_ptr<RenderPass>& renderPass)
        : AbstractSceneNode()
        , m_masterRenderer(std::make_unique<MasterRenderer>(renderPass))
    {
    }

    virtual ~RootSceneNode() = default;

public:
    void Init() override
    {
        // Init scene nodes
        auto skyBox = std::make_shared<SkyBox>();
        AddChild(skyBox);

        auto sunLight = std::make_shared<MainLight>(glm::vec3(150.0f, 50.0f, 150.0f));
        sunLight->SetTags({ TAG_MAIN_LIGHT, TAG_LIGHT });
        AddChild(sunLight);

        auto light1 = std::make_shared<Light>(glm::vec3(30.0f, 20.0f, 35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        light1->SetTags({ TAG_LIGHT });
        AddChild(light1);

        auto light2 = std::make_shared<Light>(glm::vec3(-30.0f, 20.0f, 35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        light2->SetTags({ TAG_LIGHT });
        AddChild(light2);

        auto light3 = std::make_shared<Light>(glm::vec3(0.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        light3->SetTags({ TAG_LIGHT });
        AddChild(light3);

        auto shadows = std::make_shared<Shadows>();
        shadows->SetTags({ TAG_SHADOW });
        AddChild(shadows);

        auto freeCamera = std::make_shared<Camera>();
        //freeCamera->SetTags({ TAG_MAIN_CAMERA });
        AddChild(freeCamera);

        auto camRobot = std::make_shared<CubeRobot>(glm::vec3(1.0f, -0.4f, -1.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
        freeCamera->AddChild(camRobot);

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

        auto goblin = std::make_shared<Goblin>(glm::vec3(-25.0f, 9.0f, 0.0f), glm::quat(glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f))), glm::vec3(0.005f));
        goblin->SetTags({ TAG_MAIN_CAMERA });
        AddChild(goblin);

        auto text = std::make_shared<Text>();
        AddChild(text);

        auto terrainManager = std::make_shared<TerrainManager>(1, 1);
        AddChild(terrainManager);

        auto water = std::make_shared<WaterManager>(1, 1);
        AddChild(water);

        auto lensFlare = std::make_shared<LensFlare>();
        AddChild(lensFlare);

        for (auto child : m_children) {
            child->Init();
        }

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
        m_masterRenderer->Render(renderContext, GetThis());
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
                RemmoveNode();
            } else if (keyEvent.keyCode == KeyCode::KEY_K) {
                AddNode();
            }
        }
    }

    void operator()(const TouchEvent& touchEvent)
    {
        if (touchEvent.action == TouchActionType::DOWN) {
            AddNode();
        } else if (touchEvent.action == TouchActionType::UP) {
            //RemmoveNode();
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

    void RemmoveNode()
    {
        if (m_children.size() > 0) {
            auto& child = m_children.back();
            child->ShutDown();
            RemoveChild(child);
        }
    }
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

        auto rootNode = std::make_shared<RootSceneNode>(scene->GetRenderPass());

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
