#ifndef __SELECTION_DEBUG_RENDERER_H__
#define __SELECTION_DEBUG_RENDERER_H__

#ifdef RENDER_SELECTION

#include "../../IMesh.h"
#include "../../IModel.h"
#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::debug {
class SelectionDebugRenderer final : public IRenderer<NormalRenderContext> {
public:
    SelectionDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SelectionDebugRenderer() = default;

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
        DEFAULT_ALIGNMENT glm::vec4 color;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsFS>> m_uniformsPoolFS;

    std::unique_ptr<IModel> m_selectionPointModel;
};
} // namespace prev_test::render::renderer::debug

#endif

#endif // !__SELECTION_DEBUG_RENDERER_H__
