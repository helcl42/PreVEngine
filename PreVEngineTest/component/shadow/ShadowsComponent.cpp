#include "ShadowsComponent.h"
#include "ShadowsCommon.h"

#include "../../common/intersection/AABB.h"

#include <prev/core/DeviceProvider.h>
#include <prev/util/MathUtils.h>

namespace prev_test::component::shadow {
ShadowsComponent::ShadowsComponent(const uint32_t cascadesCount, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::core::memory::image::IImageBuffer>& depthBuffer, const std::vector<ShadowsCascade>& cascades)
    : m_cascadesCount(cascadesCount)
    , m_renderPass(renderPass)
    , m_depthBuffer(depthBuffer)
    , m_cascades(cascades)
{
}

ShadowsComponent::~ShadowsComponent()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

    m_renderPass = nullptr;

    for (uint32_t i = 0; i < m_cascadesCount; i++) {
        auto& cascade{ m_cascades.at(i) };
        cascade.Destroy(*device);
    }

    m_depthBuffer->Destroy();
}

void ShadowsComponent::Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    const auto cascadeSplits{ GenerateCaascadeSplits(nearClippingPlane, farClippingPlane) };
    const auto inverseWorldToClipSpaceTransform = glm::inverse(projectionMatrix * viewMatrix);

    float lastSplitDistance{ 0.0 };
    for (uint32_t i = 0; i < m_cascadesCount; i++) {
        const float splitDistance = cascadeSplits[i];

        // Calculate orthographic projection matrix for ith cascade
        UpdateCascade(lightDirection, inverseWorldToClipSpaceTransform, nearClippingPlane, farClippingPlane, splitDistance, lastSplitDistance, m_cascades[i]);

        lastSplitDistance = splitDistance;
    }
}

std::shared_ptr<prev::render::pass::RenderPass> ShadowsComponent::GetRenderPass() const
{
    return m_renderPass;
}

const ShadowsCascade& ShadowsComponent::GetCascade(const uint32_t cascadeIndex) const
{
    return m_cascades.at(cascadeIndex);
}

VkExtent2D ShadowsComponent::GetExtent() const
{
    return { m_depthBuffer->GetExtent().width, m_depthBuffer->GetExtent().height };
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> ShadowsComponent::GetImageBuffer() const
{
    return m_depthBuffer;
}

std::vector<glm::vec3> ShadowsComponent::GenerateFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform, const float splitDistance, const float lastSplitDistance) const
{
    auto frustumCorners{ prev::util::math::GetFrustumCorners(inverseWorldToClipSpaceTransform) };

    for (uint32_t i = 0; i < 4; i++) {
        const glm::vec3 dist{ frustumCorners[i + 4] - frustumCorners[i] };
        frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDistance);
        frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDistance);
    }

    return frustumCorners;
}

glm::vec3 ShadowsComponent::CalculateFrustumCenter(const std::vector<glm::vec3>& frustumCorners) const
{
    glm::vec3 frustumCenter{ 0.0f };
    for (uint32_t i = 0; i < 8; i++) {
        frustumCenter += frustumCorners[i];
    }
    frustumCenter /= 8.0f;
    return frustumCenter;
}

float ShadowsComponent::CalculateFrustumRadius(const std::vector<glm::vec3>& frustumCorners, const glm::vec3& frustumCenter) const
{
    float radius{ 0.0f };
    for (uint32_t i = 0; i < 8; i++) {
        const float distance{ glm::length(frustumCorners[i] - frustumCenter) };
        radius = glm::max(radius, distance);
    }
    radius = std::ceil(radius * 16.0f) / 16.0f;
    return radius;
}

std::vector<float> ShadowsComponent::GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const
{
    std::vector<float> cascadeSplits(m_cascadesCount);

    const float clipRange{ farClippingPlane - nearClippingPlane };
    const float minZ{ nearClippingPlane };
    const float maxZ{ nearClippingPlane + clipRange };
    const float range{ maxZ - minZ };
    const float ratio{ maxZ / minZ };

    // Calculate split depths based on view camera furstum
    for (uint32_t i = 0; i < m_cascadesCount; i++) {
        const float p{ (i + 1) / static_cast<float>(m_cascadesCount) };
        const float log{ minZ * powf(ratio, p) };
        const float uniform{ minZ + range * p };
        const float d{ CASCADES_SPLIT_LAMBDA * (log - uniform) + uniform };
        cascadeSplits[i] = (d - nearClippingPlane) / clipRange;
    }

    return cascadeSplits;
}

void ShadowsComponent::UpdateCascade(const glm::vec3& lightDirection, const glm::mat4& inverseCameraTransform, const float nearClippingPlane, const float farClippingPlane, const float splitDistance, const float lastSplitDistance, ShadowsCascade& outCascade) const
{
    const auto clipRange{ farClippingPlane - nearClippingPlane };
    const auto frustumCorners{ GenerateFrustumCorners(inverseCameraTransform, splitDistance, lastSplitDistance) };
    const auto frustumCenter{ CalculateFrustumCenter(frustumCorners) };
    const auto radius{ CalculateFrustumRadius(frustumCorners, frustumCenter) };

    prev_test::common::intersection::AABB aabb{ radius };

    const glm::mat4 lightViewMatrix{ glm::lookAt(frustumCenter - lightDirection * -aabb.minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f)) };
    const glm::mat4 lightOrthoProjectionMatrix{ glm::ortho(aabb.minExtents.x, aabb.maxExtents.x, aabb.minExtents.y, aabb.maxExtents.y, 0.0f, aabb.maxExtents.z - aabb.minExtents.z) };

    outCascade.startSplitDepth = (nearClippingPlane + lastSplitDistance * clipRange) * -1.0f;
    outCascade.endSplitDepth = (nearClippingPlane + splitDistance * clipRange) * -1.0f;
    outCascade.viewMatrix = lightViewMatrix;
    outCascade.projectionMatrix = lightOrthoProjectionMatrix;
}
} // namespace prev_test::component::shadow
