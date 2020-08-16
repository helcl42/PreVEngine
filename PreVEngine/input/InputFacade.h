#ifndef __INPUTS_H__
#define __INPUTS_H__

#include "keyboard/KeyboardInputComponnet.h"
#include "mouse/MouseInputComponent.h"
#include "touch/TouchInputComponent.h"

namespace prev::input {
class InputsFacade {
public:
    InputsFacade() = default;

    ~InputsFacade() = default;

    InputsFacade(const InputsFacade& other) = delete;

    InputsFacade& operator=(const InputsFacade& other) = delete;

    InputsFacade(InputsFacade&& other) = delete;

    InputsFacade& operator=(InputsFacade&& other) = delete;

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

private:
    keyboard::KeyboardInputComponnet m_keyboardInputComponent;

    mouse::MouseInputComponent m_mouseInputComponent;

    touch::TouchInputComponent m_touchInuptComponent;
};

} // namespace prev::input

#endif // !__INPUTS_H__
