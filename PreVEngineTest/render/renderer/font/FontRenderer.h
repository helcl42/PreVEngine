#ifndef __FONT_RENDERER_H__
#define __FONT_RENDERER_H__

#include "../../pipeline/IPipeline.h"
#include "../RenderContextUserData.h"

#include "../../../General.h"

#include <prev/render/IRenderer.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::font {
class FontRenderer final : public prev::render::IRenderer<NormalRenderContextUserData> {
public:
    FontRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~FontRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::vec4 translation;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 color;

        DEFAULT_ALIGNMENT glm::vec4 width;

        DEFAULT_ALIGNMENT glm::vec4 edge;

        DEFAULT_ALIGNMENT glm::vec4 bias;

        DEFAULT_ALIGNMENT uint32_t hasEffect;

        DEFAULT_ALIGNMENT glm::vec4 borderWidth;

        DEFAULT_ALIGNMENT glm::vec4 borderEdge;

        DEFAULT_ALIGNMENT glm::vec4 outlineColor;

        DEFAULT_ALIGNMENT glm::vec4 outlineOffset;
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
};
} // namespace prev_test::render::renderer::font

#endif // !__FONT_RENDERER_H__
