#include "CameraComponent.h"

namespace prev_test::component::camera {
    CameraComponent::CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition, const bool useFixedUp)
    : m_initialOrientation(initialOrientation)
    , m_initialPosition(initialPosition)
    , m_useFixedUp(useFixedUp)
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

    if(m_useFixedUp) {
        m_upDirection = DEFAULT_UP_DIRECTION;
        m_forwardDirection = glm::normalize(m_initialOrientation * DEFAULT_FORWARD_DIRECTION);
        m_rightDirection = glm::cross(m_forwardDirection, DEFAULT_UP_DIRECTION);
    } else {
        m_positionDelta = glm::vec3{};
        m_orientationDelta = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

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

void CameraComponent::AddRoll(const float amountInDegrees)
{
    m_orientationDelta *= glm::angleAxis(glm::radians(amountInDegrees), m_forwardDirection);
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
    if(m_useFixedUp) {
        m_orientationDelta = m_orientation * glm::inverse(orientation); // orientation delta to previous orientation
    } else {
        m_orientation = orientation;
    }
    m_orientationChanged = true;
    Update();
}

void CameraComponent::SetOrientation(const float pitchAmountInDegrees, const float yawAmountInDegrees, const float rollAmountInDegrees)
{
    const glm::quat newOrientation( glm::angleAxis(glm::radians(pitchAmountInDegrees), m_rightDirection) * glm::angleAxis(glm::radians(yawAmountInDegrees), m_upDirection) * glm::angleAxis(glm::radians(rollAmountInDegrees), m_forwardDirection) );
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

    m_positionDelta = glm::vec3{};
}

void CameraComponent::UpdateOrientation()
{
    if(m_useFixedUp) {
        m_forwardDirection = glm::normalize(m_orientationDelta * m_forwardDirection);
        m_rightDirection = glm::normalize(glm::cross(m_forwardDirection, m_upDirection));
    } else {
        m_orientation = glm::normalize(m_orientation * m_orientationDelta);
        const auto orientationMat{glm::mat3_cast(m_orientation)};
        m_rightDirection = orientationMat[0];
        m_upDirection = orientationMat[1];
        m_forwardDirection = orientationMat[2];
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
    }

    m_orientationChanged = false;
    m_positionChanged = false;
}

} // namespace prev_test::component::camera