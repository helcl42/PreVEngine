#include "TouchInputComponent.h"

namespace prev::input::touch {
bool TouchInputComponent::RegisterTouchActionListener(ITouchActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_touchObservers.Register(listener);
}

bool TouchInputComponent::UnregisterTouchActionListener(ITouchActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_touchObservers.Unregister(listener);
}

bool TouchInputComponent::IsTouchActionListenerRegistered(ITouchActionListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_touchObservers.IsLRegistered(listener);
}

std::map<uint8_t, Touch> TouchInputComponent::GetTouches() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_touchedDownPointers;
}

bool TouchInputComponent::IsPointerTouched(const uint8_t pointerId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_touchedDownPointers.find(pointerId) != m_touchedDownPointers.cend();
}

void TouchInputComponent::operator()(const TouchEvent& touchEvent)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (touchEvent.action == TouchActionType::DOWN) {
        m_touchedDownPointers[touchEvent.pointerId] = Touch{ touchEvent.pointerId, touchEvent.position };
    } else if (touchEvent.action == TouchActionType::UP) {
        m_touchedDownPointers.erase(touchEvent.pointerId);
    }

    for (auto& listener : m_touchObservers.GetObservers()) {
        listener->OnTouchAction(touchEvent);
    }
}
} // namespace prev::input::touch
