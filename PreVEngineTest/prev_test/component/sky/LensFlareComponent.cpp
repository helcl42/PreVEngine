#include "LensFlareComponent.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::sky {
namespace {
bool ConvertWorldSpaceToNdc(const glm::vec3& worldPosition, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, glm::vec2& outNdcPosition)
{
    const auto clipPosition{ projectionMatrix * viewMatrix * glm::vec4(worldPosition, 1.0f) };
    if (clipPosition.w <= 0.0f) {
        return false;
    }
    outNdcPosition = glm::vec2(clipPosition.x, clipPosition.y) / clipPosition.w;
    return true;
}

glm::vec2 GetCameraNdc(const glm::mat4& projectionMatrix)
{
    const glm::vec4 cameraClipSpace{ projectionMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) };
    const glm::vec4 cameraNdc{ cameraClipSpace / cameraClipSpace.w };
    return { cameraNdc.x, cameraNdc.y };
}

glm::vec2 ComputeFlarePosition(const size_t index, const float spacing, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition)
{
    glm::vec2 sunNdcPosition;
    if (ConvertWorldSpaceToNdc(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunNdcPosition)) {
        const glm::vec2 cameraNdcPosition{ GetCameraNdc(projectionMatrix) };
        const glm::vec2 sunToCenter{cameraNdcPosition - sunNdcPosition};
        const float brightness{1.0f - (glm::length(sunToCenter) / 1.4f)};
        if (brightness > 0) {
            const auto offset{sunToCenter * (static_cast<float>(index + 1) * spacing)};
            const auto flarePosition{sunNdcPosition + offset};
            return flarePosition;
        }
    }
    return { -100.0f, -100.0f };
}
}

LensFlareComponent::LensFlareComponent(const std::vector<std::shared_ptr<Flare>>& flares, const float spacing, const std::shared_ptr<prev_test::render::IModel>& model)
    : m_flares(flares)
    , m_spacing(spacing)
    , m_model(model)
{
}

const std::vector<std::shared_ptr<Flare>>& LensFlareComponent::GetFlares() const
{
    return m_flares;
}

std::shared_ptr<prev_test::render::IModel> LensFlareComponent::GetModel() const
{
    return m_model;
}

std::vector<glm::vec2> LensFlareComponent::ComputeFlarePositions(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const
{
    std::vector<glm::vec2> positions(m_flares.size());
    for(size_t i = 0; i < m_flares.size(); ++i) {
        positions[i] = ComputeFlarePosition(i, m_spacing, projectionMatrix, viewMatrix, eyePosition, sunPosition);
    }
    return positions;
}
} // namespace prev_test::component::sky