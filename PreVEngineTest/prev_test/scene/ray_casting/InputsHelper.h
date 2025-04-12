#ifndef __INPUTS_HELPER_H__
#define __INPUTS_HELPER_H__

#include <prev/event/EventHandler.h>
#include <prev/input/InputFacade.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene::ray_casting {
class InputsHelper final : public prev::scene::graph::SceneNode {
public:
    InputsHelper();

    virtual ~InputsHelper() = default;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

private:
    prev::input::InputsFacade m_inputFacade;

    prev::event::EventHandler<InputsHelper, prev::input::keyboard::KeyEvent> m_keyboardEventsHandler{ *this };
};
} // namespace prev_test::scene::ray_casting

#endif // !__INPUTS_HELPER_H__
