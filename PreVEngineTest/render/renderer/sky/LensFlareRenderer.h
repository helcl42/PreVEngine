#ifndef __LENS_FLARE_RENDERER_H__
#define __LENS_FLARE_RENDERER_H__

#include "SkyEvents.h"

#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"
#include "../RenderContexts.h"

#include "../../../General.h"

#include <prev/event/EventHandler.h>
#include <prev/render/buffer/UniformBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::sky {
class LensFlareRenderer final : public IRenderer<NormalRenderContext> {
public:
    LensFlareRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~LensFlareRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const NormalRenderContext& renderContext) override;

    void PreRender(const NormalRenderContext& renderContext) override;

    void Render(const NormalRenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const NormalRenderContext& renderContext) override;

    void AfterRender(const NormalRenderContext& renderContext) override;

    void ShutDown() override;

public:
    void operator()(const prev_test::render::renderer::sky::SunVisibilityEvent& evt);

private:
    prev::event::EventHandler<LensFlareRenderer, prev_test::render::renderer::sky::SunVisibilityEvent> m_sunVisibilityEventHandler{ *this };

    float m_sunVisibilityFactor{ 0.0f };

private:
    struct DEFAULT_ALIGNMENT UniformsVS {
        DEFAULT_ALIGNMENT glm::vec4 translation;

        DEFAULT_ALIGNMENT glm::vec4 scale;
    };

    struct DEFAULT_ALIGNMENT UniformsFS {
        DEFAULT_ALIGNMENT glm::vec4 brightness;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsVS>> m_uniformsPoolVS;

    std::unique_ptr<prev::render::buffer::UniformBufferRing<UniformsFS>> m_uniformsPoolFS;
};
} // namespace prev_test::render::renderer::sky

#endif // !__LENS_FLARE_RENDERER_H__
