#ifndef __LENS_FLARE_RENDERER_H__
#define __LENS_FLARE_RENDERER_H__

#include "../../General.h"
#include "../IRenderer.h"
#include "../pipeline/IPipeline.h"
#include "SkyEvents.h"

#include <prev/event/EventHandler.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::sky {
class LensFlareRenderer final : public IRenderer<NormalRenderContextUserData> {
public:
    LensFlareRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~LensFlareRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const NormalRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const NormalRenderContextUserData& renderContextUserData) override;

    void ShutDown() override;

public:
    void operator()(const prev_test::render::sky::SunVisibilityEvent& evt);

private:
    prev::event::EventHandler<LensFlareRenderer, prev_test::render::sky::SunVisibilityEvent> m_sunVisibilityEventHandler{ *this };

    float m_sunVisibilityFactor{ 0.0f };

private:
    struct alignas(16) UniformsVS
    {
        alignas(16) glm::vec4 translation;

        alignas(16) glm::vec4 scale;
    };

    struct alignas(16) UniformsFS
    {
        alignas(16) glm::vec4 brightness;
    };

private:
    const uint32_t m_descriptorCount{ 50 };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsVS> > m_uniformsPoolVS;

    std::unique_ptr<prev::core::memory::buffer::UBOPool<UniformsFS> > m_uniformsPoolFS;
};
} // namespace prev_test::render::sky

#endif // !__LENS_FLARE_RENDERER_H__
