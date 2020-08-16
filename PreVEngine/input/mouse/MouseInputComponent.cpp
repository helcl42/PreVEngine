#include "MouseInputComponent.h"

namespace prev {
bool MouseInputComponent::RegisterMouseActionListener(IMouseActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseActionObservers.RegisterListener(listener);
}

bool MouseInputComponent::UnregisterMouseActionListener(IMouseActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseActionObservers.UnregisterListener(listener);
}

bool MouseInputComponent::IsMouseActionListenerRegistered(IMouseActionListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseActionObservers.IsListenerRegistered(listener);
}

bool MouseInputComponent::RegisterMouseScrollListener(IMouseScrollListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseScrollObservers.RegisterListener(listener);
}

bool MouseInputComponent::UnregisterMouseScrollListener(IMouseScrollListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseScrollObservers.UnregisterListener(listener);
}

bool MouseInputComponent::IsMouseScrollListenerRegistered(IMouseScrollListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mouseScrollObservers.IsListenerRegistered(listener);
}

const std::set<MouseButtonType>& MouseInputComponent::GetPressedButtons() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedButtons;
}

glm::vec2 MouseInputComponent::GetMousePosition() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_mousePosition;
}

bool MouseInputComponent::IsButtonPressed(const MouseButtonType button) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedButtons.find(button) != m_pressedButtons.cend();
}

bool MouseInputComponent::IsLocked() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_locked;
}

void MouseInputComponent::SetLocked(bool locked)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_locked = locked;

    prev::event::EventChannel::Broadcast(MouseLockRequest{ locked });
}

bool MouseInputComponent::IsCursorVisible() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_cursorVisible;
}

void MouseInputComponent::SetCursorVisible(bool visible)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_cursorVisible = visible;

    prev::event::EventChannel::Broadcast(MouseCursorVisibilityRequest{ visible });
}

void MouseInputComponent::operator()(const MouseEvent& action)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_mousePosition = action.position;

    if (action.action == MouseActionType::PRESS) {
        m_pressedButtons.insert(action.button);
    } else if (action.action == MouseActionType::RELEASE) {
        m_pressedButtons.erase(action.button);
    }

    for (auto listener : m_mouseActionObservers.GetObservers()) {
        listener->OnMouseAction(action);
    }
}

void MouseInputComponent::operator()(const MouseScrollEvent& scrollAction)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& listener : m_mouseScrollObservers.GetObservers()) {
        listener->OnMouseScroll(scrollAction);
    }
}
} // namespace prev