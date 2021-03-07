#include "CameraComponent.h"

namespace prev_test::component::camera {
CameraComponent::CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition)
    : m_initialOrientation(initialOrientation)
    , m_initialPosition(initialPosition)
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

    m_forwardDirection = glm::normalize( m_initialOrientation * m_defaultForwardDirection);
    m_rightDirection = glm::cross(m_forwardDirection, m_defaultUpDirection);

    m_positionDelta = glm::vec3{};
    m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    m_positionChanged = true;
    m_orientationChanged = true;
    Update();
}

void CameraComponent::AddPitch(float amountInDegrees)
{
    m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_rightDirection);
    m_orientationChanged = true;
    Update();
}

void CameraComponent::AddYaw(float amountInDegrees)
{
    m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_defaultUpDirection);
    m_orientationChanged = true;
    Update();
}

void CameraComponent::AddOrientation(const glm::quat& orientationDiff)
{
    m_orientationDelta *= orientationDiff;
    m_orientationChanged = true;
    Update();
}

void CameraComponent::SetOrientation(const glm::quat& orientation)
{
    const glm::quat orientationDelta{ m_orientation * glm::inverse(orientation) };
    m_orientationDelta = orientationDelta;
    m_orientationChanged = true;
    Update();
}

void CameraComponent::SetOrientation(const float pitchAmountInDegrees, const float yawAmountInDegrees)
{
    const glm::quat newOrientation{ glm::angleAxis(glm::radians(pitchAmountInDegrees), m_rightDirection) * glm::angleAxis(glm::radians(yawAmountInDegrees), m_defaultUpDirection) };
    SetOrientation(newOrientation);
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
    return m_defaultUpDirection;
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
    m_forwardDirection = glm::normalize(m_orientationDelta * m_forwardDirection);
    m_rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_defaultUpDirection));

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
        m_viewMatrix = glm::lookAt(m_position, m_position + m_forwardDirection, m_defaultUpDirection);
        m_orientation = glm::quat_cast(m_viewMatrix);
    }

    m_orientationChanged = false;
    m_positionChanged = false;
}

} // namespace prev_test::component::camera