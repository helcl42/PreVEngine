#ifndef __TERRAIN_SHADOWS_RENDERER_H__
#define __TERRAIN_SHADOWS_RENDERER_H__

#include "../../../General.h"
#include "../../pipeline/IPipeline.h"
#include "../RenderContextUserData.h"

#include "../../../General.h"

#include <prev/render/IRenderer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::shadow {
class TerrainShadowsRenderer final : public prev::render::IRenderer<SceneNodeFlags, ShadowsRenderContextUserData> {
public:
    TerrainShadowsRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~TerrainShadowsRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override;

    void PreRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const ShadowsRenderContextUserData& shadowsRenderContext) override;

    void PostRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& shadowsRenderContext) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const ShadowsRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    struct Uniforms {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

private:
    const uint32_t m_descriptorCount{ 3000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<Uniforms> > m_uniformsPool;
};
} // namespace prev_test::render::renderer::shadow

#endif // !__TERRAIN_SHADOWS_RENDERER_H__
