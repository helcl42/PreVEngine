#ifndef __PARTICLES_RENDERER_H__
#define __PARTICLES_RENDERER_H__

#include "../../IModel.h"
#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::particle {
class ParticlesRenderer final : public IRenderer<NormalRenderContext> {
public:
    ParticlesRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

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
        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;

        DEFAULT_ALIGNMENT uint32_t textureNumberOfRows;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 color;
    };

private:
    const uint32_t m_descriptorCount{ 20 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::particle

#endif // !__PARTICLES_RENDERER_H__
