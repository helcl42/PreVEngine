#ifndef __SHADOW_MAP_DEBUG_RENDERER_H__
#define __SHADOW_MAP_DEBUG_RENDERER_H__

#include "../../General.h"
#include "../IRenderer.h"
#include "../pipeline/IPipeline.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/shader/Shader.h>
#include <prev/scene/graph/ISceneNode.h>

#include <memory>

namespace prev_test::render::debug {
class ShadowMapDebugRenderer final : public prev_test::render::IRenderer<prev_test::render::DefaultRenderContextUserData> {
public:
    ShadowMapDebugRenderer(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass);

    ~ShadowMapDebugRenderer() = default;

public:
    void Init() override;

    void BeforeRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void PreRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void Render(const prev::render::RenderContext& renderContext, const std::shared_ptr<prev::scene::graph::ISceneNode<SceneNodeFlags> >& node, const DefaultRenderContextUserData& renderContextUserData) override;

    void PostRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

    void AfterRender(const prev::render::RenderContext& renderContext, const DefaultRenderContextUserData& renderContextUserData) override;

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
} // namespace prev_test::render::debug

#endif
