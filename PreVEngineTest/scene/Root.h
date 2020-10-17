#ifndef __ROOT_H__
#define __ROOT_H__

#include "../General.h"
#include "../render/renderer/IRenderer.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/touch/TouchEvents.h>
#include <prev/render/Swapchain.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Root final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Root(const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::Swapchain>& swapchain);

    ~Root() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void Render(prev::render::RenderContext& renderContext) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

private:
    void AddNode();

    void RemoveNode();

private:
    prev::event::EventHandler<Root, prev::input::keyboard::KeyEvent> m_keyEventHnadler{ *this };

    prev::event::EventHandler<Root, prev::input::touch::TouchEvent> m_touchEventHnadler{ *this };

private:
    std::unique_ptr<prev_test::render::renderer::IRenderer<prev_test::render::DefaultRenderContextUserData> > m_masterRenderer;
};
} // namespace prev_test::scene

#endif // !__ROOT_H__
