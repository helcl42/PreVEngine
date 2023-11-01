#ifndef __SUN_RENDERER_H__
#define __SUN_RENDERER_H__

#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class SunRenderer final : public IRenderer<NormalRenderContext> {
public:
    SunRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~SunRenderer() = default;

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
        DEFAULT_ALIGNMENT glm::vec4 translation;

        DEFAULT_ALIGNMENT glm::vec4 scale;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UBOPool<UniformsVS>> m_uniformsPoolVS;

private:
    uint64_t m_passedSamples{ 0 };

    uint64_t m_maxNumberOfSamples{ 0 };

    VkQueryPool m_queryPool{ nullptr };

    bool m_firstFrame{ true };
};
} // namespace prev_test::render::renderer::sky

#endif // !__SUN_RENDERER_H__
