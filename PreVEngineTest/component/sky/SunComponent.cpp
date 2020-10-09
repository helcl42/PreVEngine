#include "SunComponent.h"

namespace prev_test::component::sky {
SunComponent::SunComponent(const std::shared_ptr<Flare>& flare, const std::shared_ptr<prev_test::render::IModel>& model)
    : m_flare(flare)
    , m_model(model)
{
}

void SunComponent::Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition)
{
    glm::vec2 sunPositionInScreenSpace;
    if (ConvertWorldSpaceToScreenSpaceCoord(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunPositionInScreenSpace)) {
        const glm::vec2 screenCenter{ 0.0f, 0.0f };
        glm::vec2 sunToCenter{ screenCenter - sunPositionInScreenSpace };
        const float brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);
        if (brightness > 0) {
            m_flare->SetScreenSpacePosition(sunPositionInScreenSpace);
        } else {
            m_flare->SetScreenSpacePosition(glm::vec2(-100.0f));
        }
    } else {
        m_flare->SetScreenSpacePosition(glm::vec2(-100.0f));
    }
}

std::shared_ptr<Flare> SunComponent::GetFlare() const
{
    return m_flare;
}

std::shared_ptr<prev_test::render::IModel> SunComponent::GetModel() const
{
    return m_model;
}

bool SunComponent::ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition)
{
    const auto coord = projectionMatrix * viewMatrix * glm::vec4{ worldPosition, 1.0f };
    if (coord.w <= 0.0f) {
        return false;
    }
    convertedSceenPosition = glm::vec2(coord.x / coord.w, coord.y / coord.w);
    return true;
}

} // namespace prev_test::component::sky