#include "SunComponent.h"

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

    glm::vec2 ComputeFlareNdcPosition(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition)
    {
        glm::vec2 sunNdcPosition;
        if (ConvertWorldSpaceToNdc(eyePosition + sunPosition, projectionMatrix, viewMatrix, sunNdcPosition)) {
            const glm::vec2 cameraNdcPosition{ GetCameraNdc(projectionMatrix) };
            const glm::vec2 sunToCenter{ cameraNdcPosition - sunNdcPosition };
            const float brightness{ 1.0f - (glm::length(sunToCenter) / 1.4f) };
            if (brightness > 0) {
                return sunNdcPosition;
            }
        }
        return { -100.0f, -100.0f };
    }
} // namespace

SunComponent::SunComponent(const Flare& flare, const std::shared_ptr<prev_test::render::IMaterial>& material, const std::shared_ptr<prev_test::render::IModel>& model)
    : m_flare(flare)
    , m_material(material)
    , m_model(model)
{
}

const Flare& SunComponent::GetFlare() const
{
    return m_flare;
}

std::shared_ptr<prev_test::render::IMaterial> SunComponent::GetMaterial() const
{
    return m_material;
}

std::shared_ptr<prev_test::render::IModel> SunComponent::GetModel() const
{
    return m_model;
}

glm::vec2 SunComponent::ComputeFlarePosition(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& eyePosition, const glm::vec3& sunPosition) const
{
    return ComputeFlareNdcPosition(projectionMatrix, viewMatrix, eyePosition, sunPosition);
}
} // namespace prev_test::component::sky