#ifndef __WATER_RENDERER_H__
#define __WATER_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../component/shadow/ShadowsCommon.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::water {
class WaterRenderer final : public IRenderer<NormalRenderContext> {
public:
    WaterRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~WaterRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    struct ShadowsCascadeUniform {
        glm::mat4 viewProjectionMatrix;

        glm::vec4 split;
    };

    struct ShadowsUniform {
        ShadowsCascadeUniform cascades[prev_test::component::shadow::CASCADES_COUNT];

        uint32_t enabled;

        uint32_t useReverseDepth;
    };

    struct LightUniform {
        glm::vec4 position;

        glm::vec4 color;
    };

    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;

        DEFAULT_ALIGNMENT glm::mat4 viewMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::vec4 cameraPositions[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT float density;

        DEFAULT_ALIGNMENT float gradient;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT ShadowsUniform shadows;

        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 waterColor;

        DEFAULT_ALIGNMENT LightUniform light;

        DEFAULT_ALIGNMENT glm::vec4 nearFarClippingPlane;

        DEFAULT_ALIGNMENT float moveFactor;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    prev::render::pass::RenderPass& m_renderPass;

    prev::scene::IScene& m_scene;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev::render::pipeline::Pipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UniformRingBuffer<UniformsFS>> m_uniformsPoolFS;

    std::unique_ptr<prev::render::sampler::Sampler> m_colorSampler;

    std::unique_ptr<prev::render::sampler::Sampler> m_normalSampler;

    std::unique_ptr<prev::render::sampler::Sampler> m_depthSampler;
};
} // namespace prev_test::render::renderer::water

#endif // !__WATER_RENDERER_H__
