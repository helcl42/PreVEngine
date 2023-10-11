#include "CameraComponent.h"

namespace prev_test::component::camera {
CameraComponent::CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition, const bool useFixedUp)
    : m_initialOrientation{ initialOrientation }
    , m_initialPosition{ initialPosition }
    , m_useFixedUp{ useFixedUp }
{
    Reset();
}

const glm::mat4& CameraComponent::LookAt() const
{
    return m_viewMatrix;
}

void CameraComponent::Reset()
{
    m_position = m_initialPosition;
    m_orientation = m_initialOrientation;

    m_positionDelta = glm::vec3{};
    m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    m_positionChanged = true;
    m_orientationChanged = true;

    Update();
}

void CameraComponent::AddPitch(const float angle)
{
    AddOrientation(angle, GetRightDirection());
}

void CameraComponent::AddYaw(const float angle)
{
    AddOrientation(angle, GetUpDirection());
}

void CameraComponent::AddRoll(const float angle)
{
    AddOrientation(angle, GetForwardDirection());
}

void CameraComponent::AddOrientation(const float angle, const glm::vec3& axis)
{
    const auto orientationDelta{ glm::angleAxis(angle, axis) };
    AddOrientation(orientationDelta);
}

void CameraComponent::AddOrientation(const glm::quat& orientationDiff)
{
    m_orientationDelta = glm::normalize(orientationDiff * m_orientationDelta);
    m_orientationChanged = true;
    Update();
}

void CameraComponent::SetOrientation(const glm::quat& orientation)
{
    m_orientation = orientation;
    m_orientationChanged = true;
    Update();
}

void CameraComponent::AddPosition(const glm::vec3& positionDiff)
{
    m_positionDelta = positionDiff;
    m_positionChanged = true;
    Update();
}

void CameraComponent::SetPosition(const glm::vec3& position)
{
    m_position = position;
    m_positionChanged = true;
    Update();
}

const glm::vec3& CameraComponent::GetForwardDirection() const
{
    return m_forwardDirection;
}

const glm::vec3& CameraComponent::GetRightDirection() const
{
    return m_rightDirection;
}

const glm::vec3& CameraComponent::GetUpDirection() const
{
    if (m_useFixedUp) {
        return DEFAULT_UP_DIRECTION;
    }
    return m_upDirection;
}

const glm::vec3& CameraComponent::GetDefaultUpDirection() const
{
    return DEFAULT_UP_DIRECTION;
}

const glm::vec3& CameraComponent::GetPosition() const
{
    return m_position;
}

const glm::quat& CameraComponent::GetOrientation() const
{
    return m_orientation;
}

const prev_test::render::ViewFrustum& CameraComponent::GetViewFrustum() const
{
    return m_viewFrustum;
}

void CameraComponent::SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum)
{
    m_viewFrustum = viewFrustum;
}

void CameraComponent::UpdatePosition()
{
    m_position += m_positionDelta;

    m_positionDelta = glm::vec3{};
}

void CameraComponent::UpdateOrientation()
{
    m_orientation = glm::normalize(m_orientationDelta * m_orientation);

    const auto orientationMat{ glm::mat3_cast(m_orientation) };

    // TODO -> should be minus because of OpenGL coord system - RHS -> forward goes towards me
    m_forwardDirection = orientationMat[2];
    if (m_useFixedUp) {
        m_upDirection = DEFAULT_UP_DIRECTION;
        m_rightDirection = glm::normalize(glm::cross(m_upDirection, m_forwardDirection));
    } else {
        m_rightDirection = orientationMat[0];
        m_upDirection = orientationMat[1];
    }

    m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void CameraComponent::Update()
{
    if (m_orientationChanged) {
        UpdateOrientation();
    }

    if (m_positionChanged) {
        UpdatePosition();
    }

    if (m_orientationChanged || m_positionChanged) {
        m_viewMatrix = glm::lookAt(m_position, m_position + m_forwardDirection, m_upDirection);

        // TODO -> we want to do something like this but we still need a mechanism for locking UP axis
        // m_viewMatrix = glm::translate(glm::mat4_cast(m_orientation), m_position);
    }

    m_orientationChanged = false;
    m_positionChanged = false;
}

} // namespace prev_test::component::camera