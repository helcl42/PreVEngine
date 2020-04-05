#ifndef __RAY_CASTING_H__
#define __RAY_CASTING_H__

#include "General.h"

struct RayEvent {
    glm::vec3 direction;

    glm::vec3 startPosition;

    float length;

    RayEvent() = default;

    RayEvent(const glm::vec3& dir, const glm::vec3& startPos, const float len)
        : direction(glm::normalize(dir))
        , startPosition(startPos)
        , length(len)
    {
    }

    virtual ~RayEvent() = default;

    friend std::ostream& operator<<(std::ostream& out, const RayEvent& rayEvent)
    {
        out << "Direction: (" << rayEvent.direction.x << ", " << rayEvent.direction.y << ", " << rayEvent.direction.z << ")" << std::endl;
        out << "StartPos:  (" << rayEvent.startPosition.x << ", " << rayEvent.startPosition.y << ", " << rayEvent.startPosition.z << ")" << std::endl;
        out << "Length:     " << rayEvent.length << std::endl;
        return out;
    }
};

class IRayCasterComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual void SetViewPortDimensions(const glm::vec2& viewPortDimensions) = 0;

    virtual const glm::vec2& GetViewPortDimensions() const = 0;

    virtual const glm::mat4& GetViewMatrix() const = 0;

    virtual void SetViewMatrix(const glm::mat4& viewMatrix) = 0;

    virtual const glm::mat4& GetProjectionMatrix() const = 0;

    virtual void SetProjectionMatrix(const glm::mat4& projectionMatrix) = 0;

    virtual const glm::vec3& GetRayDirection() const = 0;

    virtual void SetRayLength(const float len) = 0;
    
    virtual float GetRayLength() const = 0;

    virtual const glm::vec3& GetStartPosition() const = 0;

    virtual void SetRayStartPosition(const glm::vec3& position) = 0;

    virtual float GetPitchCompensationAngle() const = 0;
    
    virtual void SetPitchCompensationAngle(const float angle) = 0;

public:
    virtual ~IRayCasterComponent() {}
};

class MouseRayCasterComponent : public IRayCasterComponent {
public:
    MouseRayCasterComponent() = default;

    virtual ~MouseRayCasterComponent() = default;

public:
    virtual void Update(float deltaTime) override
    {
        const glm::vec2 normalizeDeviceCoords = MathUtil::FromViewPortSpaceToNormalizedDeviceSpace(m_viewPortDimensions, glm::vec2(m_currentMousePosition.x, m_currentMousePosition.y));     
        const glm::vec4 clipSpaceCoords = MathUtil::FromNormalizedDeviceSpaceToClipSpace(normalizeDeviceCoords);
        const glm::vec4 eyeSpaceCoords = MathUtil::FromClipSpaceToCameraSpace(m_projectionMatrix, clipSpaceCoords);
        const glm::vec3 worldCoords = MathUtil::FromCameraSpaceToWorldSpace(m_viewMatrix, eyeSpaceCoords);
        m_rayDirection = glm::normalize(worldCoords);

        if (m_pitchCompensationAngle != 0.0f) {
            const auto rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_rayDirection);
            const auto compensationTransform = glm::rotate(glm::mat4(1.0f), glm::radians(m_pitchCompensationAngle), rightVec);
            m_rayDirection = glm::normalize(compensationTransform * glm::vec4(m_rayDirection, 0.0f));
        }

        RayEvent rayEvent{ m_rayDirection, m_startPosition, m_rayLength };
        EventChannel::Broadcast(rayEvent);
        std::cout << rayEvent << std::endl;
    }

    void SetViewPortDimensions(const glm::vec2& viewPortDimensions) override
    {
        m_viewPortDimensions = viewPortDimensions;
    }

    const glm::vec2& GetViewPortDimensions() const override
    {
        return m_viewPortDimensions;
    }

    const glm::mat4& GetViewMatrix() const override
    {
        return m_viewMatrix;
    }

    void SetViewMatrix(const glm::mat4& viewMatrix) override
    {
        m_viewMatrix = viewMatrix;
    }

    const glm::mat4& GetProjectionMatrix() const override
    {
        return m_projectionMatrix;
    }

    void SetProjectionMatrix(const glm::mat4& projectionMatrix) override
    {
        m_projectionMatrix = projectionMatrix;
    }

    const glm::vec3& GetRayDirection() const override
    {
        return m_rayDirection;
    }

    void SetRayLength(const float len) override
    {
        m_rayLength = len;
    }

    float GetRayLength() const override
    {
        return m_rayLength;
    }

    const glm::vec3& GetStartPosition() const override
    {
        return m_startPosition;
    }

    void SetRayStartPosition(const glm::vec3& position) override
    {
        m_startPosition = position;
    }

    float GetPitchCompensationAngle() const override
    {
        return m_pitchCompensationAngle;
    }

    void SetPitchCompensationAngle(const float angle) override
    {
        m_pitchCompensationAngle = angle;
    }

public:
    void operator()(const MouseEvent& moveEvent)
    {
        const glm::vec2 position{ moveEvent.position.x, moveEvent.position.y };
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

    glm::vec3 m_startPosition;

    float m_rayLength{ 500.0f };

    float m_pitchCompensationAngle{ 0.0f };

    glm::vec2 m_viewPortDimensions{ 0.0f, 0.0f };

private:
    EventHandler<MouseRayCasterComponent, MouseEvent> m_mouseMoveHandler{ *this };

    EventHandler<MouseRayCasterComponent, WindowResizeEvent> m_viewFrustumChangeHandler{ *this };
};

class RayCasterComponentFactory {
public:
    std::unique_ptr<IRayCasterComponent> Create()
    {
        return std::make_unique<MouseRayCasterComponent>();
    }
};

#endif // !__RAY_CASTING_H__
