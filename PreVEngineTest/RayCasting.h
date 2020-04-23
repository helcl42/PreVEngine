#ifndef __RAY_CASTING_H__
#define __RAY_CASTING_H__

#include "General.h"

#define RENDER_RAYCASTS
#define RENDER_SELECTION

struct RayEvent {
    Ray ray;

    RayEvent() = default;

    RayEvent(const Ray& r)
        : ray(r)
    {
    }

    virtual ~RayEvent() = default;
};

class RayModelFactory {
public:
    std::unique_ptr<IModel> Create(const Ray& ray) const
    {
        const float DISTANCE_BETWEEN_POINTS = 4.0f;
        const uint32_t pointsCount = static_cast<uint32_t>(ray.length / DISTANCE_BETWEEN_POINTS);

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        for (uint32_t i = 1; i < pointsCount; i++) {
            vertices.emplace_back(ray.origin + ray.direction * (static_cast<float>(i) * DISTANCE_BETWEEN_POINTS));
            indices.emplace_back(i);
        }

        auto allocator = AllocatorProvider::Instance().GetAllocator();
        auto vertexBuffer = std::make_unique<VBO>(*allocator);
        vertexBuffer->Data(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(glm::vec3));
        auto indexBuffer = std::make_unique<IBO>(*allocator);
        indexBuffer->Data(indices.data(), static_cast<uint32_t>(indices.size()));
        return std::make_unique<Model>(nullptr, std::move(vertexBuffer), std::move(indexBuffer));
    }
};

class IRayCasterComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual void SetRayDirection(const glm::vec3& rayDirection) = 0;

    virtual void SetRayLength(const float len) = 0;

    virtual void SetRayStartPosition(const glm::vec3& position) = 0;

    virtual void SetOrientationOffsetAngles(const glm::vec2& angles) = 0;

    virtual Ray GetRay() const = 0;

#ifdef RENDER_RAYCASTS
    virtual std::shared_ptr<IModel> GetModel() const = 0;
#endif
public:
    virtual ~IRayCasterComponent() {}
};

class RayCasterComponent final : public IRayCasterComponent {
public:
    RayCasterComponent() = default;

    ~RayCasterComponent() = default;

public:
    void Update(float deltaTime) override
    {
        m_rayDirection = glm::normalize(m_rayDirection);

        if (m_orientationOffsetAngles.x != 0.0f) {
            const auto rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_rayDirection);
            const auto compensationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orientationOffsetAngles.x), rightVec);
            m_rayDirection = glm::normalize(compensationTransform * glm::vec4(m_rayDirection, 0.0f));
        }

        if (m_orientationOffsetAngles.y != 0.0f) {
            const auto compensationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orientationOffsetAngles.y), glm::vec3(0, 1, 0));
            m_rayDirection = glm::normalize(compensationTransform * glm::vec4(m_rayDirection, 0.0f));
        }

        Ray ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
        EventChannel::Broadcast(RayEvent{ ray });

#ifdef RENDER_RAYCASTS
        RayModelFactory modelFactory{};
        m_model = modelFactory.Create(ray);
#endif
    }

    void SetRayDirection(const glm::vec3& rayDirection) override
    {
        m_rayDirection = rayDirection;
    }

    void SetRayLength(const float len) override
    {
        m_rayLength = len;
    }

    void SetRayStartPosition(const glm::vec3& position) override
    {
        m_rayStartPosition = position;
    }

    void SetOrientationOffsetAngles(const glm::vec2& angles) override
    {
        m_orientationOffsetAngles = angles;
    }
    
    Ray GetRay() const override 
    {
        return Ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
    }
#ifdef RENDER_RAYCASTS
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }
#endif
private:
    glm::vec3 m_rayDirection;

    glm::vec3 m_rayStartPosition;

    float m_rayLength{ 500.0f };

    glm::vec2 m_orientationOffsetAngles{ 0.0f, 0.0f };
#ifdef RENDER_RAYCASTS
    std::shared_ptr<IModel> m_model;
#endif
};

class IMouseRayCasterComponent : public IRayCasterComponent {
public:
    virtual void SetViewPortDimensions(const glm::vec2& viewPortDimensions) = 0;

    virtual void SetViewMatrix(const glm::mat4& viewMatrix) = 0;

    virtual void SetProjectionMatrix(const glm::mat4& projectionMatrix) = 0;

public:
    virtual ~IMouseRayCasterComponent() = default;
};

class MouseRayCasterComponent : public IMouseRayCasterComponent {
public:
    MouseRayCasterComponent() = default;

    virtual ~MouseRayCasterComponent() = default;

public:    
    void Update(float deltaTime) override
    {
        const glm::vec2 normalizeDeviceCoords = MathUtil::FromViewPortSpaceToNormalizedDeviceSpace(m_viewPortDimensions, glm::vec2(m_currentMousePosition.x, m_currentMousePosition.y));
        const glm::vec4 clipSpaceCoords = MathUtil::FromNormalizedDeviceSpaceToClipSpace(normalizeDeviceCoords);
        const glm::vec4 eyeSpaceCoords = MathUtil::FromClipSpaceToCameraSpace(m_projectionMatrix, clipSpaceCoords);
        const glm::vec3 worldCoords = MathUtil::FromCameraSpaceToWorldSpace(m_viewMatrix, eyeSpaceCoords);

        m_rayDirection = glm::normalize(worldCoords);

        if (m_orientationOffsetAngles.x != 0.0f) {
            const auto rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_rayDirection);
            const auto compensationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orientationOffsetAngles.x), rightVec);
            m_rayDirection = glm::normalize(compensationTransform * glm::vec4(m_rayDirection, 0.0f));
        }

        if (m_orientationOffsetAngles.y != 0.0f) {
            const auto compensationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_orientationOffsetAngles.y), glm::vec3(0, 1, 0));
            m_rayDirection = glm::normalize(compensationTransform * glm::vec4(m_rayDirection, 0.0f));
        }

        Ray ray{ m_rayStartPosition, m_rayDirection, m_rayLength }; 
        EventChannel::Broadcast(RayEvent{ ray });

#ifdef RENDER_RAYCASTS
        RayModelFactory modelFactory{};
        m_model = modelFactory.Create(ray);
#endif
    }

    void SetViewPortDimensions(const glm::vec2& viewPortDimensions) override
    {
        m_viewPortDimensions = viewPortDimensions;
    }

    void SetViewMatrix(const glm::mat4& viewMatrix) override
    {
        m_viewMatrix = viewMatrix;
    }

    void SetProjectionMatrix(const glm::mat4& projectionMatrix) override
    {
        m_projectionMatrix = projectionMatrix;
    }

    void SetRayDirection(const glm::vec3& rayDirection) override
    {
        m_rayDirection = rayDirection; // will be overwritten in update!!
    }

    void SetRayLength(const float len) override
    {
        m_rayLength = len;
    }

    void SetRayStartPosition(const glm::vec3& position) override
    {
        m_rayStartPosition = position;
    }

    void SetOrientationOffsetAngles(const glm::vec2& angles) override
    {
        m_orientationOffsetAngles = angles;
    }
    
    Ray GetRay() const override
    {
        return Ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
    }

#ifdef RENDER_RAYCASTS
    std::shared_ptr<IModel> GetModel() const override
    {
        return m_model;
    }
#endif
public:
    void operator()(const MouseEvent& moveEvent)
    {
        const glm::vec2 position{ moveEvent.position.x, m_viewPortDimensions.y - moveEvent.position.y };
        m_currentMousePosition = glm::clamp(position, glm::vec2(0.0f, 0.0f), m_viewPortDimensions);
    }

    void operator()(const WindowResizeEvent& vwindowResizeEvent)
    {
        m_viewPortDimensions = glm::vec2(vwindowResizeEvent.width, vwindowResizeEvent.height);
    }

protected:    
    glm::mat4 m_viewMatrix;

    glm::mat4 m_projectionMatrix;

    glm::vec2 m_currentMousePosition;
    
    glm::vec3 m_rayDirection;

    glm::vec3 m_rayStartPosition;

    float m_rayLength{ 500.0f };

    glm::vec2 m_orientationOffsetAngles{ 0.0f, 0.0f };

    glm::vec2 m_viewPortDimensions{ 0.0f, 0.0f };
#ifdef RENDER_RAYCASTS
    std::shared_ptr<IModel> m_model;
#endif
private:
    EventHandler<MouseRayCasterComponent, MouseEvent> m_mouseMoveHandler{ *this };

    EventHandler<MouseRayCasterComponent, WindowResizeEvent> m_viewFrustumChangeHandler{ *this };
};

class RayCasterComponentFactory {
public:
    std::unique_ptr<IRayCasterComponent> CreateRayCaster()
    {
        return std::make_unique<RayCasterComponent>();
    }

    std::unique_ptr<IMouseRayCasterComponent> CreateMouseRayCaster()
    {
        return std::make_unique<MouseRayCasterComponent>();
    }
};

class ISelectableComponent {
public:
    virtual bool IsSelected() const = 0;

    virtual void SetSelected(const bool selected) = 0;

    virtual const glm::vec3& GetPostiion() const = 0;

    virtual void SetPosition(const glm::vec3& at) = 0;

    virtual void Reset() = 0;

public:
    virtual ~ISelectableComponent() = default;
};


class SelectableComponent final : public ISelectableComponent {
public:
    bool IsSelected() const override
    {
        return m_selected;
    }

    void SetSelected(const bool selected) override
    {
        m_selected = selected;
    }

    
    const glm::vec3& GetPostiion() const override
    {
        return m_position;
    }

    void SetPosition(const glm::vec3& at) override
    {
        m_position = at;
    }

    void Reset() override
    {
        m_selected = false;
        m_position = glm::vec3(std::numeric_limits<float>::min());
    }

private:
    bool m_selected{ false };

    glm::vec3 m_position{ std::numeric_limits<float>::min() };
};

#endif // !__RAY_CASTING_H__
