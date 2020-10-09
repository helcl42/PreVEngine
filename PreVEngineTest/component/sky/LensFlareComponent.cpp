#include "LensFlareComponent.h"

namespace prev_test::component::sky {
LensFlareComponent::LensFlareComponent(const std::vector<std::shared_ptr<Flare> >& flares, float spacing, const std::shared_ptr<prev_test::render::IModel>& model)
    : m_flares(flares)
    , m_spacing(spacing)
    , m_model(model)
{
}

void LensFlareComponent::Update(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition)
{
    glm::vec2 sunPositionInScreenSpace;
    if (ConvertWorldSpaceToScreenSpaceCoord(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunPositionInScreenSpace)) {
        const glm::vec2 screenCenter{ 0.0f, 0.0f };
        glm::vec2 sunToCenter{ screenCenter - sunPositionInScreenSpace };
        const float brightness = 1.0f - (glm::length(sunToCenter) / 1.4f);
        if (brightness > 0) {
            UpdateFlareTexrures(sunToCenter, sunPositionInScreenSpace);
        } else {
            UpdateFlareTexrures(glm::vec2(-100.0f), glm::vec2(-100.0f));
        }
    } else {
        UpdateFlareTexrures(glm::vec2(-100.0f), glm::vec2(-100.0f));
    }
}

const std::vector<std::shared_ptr<Flare> >& LensFlareComponent::GetFlares() const
{
    return m_flares;
}

std::shared_ptr<prev_test::render::IModel> LensFlareComponent::GetModel() const
{
    return m_model;
}

bool LensFlareComponent::ConvertWorldSpaceToScreenSpaceCoord(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& convertedSceenPosition)
{
    const auto coord = projectionMatrix * viewMatrix * glm::vec4{ worldPosition, 1.0f };
    if (coord.w <= 0.0f) {
        return false;
    }
    convertedSceenPosition = glm::vec2(coord.x / coord.w, coord.y / coord.w);
    return true;
}

void LensFlareComponent::UpdateFlareTexrures(const glm::vec2& sunToCenterDirection, const glm::vec2& sunPositionInScreenSpace)
{
    for (unsigned int i = 0; i < m_flares.size(); i++) {
        const auto direction = sunToCenterDirection * ((i + 1) * m_spacing);
        const auto flarePosition = sunPositionInScreenSpace + direction;
        m_flares[i]->SetScreenSpacePosition(flarePosition);
    }
}
} // namespace prev_test::component::sky