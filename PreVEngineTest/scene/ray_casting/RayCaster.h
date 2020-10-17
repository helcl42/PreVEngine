#ifndef __RAY_CASTER_H__
#define __RAY_CASTER_H__

#include "../../General.h"
#include "../../component/ray_casting/IMouseRayCasterComponent.h"
#include "../../component/ray_casting/IRayCasterComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/event/EventHandler.h>
#include <prev/input/InputFacade.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::ray_casting {
class RayCaster final : public prev::scene::graph::SceneNode<SceneNodeFlags> {
public:
    RayCaster();

    ~RayCaster() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

private:
    void RemoveRayCastComponnet();

    void AddRayCastComponent(const bool mouseLocked);

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

    void operator()(const prev::input::mouse::MouseLockRequest& lockRequest);

private:
    const float RAY_LENGTH{ 200.0f };

    std::shared_ptr<prev_test::component::ray_casting::IRayCasterComponent> m_rayCasterComponent;

    std::shared_ptr<prev_test::component::ray_casting::IMouseRayCasterComponent> m_mouseRayCasterComponent;

    glm::vec2 m_viewPortSize;

    prev::input::InputsFacade m_inputFacade;

private:
    prev::event::EventHandler<RayCaster, prev::core::NewIterationEvent> m_newIterationHandler{ *this };

    prev::event::EventHandler<RayCaster, prev::input::mouse::MouseLockRequest> m_mouseLockHandler{ *this };
};

} // namespace prev_test::scene::ray_casting

#endif // !__RAY_CASTER_H__
