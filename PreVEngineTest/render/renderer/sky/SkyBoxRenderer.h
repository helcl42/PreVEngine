#ifndef __SKY_BOX_RENDEreR_H__
#define __SKY_BOX_RENDEreR_H__

#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class SkyBoxRenderer final : public IRenderer<NormalRenderContext> {
public:
    SkyBoxRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

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

        DEFAULT_ALIGNMENT glm::mat4 viewMatrix;

        DEFAULT_ALIGNMENT glm::mat4 projectionMatrix;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 fogColor;

        DEFAULT_ALIGNMENT glm::vec4 lowerLimit;

        DEFAULT_ALIGNMENT glm::vec4 upperLimit;
    };

private:
    const uint32_t m_descriptorCount{ 10 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::sky

#endif // !__SKY_BOX_RENDEreR_H__
