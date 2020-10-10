#ifndef __SHADOWS_COMPONENT_H__
#define __SHADOWS_COMPONENT_H__

#include "IShadowsComponent.h"

#include <prev/core/memory/image/IImageBuffer.h>

namespace prev_test::component::shadow {
class ShadowsComponent : public IShadowsComponent {
public:
    ShadowsComponent(const uint32_t cascadesCount);

    virtual ~ShadowsComponent() = default;

public:
    void Init() override;

    void Update(const glm::vec3& lightDirection, const float nearClippingPlane, const float farClippingPlane, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override;

    void ShutDown() override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const ShadowsCascade& GetCascade(const uint32_t cascadeIndex) const override;

    VkExtent2D GetExtent() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetImageBuffer() const override;

private:
    void InitRenderPass();

    void ShutDownRenderPass();

    void InitCascades();

    void ShutDownCascades();

    std::vector<float> GenerateCaascadeSplits(const float nearClippingPlane, const float farClippingPlane) const;

    std::vector<glm::vec3> GenerateFrustumCorners(const glm::mat4& inverseWorldToClipSpaceTransform, const float splitDistance, const float lastSplitDistance) const;

    glm::vec3 CalculateFrustumCenter(const std::vector<glm::vec3>& frustumCorners) const;

    float CalculateFrustumRadius(const std::vector<glm::vec3>& frustumCorners, const glm::vec3& frustumCenter) const;

    void UpdateCascade(const glm::vec3& lightDirection, const glm::mat4& inverseCameraTransform, const float nearClippingPlane, const float farClippingPlane, const float splitDistance, const float lastSplitDistance, ShadowsCascade& outCascade) const;

private:
    static const inline VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

    static const inline uint32_t SHADOW_MAP_DIMENSIONS{ 2048 };

    static const inline VkFilter SHADOW_MAP_FILTER{ VK_FILTER_NEAREST };

    static const inline float CASCADES_SPLIT_LAMBDA{ 0.86f };

private:
    const uint32_t m_cascadesCount;

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_depthBuffer;

    std::vector<ShadowsCascade> m_cascades;
};
} // namespace prev_test::component::shadow

#endif // !__SHADOWS_COMPONENT_H__
