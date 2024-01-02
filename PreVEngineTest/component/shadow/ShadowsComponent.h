#ifndef __SHADOWS_COMPONENT_H__
#define __SHADOWS_COMPONENT_H__

#include "IShadowsComponent.h"

#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

namespace prev_test::component::shadow {
class ShadowsComponent : public IShadowsComponent {
public:
    ShadowsComponent(const uint32_t cascadesCount, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::image::IImageBuffer>& depthBuffer, const std::shared_ptr<prev::render::sampler::Sampler>& sampler, const std::vector<ShadowsCascade>& cascades);

    ~ShadowsComponent();

public:
    void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const ShadowsCascade& GetCascade(const size_t cascadeIndex) const override;

    VkExtent2D GetExtent() const override;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetImageBuffer() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetSampler() const override;

private:
    std::vector<glm::vec3> GenerateFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform, const float nearSplitDistance, const float farSplitDistance) const;

    glm::vec3 CalculateFrustumCenter(const std::vector<glm::vec3>& frustumCorners) const;

    float CalculateFrustumRadius(const std::vector<glm::vec3>& frustumCorners, const glm::vec3& frustumCenter) const;

    std::vector<float> GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const;

    void UpdateCascade(const glm::vec3& lightDirection, const glm::mat4& inverseCameraTransform, const float nearClippingPlane, const float farClippingPlane, const float nearSplitDistance, const float farSplitDistance, ShadowsCascade& outCascade) const;

private:
    const uint32_t m_cascadesCount{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_depthBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_sampler{};

    std::vector<ShadowsCascade> m_cascades;

private:
    static const inline float CASCADES_SPLIT_LAMBDA{ 0.78f };
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_COMPONENT_H__
