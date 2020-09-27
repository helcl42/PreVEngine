#ifndef __GENERAL_H__
#define __GENERAL_H__

#include "render/VertexLayout.h"
#include "render/IMaterial.h"
#include "render/IModel.h"
#include "render/IAnimation.h"

#include <prev/common/Common.h>
#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/buffer/IndexBuffer.h>
#include <prev/core/memory/buffer/UniformBuffer.h>
#include <prev/core/memory/buffer/VertexBuffer.h>
#include <prev/core/memory/image/ColorImageBuffer.h>
#include <prev/core/memory/image/DepthImageBuffer.h>
#include <prev/core/memory/image/ImageBuffer.h>
#include <prev/core/memory/image/ImageStorageBuffer.h>
#include <prev/render/image/Image.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/scene/component/ComponentRepository.h>
#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/scene/graph/GraphTraversal.h>
#include <prev/scene/graph/SceneNode.h>
#include <prev/util/MathUtils.h>
#include <prev/util/VkUtils.h>

static const std::string TAG_LIGHT{ "Light" };
static const std::string TAG_MAIN_LIGHT{ "MainLight" };
static const std::string TAG_SHADOW{ "Shadow" };
static const std::string TAG_CAMERA{ "Camera" };
static const std::string TAG_MAIN_CAMERA{ "MainCamera" };
static const std::string TAG_PLAYER{ "Player" };

static const uint32_t MAX_LIGHT_COUNT{ 4 };
static const float AMBIENT_LIGHT_INTENSITY{ 0.2f };

static const glm::vec4 FOG_COLOR{ 0.47f, 0.53f, 0.58f, 1.0f };
static const float FOG_DENSITY{ 0.004f };
static const float FOG_GRADIENT{ 12.0f };

static const bool SHADOWS_ENABLED{ true };
static const glm::vec4 SELECTED_COLOR{ 1.0f, 0.0f, 0.0f, 1.0f };
static const uint32_t MAX_BONES_COUNT{ 100 };
static const glm::vec4 DEFAULT_CLIP_PLANE{ 0.0f, -1.0f, 0.0f, 1000.0f };

enum class SceneNodeFlags {
    TRANSFORM_COMPONENT,
    RAYCASTER_COMPONENT,
    RENDER_COMPONENT,
    RENDER_NORMAL_MAPPED_COMPONENT,
    RENDER_PARALLAX_MAPPED_COMPONENT,
    RENDER_CONE_STEP_MAPPED_COMPONENT,
    ANIMATION_RENDER_COMPONENT,
    ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT,
    ANIMATION_PARALLAX_MAPPED_RENDER_COMPONENT,
    ANIMATION_CONE_STEP_MAPPED_RENDER_COMPONENT,
    FONT_RENDER_COMPONENT,
    SKYBOX_RENDER_COMPONENT,
    TERRAIN_MANAGER_COMPONENT,
    TERRAIN_RENDER_COMPONENT,
    TERRAIN_NORMAL_MAPPED_RENDER_COMPONENT,
    TERRAIN_PARALLAX_MAPPED_RENDER_COMPONENT,
    TERRAIN_CONE_STEP_MAPPED_RENDER_COMPONENT,
    CAMERA_COMPONENT,
    SHADOWS_COMPONENT,
    LIGHT_COMPONENT,
    WATER_REFLECTION_RENDER_COMPONENT,
    WATER_REFRACTION_RENDER_COMPONENT,
    WATER_RENDER_COMPONENT,
    LENS_FLARE_RENDER_COMPONENT,
    SUN_RENDER_COMPONENT,
    BOUNDING_VOLUME_COMPONENT,
    SELECTABLE_COMPONENT,
    PARTICLE_SYSTEM_COMPONENT,
    CLOUDS_COMPONENT,
    SKY_RENDER_COMPONENT,
    TIME_COMPONENT,
    _
};

class AssetManager : public prev::common::pattern::Singleton<AssetManager> {
public:
    ~AssetManager() = default;

public:
    std::string GetAssetPath(const std::string& path)
    {
#if defined(__ANDROID__)
        return path;
#else
        return "./assets/" + path;
#endif
    }

private:
    friend class prev::common::pattern::Singleton<AssetManager>;

private:
    AssetManager() = default;
};

class ITransformComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual std::shared_ptr<ITransformComponent> GetParent() const = 0;

    virtual void SetParent(const std::shared_ptr<ITransformComponent>& parent) = 0;

    virtual void Rotate(const glm::quat& rotationDiff) = 0;

    virtual void Translate(const glm::vec3& positionDiff) = 0;

    virtual void Scale(const glm::vec3& scaleDiff) = 0;

    virtual glm::quat GetOrientation() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual glm::vec3 GetScale() const = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual void SetScale(const glm::vec3& scale) = 0;

    virtual glm::mat4 GetTransform() const = 0;

    virtual glm::mat4 GetTransformScaled() const = 0;

    virtual glm::mat4 GetWorldTransform() const = 0;

    virtual glm::mat4 GetWorldTransformScaled() const = 0;

    virtual glm::vec3 GetScaler() const = 0;

    virtual bool IsRoot() const = 0;

public:
    virtual ~ITransformComponent() = default;
};

class TransformComponent : public ITransformComponent {
public:
    TransformComponent()
        : m_worldTransform(1.0f)
        , m_position(glm::vec3(0.0f))
        , m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        , m_scaler(glm::vec3(1.0f))
    {
    }

    TransformComponent(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : m_worldTransform(1.0f)
        , m_position(position)
        , m_orientation(orientation)
        , m_scaler(scale)
    {
    }

    ~TransformComponent() = default;

public:
    void Update(float deltaTime) override
    {
        if (auto parent = m_parent.lock()) { //This node has a parent...
            m_worldTransform = parent->GetWorldTransform() * GetTransform();
        } else { //Root node, world transform is local transform!
            m_worldTransform = GetTransform();
        }
    }

    void SetParent(const std::shared_ptr<ITransformComponent>& parent) override
    {
        m_parent = parent;
    }

    std::shared_ptr<ITransformComponent> GetParent() const override
    {
        return m_parent.lock();
    }

    void Rotate(const glm::quat& rotationDiff) override
    {
        m_orientation = glm::normalize(m_orientation * rotationDiff);
    }

    void Translate(const glm::vec3& positionDiff) override
    {
        m_position += positionDiff;
    }

    void Scale(const glm::vec3& scaleDiff) override
    {
        m_scaler += scaleDiff;
    }

    glm::quat GetOrientation() const override
    {
        return m_orientation;
    }

    glm::vec3 GetPosition() const override
    {
        return m_position;
    }

    glm::vec3 GetScale() const override
    {
        return m_scaler;
    }

    void SetOrientation(const glm::quat& orientation) override
    {
        m_orientation = orientation;
    }

    void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
    }

    void SetScale(const glm::vec3& scale) override
    {
        m_scaler = scale;
    }

    glm::mat4 GetTransform() const override
    {
        return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, glm::vec3(1.0f));
    }

    glm::mat4 GetTransformScaled() const override
    {
        return prev::util::MathUtil::CreateTransformationMatrix(m_position, m_orientation, m_scaler);
    }

    glm::mat4 GetWorldTransform() const override
    {
        return m_worldTransform;
    }

    glm::mat4 GetWorldTransformScaled() const override
    {
        return glm::scale(GetWorldTransform(), m_scaler);
    }

    glm::vec3 GetScaler() const override
    {
        return m_scaler;
    }

    bool IsRoot() const override
    {
        return !m_parent.lock();
    }

private:
    std::weak_ptr<ITransformComponent> m_parent;

    glm::mat4 m_worldTransform;

    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_scaler;
};

class TrasnformComponentFactory {
public:
    std::unique_ptr<ITransformComponent> Create() const
    {
        return std::make_unique<TransformComponent>();
    }

    std::unique_ptr<ITransformComponent> Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) const
    {
        return std::make_unique<TransformComponent>(position, orientation, scale);
    }
};

class IRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;

    virtual std::shared_ptr<prev_test::render::IMaterial> GetMaterial(const uint32_t = 0) const = 0;

    virtual const std::vector<std::shared_ptr<prev_test::render::IMaterial> >& GetMaterials() const = 0;

    virtual bool CastsShadows() const = 0;

    virtual bool IsCastedByShadows() const = 0;

public:
    virtual ~IRenderComponent() = default;
};

class IAnimationRenderComponent : public IRenderComponent {
public:
    virtual std::shared_ptr<prev_test::render::IAnimation> GetAnimation() const = 0;

public:
    virtual ~IAnimationRenderComponent() = default;
};

class ITimeComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual float GetCurrentDeltaTime() const = 0;

    virtual float GetElapsedTime() const = 0;

public:
    virtual ~ITimeComponent() = default;
};

class TimeComponent final : public ITimeComponent {
public:
    TimeComponent() = default;

    ~TimeComponent() = default;

public:
    void Update(float deltaTime) override
    {
        m_currentDelta = deltaTime;
        m_elapsedTime += deltaTime;
    }

    float GetCurrentDeltaTime() const override
    {
        return m_currentDelta;
    }

    float GetElapsedTime() const override
    {
        return m_elapsedTime;
    }

private:
    float m_currentDelta{ 0.0f };

    float m_elapsedTime{ 0.0f };
};

#endif