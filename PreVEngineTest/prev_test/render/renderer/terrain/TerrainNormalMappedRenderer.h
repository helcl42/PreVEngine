#ifndef __TERRAIN_NORMAL_MAPPED_RENDERER_H__
#define __TERRAIN_NORMAL_MAPPED_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../component/light/LightCommon.h"
#include "../../../component/shadow/ShadowsCommon.h"

#include <prev/render/buffer/BufferPool.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/GraphicsPipelineBuilder.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::terrain {
class TerrainNormalMappedRenderer final : public IRenderer<NormalRenderContext> {
public:
    TerrainNormalMappedRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~TerrainNormalMappedRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    struct DEFAULT_ALIGNMENT ShadowsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;

        ShadowsCascadeUniform() = default;

        ShadowsCascadeUniform(const glm::mat4& vpMat, const glm::vec4& spl)
            : viewProjectionMatrix(vpMat)
            , split(spl)
        {
        }
    };

    struct DEFAULT_ALIGNMENT ShadowsUniform {
        ShadowsCascadeUniform cascades[prev_test::component::shadow::CASCADES_COUNT];

        uint32_t enabled;

        uint32_t useReverseDepth;
    };

    struct DEFAULT_ALIGNMENT LightUniform {
        glm::vec4 position;

        glm::vec4 color;

        glm::vec4 attenuation;

        LightUniform() = default;

        LightUniform(const glm::vec4& pos, const glm::vec4& col, const glm::vec4& atten)
            : position(pos)
            , color(col)
            , attenuation(atten)
        {
        }
    };

    struct DEFAULT_ALIGNMENT LightningUniform {
        LightUniform lights[prev_test::component::light::MAX_LIGHT_COUNT];

        uint32_t realCountOfLights;

        float ambientFactor;
    };

    struct DEFAULT_ALIGNMENT MaterialUniform {
        glm::vec4 color;

        float shineDamper;

        float reflectivity;

        MaterialUniform() = default;

        MaterialUniform(const glm::vec4& col, const float shineDaperr, const float reflect)
            : color(col)
            , shineDamper(shineDaperr)
            , reflectivity(reflect)
        {
        }
    };

    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;

        DEFAULT_ALIGNMENT glm::mat4 normalMatrix;

        DEFAULT_ALIGNMENT glm::mat4 viewMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::vec4 cameraPositions[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::vec4 clipPlane;

        DEFAULT_ALIGNMENT LightningUniform lightning;

        DEFAULT_ALIGNMENT float density;
        float gradient;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT ShadowsUniform shadows;

        DEFAULT_ALIGNMENT LightningUniform lightning;

        DEFAULT_ALIGNMENT MaterialUniform material[4];

        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 selectedColor;

        DEFAULT_ALIGNMENT uint32_t selected;
        uint32_t castedByShadows;
        float minHeight;
        float maxHeight;

        DEFAULT_ALIGNMENT glm::vec4 heightSteps[4];
        DEFAULT_ALIGNMENT float heightTransitionRange;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::BufferPool> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::BufferPool> m_uniformsPoolFS;

    std::unique_ptr<prev::render::sampler::Sampler> m_colorSampler;

    std::unique_ptr<prev::render::sampler::Sampler> m_normalSampler;

    std::unique_ptr<prev::render::sampler::Sampler> m_depthSampler;
};
} // namespace prev_test::render::renderer::terrain

#endif