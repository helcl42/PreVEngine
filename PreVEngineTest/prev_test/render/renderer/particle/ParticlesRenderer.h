#ifndef __PARTICLES_RENDERER_H__
#define __PARTICLES_RENDERER_H__

#include "../../IModel.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/sampler/Sampler.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::particle {
class ParticlesRenderer final : public IRenderer<NormalRenderContext> {
public:
    ParticlesRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~ParticlesRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

private:
    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::mat4 viewMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT uint32_t textureNumberOfRows;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 color;
    };

private:
    const uint32_t m_descriptorCount{ 20 };

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
};
} // namespace prev_test::render::renderer::particle

#endif // !__PARTICLES_RENDERER_H__
