#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "render/ViewFrustum.h"

class ICameraComponent {
public:
    virtual const glm::mat4& LookAt() const = 0;

    virtual void Reset() = 0;

    virtual void AddPitch(const float amountInDegrees) = 0;

    virtual void AddYaw(const float amountInDegrees) = 0;

    virtual void AddOrientation(const glm::quat& orientationDiff) = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void SetOrientation(const float pitch, const float yaw) = 0;

    virtual void AddPosition(const glm::vec3& positionDiff) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual glm::vec3 GetForwardDirection() const = 0;

    virtual glm::vec3 GetRightDirection() const = 0;

    virtual glm::vec3 GetUpDirection() const = 0;

    virtual glm::vec3 GetPosition() const = 0;

    virtual glm::quat GetOrientation() const = 0;

    virtual const prev_test::render::ViewFrustum& GetViewFrustum() const = 0;

    virtual void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) = 0;

public:
    virtual ~ICameraComponent() = default;
};

class CameraComponent : public ICameraComponent {
private:
    const glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

    const glm::vec3 m_defaultForwardDirection{ 0.0f, 0.0f, -1.0f };

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialPosition;

private:
    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_positionDelta;

    glm::vec3 m_forwardDirection;

    glm::vec3 m_rightDirection;

    glm::quat m_orientationDelta;

    glm::mat4 m_viewMatrix;

    glm::vec2 m_prevTouchPosition;

    prev_test::render::ViewFrustum m_viewFrustum{ 45.0f, 10.0f, 300.0f };

    bool m_orientationChanged{ false };

    bool m_positionChanged{ false };

public:
    CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition)
        : m_initialOrientation(initialOrientation)
        , m_initialPosition(initialPosition)
    {
        Reset();
    }

    virtual ~CameraComponent() = default;

private:
    void UpdatePosition()
    {
        m_position += m_positionDelta;

        m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void UpdateOrientation()
    {
        //add the two quaternions
        glm::quat orientation = glm::normalize(m_orientationDelta);

        // update forward direction from the quaternion
        m_forwardDirection = glm::normalize(orientation * m_forwardDirection);

        // compute right direction from up and formward
        m_rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));

        // reset current iteration deltas
        m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    void Update()
    {
        if (m_orientationChanged) {
            UpdateOrientation();
        }

        if (m_positionChanged) {
            UpdatePosition();
        }

        if (m_orientationChanged || m_positionChanged) {
            m_viewMatrix = glm::lookAt(m_position, m_position + m_forwardDirection, m_upDirection);
            m_orientation = glm::quat_cast(m_viewMatrix);
        }

        m_orientationChanged = false;
        m_positionChanged = false;
    }

public:
    const glm::mat4& LookAt() const override
    {
        return m_viewMatrix;
    }

    void Reset() override
    {
        m_position = m_initialPosition;
        m_orientation = m_initialOrientation;
        m_forwardDirection = m_initialOrientation * m_defaultForwardDirection;
        m_rightDirection = glm::cross(m_forwardDirection, m_upDirection);

        m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
        m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        m_viewMatrix = glm::mat4(1.0f);

        m_prevTouchPosition = glm::vec2(0.0f, 0.0f);

        m_positionChanged = true;
        m_orientationChanged = true;
        Update();
    }

    void AddPitch(float amountInDegrees) override
    {
        m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_rightDirection);
        m_orientationChanged = true;
        Update();
    }

    void AddYaw(float amountInDegrees) override
    {
        m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_upDirection);
        m_orientationChanged = true;
        Update();
    }

    void AddOrientation(const glm::quat& orientationDiff) override
    {
        m_orientationDelta *= orientationDiff;
        m_orientationChanged = true;
        Update();
    }

    void SetOrientation(const glm::quat& orientation) override
    {
        m_forwardDirection = glm::normalize(orientation * m_defaultForwardDirection);
        m_orientationChanged = true;
        Update();
    }

    void SetOrientation(const float pitchAmountInDegrees, const float yawAmountInDeregrees) override
    {
        glm::quat newOrientation = glm::angleAxis(glm::radians(pitchAmountInDegrees), m_rightDirection);
        newOrientation *= glm::angleAxis(glm::radians(yawAmountInDeregrees), m_upDirection);
        SetOrientation(newOrientation);
    }

    void AddPosition(const glm::vec3& positionDiff) override
    {
        m_positionDelta = positionDiff;
        m_positionChanged = true;
        Update();
    }

    void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
        m_positionChanged = true;
        Update();
    }

    glm::vec3 GetForwardDirection() const override
    {
        return m_forwardDirection;
    }

    glm::vec3 GetRightDirection() const override
    {
        return m_rightDirection;
    }

    glm::vec3 GetUpDirection() const override
    {
        return m_upDirection;
    }

    glm::vec3 GetPosition() const override
    {
        return m_position;
    }

    glm::quat GetOrientation() const override
    {
        return m_orientation;
    }

    const prev_test::render::ViewFrustum& GetViewFrustum() const override
    {
        return m_viewFrustum;
    }

    void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) override
    {
        m_viewFrustum = viewFrustum;
    }
};

class CameraComponentFactory {
public:
    std::unique_ptr<ICameraComponent> Create(const glm::quat& orient, const glm::vec3& pos) const
    {
        return std::make_unique<CameraComponent>(orient, pos);
    }
};

#endif