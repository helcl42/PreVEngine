#ifndef __SKY_BOX_RENDERER_H__
#define __SKY_BOX_RENDERER_H__

#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/pipeline/Pipeline.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/IScene.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class SkyBoxRenderer final : public IRenderer<NormalRenderContext> {
public:
    SkyBoxRenderer(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, prev::scene::IScene& scene);

    ~SkyBoxRenderer() = default;

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
        DEFAULT_ALIGNMENT glm::mat4 modelMatrix;

        DEFAULT_ALIGNMENT glm::mat4 viewMatrices[MAX_VIEW_COUNT];

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrices[MAX_VIEW_COUNT];
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 lowerLimit;

        DEFAULT_ALIGNMENT glm::vec4 upperLimit;
    };

private:
    const uint32_t m_descriptorCount{ 10 };

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
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_BOX_RENDERER_H__
