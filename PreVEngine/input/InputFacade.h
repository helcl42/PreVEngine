#ifndef __INPUTS_H__
#define __INPUTS_H__

#include "keyboard/KeyboardInputComponnet.h"
#include "mouse/MouseInputComponent.h"
#include "touch/TouchInputComponent.h"

namespace PreVEngine {
class InputsFacade {
public:
    InputsFacade() = default;

    ~InputsFacade() = default;

    InputsFacade(const InputsFacade& other) = delete;

    InputsFacade& operator=(const InputsFacade& other) = delete;

    InputsFacade(InputsFacade&& other) = delete;

    InputsFacade& operator=(InputsFacade&& other) = delete;

public:
    bool RegisterKeyboardActionListener(IKeyboardActionListener& listener);

    bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener);

    bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const;

    bool RegisterTextListener(ITextListener& listener);

    bool UnregisterTextListener(ITextListener& listener);

    bool IsTextListenerRegistered(ITextListener& listener) const;

    bool RegisterMouseActionListener(IMouseActionListener& listener);

    bool UnregisterMouseActionListener(IMouseActionListener& listener);

    bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const;

    bool RegisterMouseScrollListener(IMouseScrollListener& listener);

    bool UnregisterMouseScrollListener(IMouseScrollListener& listener);

    bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const;

    bool RegisterTouchActionListener(ITouchActionListener& listener);

    bool UnregisterTouchActionListener(ITouchActionListener& listener);

    bool IsTouchActionListenerRegistered(ITouchActionListener& listener) const;

public:
    bool IsKeyPressed(const KeyCode keyCode) const;

    std::set<KeyCode> GetPressedKeys() const;

    std::set<MouseButtonType> GetPressedButtons() const;

    glm::vec2 GetMousePosition() const;

    bool IsButtonPressed(const MouseButtonType button) const;

    bool IsMouseLocked() const;

    void SetMouseLocked(bool locked);

    std::map<uint8_t, Touch> GetTouches() const;

    bool IsPointerTouched(const uint8_t pointerId) const;

    bool IsMouseCursorVisible() const;

    void SetMouseCursorVisible(bool visible);

private:
    KeyboardInputComponnet m_keyboardInputComponent;

    MouseInputComponent m_mouseInputComponent;

    TouchInputComponent m_touchInuptComponent;
};

} // namespace PreVEngine

#endif // !__INPUTS_H__
