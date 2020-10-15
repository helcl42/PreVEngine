#ifndef __PARTICLES_RENDERER_H__
#define __PARTICLES_RENDERER_H__

#include "../../../General.h"
#include "../../IModel.h"
#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"

#include <prev/core/memory/buffer/VertexBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::renderer::particle {
class ParticlesRenderer final : public IRenderer<NormalRenderContextUserData> {
public:
    ParticlesRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~ParticlesRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

private:
    std::unique_ptr<prev_test::render::IModel> CreateModel(prev::core::memory::Allocator& allocator, const std::shared_ptr<IMesh>& mesh) const;

private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::mat4 viewMatrix;

        alignas(16) glm::mat4 projectionMatrix;

        alignas(16) uint32_t textureNumberOfRows;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 color;
    };

private:
    const uint32_t m_descriptorCount{ 2000 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsFS> > m_uniformsPoolFS;

    std::unique_ptr<prev::core::memory::buffer::VertexBuffer> m_instanceDataBuffer;
};
} // namespace prev_test::render::renderer::particle

#endif // !__PARTICLES_RENDERER_H__
