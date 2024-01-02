#include "LightComponent.h"

namespace prev_test::component::light {
LightComponent::LightComponent(const glm::vec3& pos)
    : m_position(pos)
{
}

LightComponent::LightComponent(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& attenuation)
    : m_position(pos)
    , m_color(color)
    , m_attenuation(attenuation)
{
}

glm::mat4 LightComponent::LookAt() const
{
    return glm::lookAt(m_position, m_lookAtPosition, m_upDirection);
}

glm::mat4 LightComponent::GetProjectionMatrix() const
{
    return m_viewFrustum.CreateProjectionMatrix(1.0f); // square frustum base
}

glm::vec3 LightComponent::GetPosition() const
{
    return m_position;
}

void LightComponent::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

glm::vec3 LightComponent::GetDirection() const
{
    return glm::normalize(-m_position);
}

glm::vec3 LightComponent::GetColor() const
{
    return m_color;
}

void LightComponent::SetColor(const glm::vec3& color)
{
    m_color = color;
}

glm::vec3 LightComponent::GetAttenuation() const
{
    return m_attenuation;
}

void LightComponent::SetAttenuation(const glm::vec3& attenuation)
{
    m_attenuation = attenuation;
}

const prev_test::render::ViewFrustum& LightComponent::GetViewFrustum() const
{
    return m_viewFrustum;
}

void LightComponent::SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum)
{
    m_viewFrustum = viewFrustum;
}
} // namespace prev_test::component::light