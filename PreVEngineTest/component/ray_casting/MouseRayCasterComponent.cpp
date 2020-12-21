#include "MouseRayCasterComponent.h"
#include "RayCastingEvents.h"
#include "RayModelFactory.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::ray_casting {
void MouseRayCasterComponent::Update(float deltaTime)
{
    const glm::vec2 normalizeDeviceCoords = prev::util::MathUtil::FromViewPortSpaceToNormalizedDeviceSpace(m_viewPortDimensions, glm::vec2(m_currentMousePosition.x, m_currentMousePosition.y));
    const glm::vec4 clipSpaceCoords = prev::util::MathUtil::FromNormalizedDeviceSpaceToClipSpace(normalizeDeviceCoords);
    const glm::vec4 eyeSpaceCoords = prev::util::MathUtil::FromClipSpaceToCameraSpace(m_projectionMatrix, clipSpaceCoords);
    const glm::vec3 worldCoords = prev::util::MathUtil::FromCameraSpaceToWorldSpace(m_viewMatrix, eyeSpaceCoords);

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

    prev_test::common::intersection::Ray ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
    prev::event::EventChannel::Post(RayEvent{ ray });

#ifdef RENDER_RAYCASTS
    RayModelFactory modelFactory{};
    m_model = modelFactory.Create(ray);
#endif
}

void MouseRayCasterComponent::SetViewPortDimensions(const glm::vec2& viewPortDimensions)
{
    m_viewPortDimensions = viewPortDimensions;
}

void MouseRayCasterComponent::SetViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
}

void MouseRayCasterComponent::SetProjectionMatrix(const glm::mat4& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
}

void MouseRayCasterComponent::SetRayDirection(const glm::vec3& rayDirection)
{
    m_rayDirection = rayDirection; // will be overwritten in update!!
}

void MouseRayCasterComponent::SetRayLength(const float len)
{
    m_rayLength = len;
}

void MouseRayCasterComponent::SetRayStartPosition(const glm::vec3& position)
{
    m_rayStartPosition = position;
}

void MouseRayCasterComponent::SetOrientationOffsetAngles(const glm::vec2& angles)
{
    m_orientationOffsetAngles = angles;
}

prev_test::common::intersection::Ray MouseRayCasterComponent::GetRay() const
{
    return prev_test::common::intersection::Ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
}

#ifdef RENDER_RAYCASTS
std::shared_ptr<prev_test::render::IModel> MouseRayCasterComponent::GetModel() const
{
    return m_model;
}
#endif

void MouseRayCasterComponent::operator()(const prev::input::mouse::MouseEvent& moveEvent)
{
    const glm::vec2 position{ moveEvent.position.x, m_viewPortDimensions.y - moveEvent.position.y };
    m_currentMousePosition = glm::clamp(position, glm::vec2(0.0f, 0.0f), m_viewPortDimensions);
}

void MouseRayCasterComponent::operator()(const prev::window::WindowResizeEvent& vwindowResizeEvent)
{
    m_viewPortDimensions = glm::vec2(vwindowResizeEvent.width, vwindowResizeEvent.height);
}
} // namespace prev_test::component::ray_casting