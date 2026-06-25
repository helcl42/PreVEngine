#ifndef __SKY_RENDERER_H__
#define __SKY_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include <prev/render/buffer/FrameScopedBufferPool.h>
#include <prev/render/buffer/ImageBuffer.h>
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
    SkyRenderer(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~SkyRenderer() = default;

public:
    void Init() override;

    void BeginFrame(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void EndFrame(const NormalRenderContext& renderContext) override;

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

    uint32_t m_frameCounter{ 0 };

private:
    static const inline GfxFormat COLOR_FORMAT{ GFX_FORMAT_R8G8B8A8_UNORM };

    static const inline GfxFormat DEPTH_FORMAT{ GFX_FORMAT_R32_FLOAT };

private:
    void UpdateImageBufferExtents(GfxCommandEncoder commandEncoder, const GfxExtent2D& extent, const GfxFormat format, ImageBufferData& imageBuffer, GfxTextureUsageFlags usageFlags);
    void CopyImageBuffer(const ImageBufferData& src, const ImageBufferData& dst, GfxCommandEncoder commandEncoder);

private:
    struct DEFAULT_ALIGNMENT UniformsSkyCS {
        // View & Matrices
        DEFAULT_ALIGNMENT glm::vec4 resolution;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseViewMatrix;

        // Light & Color
        DEFAULT_ALIGNMENT glm::vec4 lightColor;
        DEFAULT_ALIGNMENT glm::vec4 lightDirection;
        DEFAULT_ALIGNMENT glm::vec4 baseCloudColor;
        DEFAULT_ALIGNMENT glm::vec4 skyColorBottom;
        DEFAULT_ALIGNMENT glm::vec4 skyColorTop;

        // Camera & World
        DEFAULT_ALIGNMENT glm::vec4 cameraPosition;
        DEFAULT_ALIGNMENT glm::vec4 worldOrigin;
        DEFAULT_ALIGNMENT glm::vec4 windDirection;
        DEFAULT_ALIGNMENT float earthRadius;
        float sphereInnerRadius;
        float sphereOuterRadius;
        float cloudTopOffset;

        // Cloud Shape
        DEFAULT_ALIGNMENT float coverageFactor;
        float crispiness;
        float curliness;
        float densityFactor;

        DEFAULT_ALIGNMENT float cloudSpeed;
        float absorption;
        float maxDepth;
        float time;

        // Rendering Quality
        DEFAULT_ALIGNMENT uint32_t cloudSteps;
        uint32_t lightSteps;
        uint32_t useIGN;
        uint32_t frameCounter;

        // Effects
        DEFAULT_ALIGNMENT uint32_t enablePowder;
        float powderWeight;
        float lodScale;
        float ambientScale;

        // Reprojection
        DEFAULT_ALIGNMENT glm::mat4 prevViewProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 currentViewProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 prevInverseViewProjectionMatrix;
        DEFAULT_ALIGNMENT uint32_t enableCheckerboard;
        float reprojectionBlend;
        uint32_t enableTemporalBlend;
        uint32_t enableFullReproject;
    };

    struct DEFAULT_ALIGNMENT UniformsSkyPostProcessCS {
        DEFAULT_ALIGNMENT glm::vec4 resolution;
        DEFAULT_ALIGNMENT glm::vec4 lisghtPosition;
        DEFAULT_ALIGNMENT glm::mat4 inverseProjectionMatrix;
        DEFAULT_ALIGNMENT glm::mat4 inverseViewMatrix;
        DEFAULT_ALIGNMENT glm::vec4 lightDirection;
        DEFAULT_ALIGNMENT glm::vec4 skyColorBottom;
        DEFAULT_ALIGNMENT glm::vec4 skyColorTop;
        DEFAULT_ALIGNMENT uint32_t enableGodRays;
        uint32_t enableBlur;
        float lightDotCameraForward;
        uint32_t godRaySamples;
        DEFAULT_ALIGNMENT uint32_t enableCheckerboardResolve;
        uint32_t frameCounter;
        float pad0;
        float pad1;
    };

private:
    const uint32_t m_descriptorCount{ 16 };

private:
    prev::core::device::Device& m_device;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_skyShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_skyPipeline;

    std::unique_ptr<prev::render::buffer::FrameScopedBufferPool> m_uniformsPoolSkyCS;

    std::unique_ptr<prev::render::shader::Shader> m_skyPostProcessShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_skyPostProcessPipeline;

    std::unique_ptr<prev::render::buffer::FrameScopedBufferPool> m_uniformsPoolSkyPostProcessCS;

    std::unique_ptr<prev::render::shader::Shader> m_compositeShader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_compositePipeline;

    // Reprojection history
    ImageBufferData m_skyHistoryColorImageBuffer[MAX_VIEW_COUNT];
    ImageBufferData m_skyHistoryDepthImageBuffer[MAX_VIEW_COUNT];
    glm::mat4 m_prevViewProjectionMatrix[MAX_VIEW_COUNT]{};
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_RENDERER_H__
