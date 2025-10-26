#include "ShadowsComponent.h"
#include "ShadowsCommon.h"

#include "../../common/intersection/AABB.h"
#include "../../common/intersection/Frustum.h"

#include <prev/util/MathUtils.h>

namespace prev_test::component::shadow {
ShadowsComponent::ShadowsComponent(prev::core::device::Device& device, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::ImageBuffer>& depthBuffer, const std::vector<ShadowsCascadeRenderData>& cascadesRenderData)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_depthBuffer{ depthBuffer }
    , m_cascadesRenderData{ cascadesRenderData }
    , m_cascadesFrameData{ cascadesRenderData.size() }
{
}

ShadowsComponent::~ShadowsComponent()
{
    m_device.WaitIdle();

    m_renderPass = nullptr;

    for (size_t i = 0; i < m_cascadesRenderData.size(); ++i) {
        m_cascadesRenderData[i].Destroy(m_device);
    }

    m_depthBuffer = nullptr;
}

void ShadowsComponent::Update(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& viewFrustum, const glm::mat4& viewMatrix)
{
    const auto cascadeSplits{ GenerateCascadeSplits(viewFrustum.GetNearClippingPlane(), viewFrustum.GetFarClippingPlane()) };
    const auto clippingRange{ viewFrustum.GetClippingRange() };

    float nearSplitDistance{ MIN_DEPTH };
    for (size_t i = 0; i < m_cascadesRenderData.size(); ++i) {
        const float farSplitDistance{ cascadeSplits[i] };

        const float splitNearClippingPlane{ viewFrustum.GetNearClippingPlane() + clippingRange * nearSplitDistance };
        const float splitFarClippingPlane{ viewFrustum.GetNearClippingPlane() + clippingRange * farSplitDistance };
        const prev_test::render::ViewFrustum splitViewFrustum{ viewFrustum.GetVerticalFov(), 1.0f, splitNearClippingPlane, splitFarClippingPlane };

        UpdateCascadeFrameData(lightDirection, splitViewFrustum, viewMatrix, m_cascadesFrameData[i]);

        nearSplitDistance = farSplitDistance;
    }
}

std::shared_ptr<prev::render::pass::RenderPass> ShadowsComponent::GetRenderPass() const
{
    return m_renderPass;
}

const ShadowsCascadeRenderData& ShadowsComponent::GetCascadeRenderData(const uint32_t cascadeIndex) const
{
    return m_cascadesRenderData[cascadeIndex];
}

const ShadowsCascadeFrameData& ShadowsComponent::GetCascadeFrameData(const uint32_t cascadeIndex) const
{
    return m_cascadesFrameData[cascadeIndex];
}

VkExtent2D ShadowsComponent::GetExtent() const
{
    return { m_depthBuffer->GetExtent().width, m_depthBuffer->GetExtent().height };
}

std::shared_ptr<prev::render::buffer::ImageBuffer> ShadowsComponent::GetImageBuffer() const
{
    return m_depthBuffer;
}

std::vector<float> ShadowsComponent::GenerateCascadeSplits(const float nearClippingPlane, const float farClippingPlane) const
{
    const float minZ{ std::min(nearClippingPlane, farClippingPlane) };
    const float maxZ{ std::max(nearClippingPlane, farClippingPlane) };
    const float range{ maxZ - minZ };
    const float ratio{ maxZ / minZ };

    const uint32_t cascaedesCount{ static_cast<uint32_t>(m_cascadesRenderData.size()) };
    std::vector<float> cascadeSplits(cascaedesCount);

    // Calculate split depths based on view camera furstum
    for (uint32_t i = 0; i < cascaedesCount; ++i) {
        const float p{ (i + 1) / static_cast<float>(cascaedesCount) };
        const float log{ minZ * powf(ratio, p) };
        const float uniform{ minZ + range * p };
        const float d{ CASCADES_SPLIT_LAMBDA * (log - uniform) + uniform };
        const float split{ (d - minZ) / range };
        if constexpr (REVERSE_DEPTH) {
            cascadeSplits[i] = 1.0f - split;
        } else {
            cascadeSplits[i] = split;
        }
    }

    return cascadeSplits;
}

void ShadowsComponent::UpdateCascadeFrameData(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& cascadeViewFrustum, const glm::mat4& lightViewMatrix, ShadowsCascadeFrameData& inOutFrameData) const
{
    const prev_test::common::intersection::Frustum frustum{ cascadeViewFrustum.CreateProjectionMatrix(), lightViewMatrix }; // symmetric aspect ratio for shadows, thus 1.0f
    const prev_test::common::intersection::AABB aabb{ frustum.GetRadius() };

    const auto frustumCenter{ frustum.GetCenter().position };
    const glm::mat4 cascadeLightViewMatrix{ glm::lookAt(frustumCenter - lightDirection * -aabb.minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f)) };

    glm::mat4 cascadeLightOrthoProjectionMatrix;
    if constexpr (REVERSE_DEPTH) {
        cascadeLightOrthoProjectionMatrix = prev::util::math::CreateOrthographicProjectionMatrix(aabb.minExtents.x, aabb.maxExtents.x, aabb.minExtents.y, aabb.maxExtents.y, aabb.maxExtents.z - aabb.minExtents.z, 0.0f);
    } else {
        cascadeLightOrthoProjectionMatrix = prev::util::math::CreateOrthographicProjectionMatrix(aabb.minExtents.x, aabb.maxExtents.x, aabb.minExtents.y, aabb.maxExtents.y, 0.0f, aabb.maxExtents.z - aabb.minExtents.z);
    }

    const float startSplitDepth{ cascadeViewFrustum.GetNearClippingPlane() * -1.0f };
    const float endSplitDepth{ cascadeViewFrustum.GetFarClippingPlane() * -1.0f };

    inOutFrameData = ShadowsCascadeFrameData{ startSplitDepth, endSplitDepth, cascadeLightViewMatrix, cascadeLightOrthoProjectionMatrix };
}
} // namespace prev_test::component::shadow
