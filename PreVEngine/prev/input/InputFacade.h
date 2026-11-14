#ifndef __INPUT_FACADE_H__
#define __INPUT_FACADE_H__

#include "gamepad/GamepadInputComponent.h"
#include "keyboard/KeyboardInputComponent.h"
#include "mouse/MouseInputComponent.h"
#include "touch/TouchInputComponent.h"

namespace prev::input {
class InputFacade {
public:
    InputFacade() = default;

    ~InputFacade() = default;

    InputFacade(const InputFacade& other) = delete;

    InputFacade& operator=(const InputFacade& other) = delete;

    InputFacade(InputFacade&& other) = delete;

    InputFacade& operator=(InputFacade&& other) = delete;

public:
    bool RegisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener);

    bool UnregisterKeyboardActionListener(keyboard::IKeyboardActionListener& listener);

    bool IsKeyboardActionListenerRegistered(keyboard::IKeyboardActionListener& listener) const;

    bool RegisterTextListener(keyboard::ITextListener& listener);

    bool UnregisterTextListener(keyboard::ITextListener& listener);

    bool IsTextListenerRegistered(keyboard::ITextListener& listener) const;

    bool RegisterMouseActionListener(mouse::IMouseActionListener& listener);

    bool UnregisterMouseActionListener(mouse::IMouseActionListener& listener);

    bool IsMouseActionListenerRegistered(mouse::IMouseActionListener& listener) const;

    bool RegisterMouseScrollListener(mouse::IMouseScrollListener& listener);

    bool UnregisterMouseScrollListener(mouse::IMouseScrollListener& listener);

    bool IsMouseScrollListenerRegistered(mouse::IMouseScrollListener& listener) const;

    bool RegisterTouchActionListener(touch::ITouchActionListener& listener);

    bool UnregisterTouchActionListener(touch::ITouchActionListener& listener);

    bool IsTouchActionListenerRegistered(touch::ITouchActionListener& listener) const;

    bool RegisterGamepadButtonListener(gamepad::IGamepadButtonListener& listener);

    bool UnregisterGamepadButtonListener(gamepad::IGamepadButtonListener& listener);

    bool IsGamepadButtonListenerRegistered(gamepad::IGamepadButtonListener& listener) const;

    bool RegisterGamepadConnectionListener(gamepad::IGamepadConnectionListener& listener);

    bool UnregisterGamepadConnectionListener(gamepad::IGamepadConnectionListener& listener);

    bool IsGamepadConnectionListenerRegistered(gamepad::IGamepadConnectionListener& listener) const;

public:
    bool IsKeyPressed(const keyboard::KeyCode keyCode) const;

    std::set<keyboard::KeyCode> GetPressedKeys() const;

    std::set<mouse::MouseButtonType> GetPressedButtons() const;

    glm::vec2 GetMousePosition() const;

    bool IsButtonPressed(const mouse::MouseButtonType button) const;

    bool IsMouseLocked() const;

    void SetMouseLocked(bool locked);

    std::map<uint8_t, touch::Touch> GetTouches() const;

    bool IsPointerTouched(const uint8_t pointerId) const;

    bool IsMouseCursorVisible() const;

    void SetMouseCursorVisible(bool visible);

    std::vector<uint32_t> GetConnectedGamepads() const;

    bool IsGamepadConnected(const uint32_t gamepadId) const;

    bool IsGamepadButtonPressed(const uint32_t gamepadId, const gamepad::GamepadButton button) const;

    float GetGamepadAxis(const uint32_t gamepadId, const gamepad::GamepadAxis axis) const;

    glm::vec2 GetGamepadStick(const uint32_t gamepadId, const gamepad::GamepadStick stick) const;

    gamepad::GamepadState GetGamepadState(const uint32_t gamepadId) const;

    std::string GetGamepadName(const uint32_t gamepadId) const;

private:
    keyboard::KeyboardInputComponent m_keyboardInputComponent;

    mouse::MouseInputComponent m_mouseInputComponent;

    touch::TouchInputComponent m_touchInputComponent;

    gamepad::GamepadInputComponent m_gamepadInputComponent;
};

} // namespace prev::input

#endif // !__INPUT_FACADE_H__
