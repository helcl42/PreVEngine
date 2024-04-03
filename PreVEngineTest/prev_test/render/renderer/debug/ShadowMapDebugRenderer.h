#ifndef __SHADOW_MAP_DEBUG_RENDERER_H__
#define __SHADOW_MAP_DEBUG_RENDERER_H__

#include "../../../General.h"
#include "../../IModel.h"
#include "../../pipeline/IPipeline.h"
#include "../IRenderer.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

namespace prev_test::render::renderer::debug {
class ShadowMapDebugRenderer final : public IRenderer<prev::render::RenderContext> {
public:
    ShadowMapDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~ShadowMapDebugRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext) override;

    void PreRender(const prev::render::RenderContext& renderContext) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode>& node) override;

    void PostRender(const prev::render::RenderContext& renderContext) override;

    void AfterRender(const prev::render::RenderContext& renderContext) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

private:
    struct PushConstantBlock {
        uint32_t imageIndex;
        float nearClippingPlane;
        float farClippingPlane;
    };

private:
    const uint32_t m_descriptorCount{ 1000 };

private:
    prev::event::EventHandler<ShadowMapDebugRenderer, prev::input::keyboard::KeyEvent> m_keyEvent{ *this };

private:
    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

private:
    std::unique_ptr<prev::render::shader::Shader> m_shader;

    std::unique_ptr<prev_test::render::pipeline::IPipeline> m_pipeline;

private:
    std::unique_ptr<IModel> m_quadModel;

    int32_t m_cascadeIndex{ 0 };
};
} // namespace prev_test::render::renderer::debug

#endif
