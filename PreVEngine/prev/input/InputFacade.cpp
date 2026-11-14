#include "InputFacade.h"

namespace prev::input {
bool InputFacade::RegisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener)
{
    return m_keyboardInputComponent.RegisterKeyboardActionListener(listener);
}

bool InputFacade::UnregisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener)
{
    return m_keyboardInputComponent.UnregisterKeyboardActionListener(listener);
}

bool InputFacade::IsKeyboardActionListenerRegistered(keyboard::IKeyboardActionListener& listener) const
{
    return m_keyboardInputComponent.IsKeyboardActionListenerRegistered(listener);
}

bool InputFacade::RegisterTextListener(keyboard::ITextListener& listener)
{
    return m_keyboardInputComponent.RegisterTextListener(listener);
}

bool InputFacade::UnregisterTextListener(keyboard::ITextListener& listener)
{
    return m_keyboardInputComponent.UnregisterTextListener(listener);
}

bool InputFacade::IsTextListenerRegistered(keyboard::ITextListener& listener) const
{
    return m_keyboardInputComponent.IsTextListenerRegistered(listener);
}

bool InputFacade::RegisterMouseActionListener(mouse::IMouseActionListener& listener)
{
    return m_mouseInputComponent.RegisterMouseActionListener(listener);
}

bool InputFacade::UnregisterMouseActionListener(mouse::IMouseActionListener& listener)
{
    return m_mouseInputComponent.UnregisterMouseActionListener(listener);
}

bool InputFacade::IsMouseActionListenerRegistered(mouse::IMouseActionListener& listener) const
{
    return m_mouseInputComponent.IsMouseActionListenerRegistered(listener);
}

bool InputFacade::RegisterMouseScrollListener(mouse::IMouseScrollListener& listener)
{
    return m_mouseInputComponent.RegisterMouseScrollListener(listener);
}

bool InputFacade::UnregisterMouseScrollListener(mouse::IMouseScrollListener& listener)
{
    return m_mouseInputComponent.UnregisterMouseScrollListener(listener);
}

bool InputFacade::IsMouseScrollListenerRegistered(mouse::IMouseScrollListener& listener) const
{
    return m_mouseInputComponent.IsMouseScrollListenerRegistered(listener);
}

bool InputFacade::RegisterTouchActionListener(touch::ITouchActionListener& listener)
{
    return m_touchInputComponent.RegisterTouchActionListener(listener);
}

bool InputFacade::UnregisterTouchActionListener(touch::ITouchActionListener& listener)
{
    return m_touchInputComponent.UnregisterTouchActionListener(listener);
}

bool InputFacade::IsTouchActionListenerRegistered(touch::ITouchActionListener& listener) const
{
    return m_touchInputComponent.IsTouchActionListenerRegistered(listener);
}

bool InputFacade::RegisterGamepadButtonListener(gamepad::IGamepadButtonListener& listener)
{
    return m_gamepadInputComponent.RegisterButtonListener(listener);
}

bool InputFacade::UnregisterGamepadButtonListener(gamepad::IGamepadButtonListener& listener)
{
    return m_gamepadInputComponent.UnregisterButtonListener(listener);
}

bool InputFacade::IsGamepadButtonListenerRegistered(gamepad::IGamepadButtonListener& listener) const
{
    return m_gamepadInputComponent.IsButtonListenerRegistered(listener);
}

bool InputFacade::RegisterGamepadConnectionListener(gamepad::IGamepadConnectionListener& listener)
{
    return m_gamepadInputComponent.RegisterConnectionListener(listener);
}

bool InputFacade::UnregisterGamepadConnectionListener(gamepad::IGamepadConnectionListener& listener)
{
    return m_gamepadInputComponent.UnregisterConnectionListener(listener);
}

bool InputFacade::IsGamepadConnectionListenerRegistered(gamepad::IGamepadConnectionListener& listener) const
{
    return m_gamepadInputComponent.IsConnectionListenerRegistered(listener);
}

bool InputFacade::IsKeyPressed(const keyboard::KeyCode keyCode) const
{
    return m_keyboardInputComponent.IsKeyPressed(keyCode);
}

std::set<keyboard::KeyCode> InputFacade::GetPressedKeys() const
{
    return m_keyboardInputComponent.GetPressedKeys();
}

std::set<mouse::MouseButtonType> InputFacade::GetPressedButtons() const
{
    return m_mouseInputComponent.GetPressedButtons();
}

glm::vec2 InputFacade::GetMousePosition() const
{
    return m_mouseInputComponent.GetMousePosition();
}

bool InputFacade::IsButtonPressed(const mouse::MouseButtonType button) const
{
    return m_mouseInputComponent.IsButtonPressed(button);
}

bool InputFacade::IsMouseLocked() const
{
    return m_mouseInputComponent.IsLocked();
}

void InputFacade::SetMouseLocked(bool locked)
{
    m_mouseInputComponent.SetLocked(locked);
}

std::map<uint8_t, touch::Touch> InputFacade::GetTouches() const
{
    return m_touchInputComponent.GetTouches();
}

bool InputFacade::IsPointerTouched(const uint8_t pointerId) const
{
    return m_touchInputComponent.IsPointerTouched(pointerId);
}

bool InputFacade::IsMouseCursorVisible() const
{
    return m_mouseInputComponent.IsCursorVisible();
}

void InputFacade::SetMouseCursorVisible(bool visible)
{
    m_mouseInputComponent.SetCursorVisible(visible);
}

std::vector<uint32_t> InputFacade::GetConnectedGamepads() const
{
    return m_gamepadInputComponent.GetConnectedGamepads();
}

bool InputFacade::IsGamepadConnected(const uint32_t gamepadId) const
{
    return m_gamepadInputComponent.IsConnected(gamepadId);
}

bool InputFacade::IsGamepadButtonPressed(const uint32_t gamepadId, const gamepad::GamepadButton button) const
{
    return m_gamepadInputComponent.IsButtonPressed(gamepadId, button);
}

float InputFacade::GetGamepadAxis(const uint32_t gamepadId, const gamepad::GamepadAxis axis) const
{
    return m_gamepadInputComponent.GetAxis(gamepadId, axis);
}

glm::vec2 InputFacade::GetGamepadStick(const uint32_t gamepadId, const gamepad::GamepadStick stick) const
{
    return m_gamepadInputComponent.GetStick(gamepadId, stick);
}

gamepad::GamepadState InputFacade::GetGamepadState(const uint32_t gamepadId) const
{
    return m_gamepadInputComponent.GetState(gamepadId);
}

std::string InputFacade::GetGamepadName(const uint32_t gamepadId) const
{
    return m_gamepadInputComponent.GetName(gamepadId);
}
} // namespace prev::input