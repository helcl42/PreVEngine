#include "ShadowsComponent.h"

#include "../../General.h"
#include "../../common/intersection/AABB.h"

#include <prev/core/DeviceProvider.h>
#include <prev/core/memory/image/DepthImageBuffer.h>
#include <prev/scene/AllocatorProvider.h>
#include <prev/util/MathUtils.h>
#include <prev/util/VkUtils.h>

namespace prev_test::component::shadow {
ShadowsComponent::ShadowsComponent(const uint32_t cascadesCount)
    : m_cascadesCount(cascadesCount)
{
}

void ShadowsComponent::Init()
{
    InitRenderPass();
    InitCascades();
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

void ShadowsComponent::ShutDown()
{
    ShutDownCascades();
    ShutDownRenderPass();
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
    return { SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };
}

std::shared_ptr<prev::core::memory::image::IImageBuffer> ShadowsComponent::GetImageBuffer() const
{
    return m_depthBuffer;
}

void ShadowsComponent::InitRenderPass()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    std::vector<VkSubpassDependency> dependencies{ 2 };
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    m_renderPass = std::make_shared<prev::render::pass::RenderPass>(*device);
    m_renderPass->AddDepthAttachment(DEPTH_FORMAT);
    m_renderPass->AddSubpass({ 0 });
    m_renderPass->AddSubpassDependencies(dependencies);
    m_renderPass->Create();
}

void ShadowsComponent::ShutDownRenderPass()
{
    m_renderPass->Destroy();
}

void ShadowsComponent::InitCascades()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };
    auto allocator{ prev::scene::AllocatorProvider::Instance().GetAllocator() };

    m_depthBuffer = std::make_shared<prev::core::memory::image::DepthImageBuffer>(*allocator);
    m_depthBuffer->Create(prev::core::memory::image::ImageBufferCreateInfo{ GetExtent(), VK_IMAGE_TYPE_2D, DEPTH_FORMAT, VK_SAMPLE_COUNT_1_BIT, 0, false, false, VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_cascadesCount });
    m_depthBuffer->CreateSampler(1.0f, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, false);

    m_cascades.resize(m_cascadesCount);
    for (uint32_t i = 0; i < m_cascadesCount; i++) {
        auto& cascade{ m_cascades.at(i) };
        cascade.imageView = prev::util::VkUtils::CreateImageView(*device, m_depthBuffer->GetImage(), m_depthBuffer->GetFormat(), VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_depthBuffer->GetMipLevels(), VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
        cascade.frameBuffer = prev::util::VkUtils::CreateFrameBuffer(*device, *m_renderPass, { cascade.imageView }, GetExtent());
    }
}

void ShadowsComponent::ShutDownCascades()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

    for (uint32_t i = 0; i < m_cascadesCount; i++) {
        auto& cascade{ m_cascades.at(i) };
        cascade.Destroy(*device);
    }

    m_depthBuffer->Destroy();
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

std::vector<glm::vec3> ShadowsComponent::GenerateFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform, const float splitDistance, const float lastSplitDistance) const
{
    auto frustumCorners{ prev::util::MathUtil::GetFrustumCorners(inverseWorldToClipSpaceTransform) };

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
