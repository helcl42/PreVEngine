#ifndef __TEXTURE_DEBUG_RENDERER_H__
#define __TEXTURE_DEBUG_RENDERER_H__

#include "../../General.h"
#include "../IRenderer.h"
#include "../pipeline/IPipeline.h"

#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::debug {
class TextureDebugRenderer final : public prev_test::render::IRenderer<prev_test::render::DefaultRenderContextUserData> {
public:
    TextureDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~TextureDebugRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

private:
    std::unique_ptr<IModel> m_quadModel;
};
} // namespace prev_test::render::debug

#endif // !__TEXTURE_DEBUG_RENDERER_H__
