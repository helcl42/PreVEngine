#ifndef __SHADOWS_COMPONENT_H__
#define __SHADOWS_COMPONENT_H__

#include "IShadowsComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::shadow {
class ShadowsComponent : public IShadowsComponent {
public:
    ShadowsComponent(prev::core::device::Device& device, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::ImageBuffer>& depthBuffer, const std::vector<ShadowsCascadeRenderData>& cascadesRenderData);

    ~ShadowsComponent();

public:
    void Update(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& viewFurstum, const glm::mat4& viewMatrix) override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const ShadowsCascadeRenderData& GetCascadeRenderData(const uint32_t cascadeIndex) const override;

    const ShadowsCascadeFrameData& GetCascadeFrameData(const uint32_t cascadeIndex) const override;

    VkExtent2D GetExtent() const override;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetImageBuffer() const override;

private:
    std::vector<float> GenerateCascadeSplits(const float nearClippingPlane, const float farClippingPlane) const;

    void UpdateCascadeFrameData(const glm::vec3& lightDirection, const prev_test::render::ViewFrustum& cascadeViewFrustum, const glm::mat4& lightViewMatrix, ShadowsCascadeFrameData& inOutFrameData) const;

private:
    prev::core::device::Device& m_device;

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};

    std::vector<ShadowsCascadeRenderData> m_cascadesRenderData;

    std::vector<ShadowsCascadeFrameData> m_cascadesFrameData;

private:
    static const inline float CASCADES_SPLIT_LAMBDA{ 0.78f };
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_COMPONENT_H__
