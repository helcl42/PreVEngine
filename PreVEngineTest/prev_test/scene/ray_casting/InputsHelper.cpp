#include "InputsHelper.h"

namespace prev_test::scene::ray_casting {
InputsHelper::InputsHelper()
    : SceneNode()
{
    m_inputFacade.SetMouseLocked(true);
    m_inputFacade.SetMouseCursorVisible(false);
}

void InputsHelper::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_L) {
            m_inputFacade.SetMouseLocked(!m_inputFacade.IsMouseLocked());
            m_inputFacade.SetMouseCursorVisible(!m_inputFacade.IsMouseCursorVisible());
        }
    }
}
} // namespace prev_test::scene::ray_casting