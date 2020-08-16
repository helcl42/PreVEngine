#ifndef __MOUSE_INPUT_COMPONENT_H__
#define __MOUSE_INPUT_COMPONENT_H__

#include "IMouseActionListener.h"
#include "IMouseScrollListener.h"
#include "MouseEvents.h"

#include "../../common/pattern/Observer.h"
#include "../../event/EventHandler.h"

#include <mutex>
#include <set>

namespace prev {
class MouseInputComponent final {
public:
    MouseInputComponent() = default;

    ~MouseInputComponent() = default;

    MouseInputComponent(const MouseInputComponent& other) = delete;

    MouseInputComponent& operator=(const MouseInputComponent& other) = delete;

    MouseInputComponent(MouseInputComponent&& other) = delete;

    MouseInputComponent& operator=(MouseInputComponent&& other) = delete;

public:
    bool RegisterMouseActionListener(IMouseActionListener& listener);

    bool UnregisterMouseActionListener(IMouseActionListener& listener);

    bool IsMouseActionListenerRegistered(IMouseActionListener& listener) const;

    bool RegisterMouseScrollListener(IMouseScrollListener& listener);

    bool UnregisterMouseScrollListener(IMouseScrollListener& listener);

    bool IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const;

public:
    const std::set<MouseButtonType>& GetPressedButtons() const;

    glm::vec2 GetMousePosition() const;

    bool IsButtonPressed(const MouseButtonType button) const;

    bool IsLocked() const;

    void SetLocked(bool locked);

    bool IsCursorVisible() const;

    void SetCursorVisible(bool visible);

public:
    void operator()(const MouseEvent& action);

    void operator()(const MouseScrollEvent& scrollAction);

private:
    EventHandler<MouseInputComponent, MouseEvent> m_mouseActionsHandler{ *this };

    EventHandler<MouseInputComponent, MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    prev::common::pattern::Observer<IMouseActionListener> m_mouseActionObservers;

    prev::common::pattern::Observer<IMouseScrollListener> m_mouseScrollObservers;

private:
    mutable std::mutex m_mutex;

    std::set<MouseButtonType> m_pressedButtons;

    glm::vec2 m_mousePosition{ 0, 0 };

    static inline bool m_locked{ false };

    static inline bool m_cursorVisible{ true };
};
} // namespace prev

#endif