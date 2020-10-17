#ifndef __SUN_H__
#define __SUN_H__

#include "../../General.h"
#include "../../component/sky/ISunComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/event/EventHandler.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::sky {
class Sun final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    Sun();

    ~Sun() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent)
    {
        m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
    }

private:
    std::shared_ptr<prev_test::component::sky::ISunComponent> m_sunComponent;

    glm::vec2 m_viewPortSize;

private:
    prev::event::EventHandler<Sun, prev::core::NewIterationEvent> m_newIterationHandler{ *this };
};
} // namespace prev_test::scene::sky

#endif // !__SUN_H__