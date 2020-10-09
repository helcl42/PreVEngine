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

void CameraComponent::AddPitch(float amountInDegrees)
{
    m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_rightDirection);
    m_orientationChanged = true;
    Update();
}

void CameraComponent::AddYaw(float amountInDegrees)
{
    m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_upDirection);
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
    m_forwardDirection = glm::normalize(orientation * m_defaultForwardDirection);
    m_orientationChanged = true;
    Update();
}

void CameraComponent::SetOrientation(const float pitchAmountInDegrees, const float yawAmountInDeregrees)
{
    glm::quat newOrientation = glm::angleAxis(glm::radians(pitchAmountInDegrees), m_rightDirection);
    newOrientation *= glm::angleAxis(glm::radians(yawAmountInDeregrees), m_upDirection);
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
    return m_upDirection;
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

    m_positionDelta = glm::vec3(0.0f, 0.0f, 0.0f);
}

void CameraComponent::UpdateOrientation()
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
        m_orientation = glm::quat_cast(m_viewMatrix);
    }

    m_orientationChanged = false;
    m_positionChanged = false;
}

} // namespace prev_test::component::camera