#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

#include <array>

namespace prev_test::render::renderer::sky {
class SkyRenderer final : public IRenderer<NormalRenderContext> {
public:
    SkyRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

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
    enum SamplerType {
        LINEAR = 0,
        NEAREST = 1,
        LINEAR_REPEATED = 2,
        COUNT = 3,
    };

    std::array<std::shared_ptr<prev::render::sampler::Sampler>, SamplerType::COUNT> m_samplers;

    struct ImageBufferData {
        std::shared_ptr<prev::render::buffer::ImageBuffer> image;
        SamplerType samplerType;
    };

    ImageBufferData m_skyColorImageBuffer[MAX_VIEW_COUNT];

    ImageBufferData m_skyBloomImageBuffer[MAX_VIEW_COUNT];

    ImageBufferData m_skyAlphanessImageBuffer[MAX_VIEW_COUNT];

    ImageBufferData m_skyCloudDistanceImageBuffer[MAX_VIEW_COUNT];

    ImageBufferData m_skyPostProcessColorImageBuffer[MAX_VIEW_COUNT];

private:
    static const inline VkFormat COLOR_FORMAT{ VK_FORMAT_R8G8B8A8_UNORM };

    static const inline VkFormat DEPTH_FORMAT{ VK_FORMAT_R32_SFLOAT };

private:
    void UpdateImageBufferExtents(const VkExtent2D& extent, const VkFormat format, ImageBufferData& imageBuffer);

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

        DEFAULT_ALIGNMENT glm::vec4 worldOrigin;

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

        DEFAULT_ALIGNMENT float maxDepth;
    };

    struct DEFAULT_ALIGNMENT UniformsSkyPostProcessCS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;
        DEFAULT_ALIGNMENT glm::vec4 lisghtPosition;
        DEFAULT_ALIGNMENT uint32_t enableGodRays;
        float lightDotCameraForward;
    };

private:
    const uint32_t m_descriptorCount{ 24 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_skyShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_skyPipeline;

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsSkyCS>> m_uniformsPoolSkyCS;

    std::unique_ptr<prev::render::shader::Shader> m_skyPostProcessShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_skyPostProcessPipeline;

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsSkyPostProcessCS>> m_uniformsPoolSkyPostProcessCS;

    std::unique_ptr<prev::render::shader::Shader> m_compositeShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_compositePipeline;
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_RENDERER_H__
