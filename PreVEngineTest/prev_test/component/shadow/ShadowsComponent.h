#ifndef __SHADOWS_COMPONENT_H__
#define __SHADOWS_COMPONENT_H__

#include "IShadowsComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::shadow {
class ShadowsComponent : public IShadowsComponent {
public:
    ShadowsComponent(prev::core::device::Device& device, const uint32_t cascadesCount, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::ImageBuffer>& depthBuffer, const std::vector<ShadowsCascade>& cascades);

    ~ShadowsComponent();

public:
    void Update(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& viewFurstum, const glm::mat4& viewMatrix) override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const ShadowsCascade& GetCascade(const size_t cascadeIndex) const override;

    VkExtent2D GetExtent() const override;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const override;

private:
    std::vector<float> GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const;

    void UpdateCascade(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& cascadeViewFrustum, const glm::mat4& lightViewMatrix, ShadowsCascade& inOutCascade) const;

private:
    prev::core::device::Device& m_device;

    uint32_t m_cascadesCount{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};

    std::vector<ShadowsCascade> m_cascades;

private:
    static const inline float CASCADES_SPLIT_LAMBDA{ 0.78f };
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_COMPONENT_H__
