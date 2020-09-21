#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "../../General.h"
#include "../IRenderer.h"
#include "../pipeline/IPipeline.h"

#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::sky {
class SkyRenderer final : public IRenderer<NormalRenderContextUserData> {
public:
    SkyRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SkyRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    void UpdateImageBufferExtents(const VkExtent2D& extent, std::shared_ptr<prev::core::memory::image::IImageBuffer>& imageBuffer);

    void AddInterComputeImageBufferBarrier(const VkImage image, VkCommandBuffer commandBuffer);

    void AddImageBufferBarrier(const VkImage image, VkCommandBuffer commandBuffer);

private:
    struct alignas(16) UniformsSkyCS
    {
        alignas(16) glm::vec4 resolution;

        alignas(16) glm::mat4 inverseProjectionMatrix;
        alignas(16) glm::mat4 inverseViewMatrix;

        alignas(16) glm::vec4 lightColor;
        alignas(16) glm::vec4 lightDirection;
        alignas(16) glm::vec4 cameraPosition;

        alignas(16) glm::vec4 baseCloudColor;
        alignas(16) glm::vec4 skyColorBottom;
        alignas(16) glm::vec4 skyColorTop;
        alignas(16) glm::vec4 windDirection;

        alignas(16) float time;
        float coverageFactor;
        float cloudSpeed;
        float crispiness;

        alignas(16) float absorption;
        float curliness;
        uint32_t enablePowder;
        float densityFactor;

        alignas(16) float earthRadius;
        float sphereInnerRadius;
        float sphereOuterRadius;
        float cloudTopOffset;
    };

    struct alignas(16) UniformsSkyPostProcessCS
    {
        alignas(16) glm::vec4 resolution;
        alignas(16) glm::vec4 lisghtPosition;
        alignas(16) uint32_t enableGodRays;
        float lightDotCameraFront;
    };

private:
    const uint32_t m_descriptorCount{ 10 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_conmputeSkyShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_computeSkyPipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsSkyCS> > m_uniformsPoolSkyCS;

    std::unique_ptr<prev::render::shader::Shader> m_conmputeSkyPostProcessShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_computeSkyPostProcessPipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsSkyPostProcessCS> > m_uniformsPoolSkyPorstProcessCS;

    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_skyColorImageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_skyBloomImageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_skyAlphanessImageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_skyCloudDistanceImageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_skyPostProcessColorImageBuffer;
};
} // namespace prev_test::render::sky

#endif // !__SKY_RENDERER_H__
