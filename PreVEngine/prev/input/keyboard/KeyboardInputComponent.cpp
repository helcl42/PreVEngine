#include "KeyboardInputComponent.h"

namespace prev::input::keyboard {
bool KeyboardInputComponent::RegisterKeyboardActionListener(IKeyboardActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.Register(listener);
}

bool KeyboardInputComponent::UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.Unregister(listener);
}

bool KeyboardInputComponent::IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.IsRegistered(listener);
}

bool KeyboardInputComponent::RegisterTextListener(ITextListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.Register(listener);
}

bool KeyboardInputComponent::UnregisterTextListener(ITextListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.Unregister(listener);
}

bool KeyboardInputComponent::IsTextListenerRegistered(ITextListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.IsRegistered(listener);
}

bool KeyboardInputComponent::IsKeyPressed(const KeyCode keyCode) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedKeys.find(keyCode) != m_pressedKeys.cend();
}

std::set<KeyCode> KeyboardInputComponent::GetPressedKeys() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedKeys;
}

void KeyboardInputComponent::operator()(const KeyEvent& keyEvent)
{
    std::set<IKeyboardActionListener*> observers;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (keyEvent.action == KeyActionType::PRESS) {
            m_pressedKeys.insert(keyEvent.keyCode);
        } else if (keyEvent.action == KeyActionType::RELEASE) {
            m_pressedKeys.erase(keyEvent.keyCode);
        }
        observers = m_keyActionObservers.GetObservers();
    }

    for (auto* listener : observers) {
        listener->OnKeyAction(keyEvent);
    }
}

void KeyboardInputComponent::operator()(const TextEvent& textEvent)
{
    std::set<ITextListener*> observers;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        observers = m_textObservers.GetObservers();
    }

    for (auto* listener : observers) {
        listener->OnText(textEvent);
    }
}
} // namespace prev::input::keyboard