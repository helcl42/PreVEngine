#include "RayCasterComponent.h"
#include "RayModelFactory.h"

namespace prev_test::component::ray_casting {
void RayCasterComponent::Update(float deltaTime)
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

#ifdef RENDER_RAYCASTS
    m_model = RayModelFactory{}.Create(GetRay());
#endif
}

void RayCasterComponent::SetRayDirection(const glm::vec3& rayDirection)
{
    m_rayDirection = rayDirection;
}

void RayCasterComponent::SetRayLength(const float len)
{
    m_rayLength = len;
}

void RayCasterComponent::SetRayStartPosition(const glm::vec3& position)
{
    m_rayStartPosition = position;
}

void RayCasterComponent::SetOrientationOffsetAngles(const glm::vec2& angles)
{
    m_orientationOffsetAngles = angles;
}

prev_test::common::intersection::Ray RayCasterComponent::GetRay() const
{
    return prev_test::common::intersection::Ray{ m_rayStartPosition, m_rayDirection, m_rayLength };
}
#ifdef RENDER_RAYCASTS
std::shared_ptr<prev_test::render::IModel> RayCasterComponent::GetModel() const
{
    return m_model;
}
#endif
} // namespace prev_test::component::ray_casting