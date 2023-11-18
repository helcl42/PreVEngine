#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class SkyRenderer final : public IRenderer<NormalRenderContext> {
public:
    SkyRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SkyRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    void UpdateImageBufferExtents(const VkExtent2D& extent, const VkFormat format, std::shared_ptr<prev::render::buffer::image::IImageBuffer>& imageBuffer, std::shared_ptr<prev::render::sampler::Sampler>& sampler);

    void AddImageBufferPipelineBarrierCommand(const VkImage image, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkPipelineStageFlags srcShaderStageMask, const VkPipelineStageFlags dstShaderStageMask, VkCommandBuffer commandBuffer);

private:
    struct DEFAULT_ALIGNMENT UniformsSkyCS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseViewMatrix;

        DEFAULT_ALIGNMENT glm::vec4 lightColor;
        DEFAULT_ALIGNMENT glm::vec4 lightDirection;
        DEFAULT_ALIGNMENT glm::vec4 cameraPosition;

        DEFAULT_ALIGNMENT glm::vec4 baseCloudColor;
        DEFAULT_ALIGNMENT glm::vec4 skyColorBottom;
        DEFAULT_ALIGNMENT glm::vec4 skyColorTop;
        DEFAULT_ALIGNMENT glm::vec4 windDirection;

        DEFAULT_ALIGNMENT float time;
        float coverageFactor;
        float cloudSpeed;
        float crispiness;

        DEFAULT_ALIGNMENT float absorption;
        float curliness;
        uint32_t enablePowder;
        float densityFactor;

        DEFAULT_ALIGNMENT float earthRadius;
        float sphereInnerRadius;
        float sphereOuterRadius;
        float cloudTopOffset;
    };

    struct DEFAULT_ALIGNMENT UniformsSkyPostProcessCS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;
        DEFAULT_ALIGNMENT glm::vec4 lisghtPosition;
        DEFAULT_ALIGNMENT uint32_t enableGodRays;
        float lightDotCameraForward;
    };

private:
    const uint32_t m_descriptorCount{ 10 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_skyShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_skyPipeline;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsSkyCS>> m_uniformsPoolSkyCS;

    std::unique_ptr<prev::render::shader::Shader> m_skyPostProcessShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_skyPostProcessPipeline;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsSkyPostProcessCS>> m_uniformsPoolSkyPostProcessCS;

    std::unique_ptr<prev::render::shader::Shader> m_compositeShader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_compositePipeline;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_skyColorImageBuffer;

    std::shared_ptr<prev::render::sampler::Sampler> m_skyColorImageSampler;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_skyBloomImageBuffer;

    std::shared_ptr<prev::render::sampler::Sampler> m_skyBloomImageSampler;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_skyAlphanessImageBuffer;

    std::shared_ptr<prev::render::sampler::Sampler> m_skyAlphanessImageSampler;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_skyCloudDistanceImageBuffer;

    std::shared_ptr<prev::render::sampler::Sampler> m_skyCloudDistanceImageSampler;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_skyPostProcessColorImageBuffer;

    std::shared_ptr<prev::render::sampler::Sampler> m_skyPostProcessImageSampler;

private:
    static const inline VkFormat COLOR_FORMAT{ VK_FORMAT_R8G8B8A8_UNORM };

    static const inline VkFormat DEPTH_FORMAT{ VK_FORMAT_R32_SFLOAT };
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_RENDERER_H__
