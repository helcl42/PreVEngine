#include "InputFacade.h"

namespace prev::input {
bool InputsFacade::RegisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener)
{
    return m_keyboardInputComponent.RegisterKeyboardActionListener(listener);
}

bool InputsFacade::UnregisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener)
{
    return m_keyboardInputComponent.UnregisterKeyboardActionListener(listener);
}

bool InputsFacade::IsKeyboardActionListenerRegistered(keyboard::IKeyboardActionListener& listener) const
{
    return m_keyboardInputComponent.IsKeyboardActionListenerRegistered(listener);
}

bool InputsFacade::RegisterTextListener(keyboard::ITextListener& listener)
{
    return m_keyboardInputComponent.RegisterTextListener(listener);
}

bool InputsFacade::UnregisterTextListener(keyboard::ITextListener& listener)
{
    return m_keyboardInputComponent.UnregisterTextListener(listener);
}

bool InputsFacade::IsTextListenerRegistered(keyboard::ITextListener& listener) const
{
    return m_keyboardInputComponent.IsTextListenerRegistered(listener);
}

bool InputsFacade::RegisterMouseActionListener(mouse::IMouseActionListener& listener)
{
    return m_mouseInputComponent.RegisterMouseActionListener(listener);
}

bool InputsFacade::UnregisterMouseActionListener(mouse::IMouseActionListener& listener)
{
    return m_mouseInputComponent.UnregisterMouseActionListener(listener);
}

bool InputsFacade::IsMouseActionListenerRegistered(mouse::IMouseActionListener& listener) const
{
    return m_mouseInputComponent.IsMouseActionListenerRegistered(listener);
}

bool InputsFacade::RegisterMouseScrollListener(mouse::IMouseScrollListener& listener)
{
    return m_mouseInputComponent.RegisterMouseScrollListener(listener);
}

bool InputsFacade::UnregisterMouseScrollListener(mouse::IMouseScrollListener& listener)
{
    return m_mouseInputComponent.UnregisterMouseScrollListener(listener);
}

bool InputsFacade::IsMouseScrollListenerRegistered(mouse::IMouseScrollListener& listener) const
{
    return m_mouseInputComponent.IsMouseScrollListenerRegistered(listener);
}

bool InputsFacade::RegisterTouchActionListener(touch::ITouchActionListener& listener)
{
    return m_touchInuptComponent.RegisterTouchActionListener(listener);
}

bool InputsFacade::UnregisterTouchActionListener(touch::ITouchActionListener& listener)
{
    return m_touchInuptComponent.UnregisterTouchActionListener(listener);
}

bool InputsFacade::IsTouchActionListenerRegistered(touch::ITouchActionListener& listener) const
{
    return m_touchInuptComponent.IsTouchActionListenerRegistered(listener);
}

bool InputsFacade::IsKeyPressed(const keyboard::KeyCode keyCode) const
{
    return m_keyboardInputComponent.IsKeyPressed(keyCode);
}

std::set<keyboard::KeyCode> InputsFacade::GetPressedKeys() const
{
    return m_keyboardInputComponent.GetPressedKeys();
}

std::set<mouse::MouseButtonType> InputsFacade::GetPressedButtons() const
{
    return m_mouseInputComponent.GetPressedButtons();
}

glm::vec2 InputsFacade::GetMousePosition() const
{
    return m_mouseInputComponent.GetMousePosition();
}

bool InputsFacade::IsButtonPressed(const mouse::MouseButtonType button) const
{
    return m_mouseInputComponent.IsButtonPressed(button);
}

bool InputsFacade::IsMouseLocked() const
{
    return m_mouseInputComponent.IsLocked();
}

void InputsFacade::SetMouseLocked(bool locked)
{
    m_mouseInputComponent.SetLocked(locked);
}

std::map<uint8_t, touch::Touch> InputsFacade::GetTouches() const
{
    return m_touchInuptComponent.GetTouches();
}

bool InputsFacade::IsPointerTouched(const uint8_t pointerId) const
{
    return m_touchInuptComponent.IsPointerTouched(pointerId);
}

bool InputsFacade::IsMouseCursorVisible() const
{
    return m_mouseInputComponent.IsCursorVisible();
}

void InputsFacade::SetMouseCursorVisible(bool visible)
{
    m_mouseInputComponent.SetCursorVisible(visible);
}
} // namespace prev::input