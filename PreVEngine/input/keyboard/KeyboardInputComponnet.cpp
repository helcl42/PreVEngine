#include "KeyboardInputComponnet.h"

namespace PreVEngine {
bool KeyboardInputComponnet::RegisterKeyboardActionListener(IKeyboardActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.RegisterListener(listener);
}

bool KeyboardInputComponnet::UnregisterKeyboardActionListener(IKeyboardActionListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.UnregisterListener(listener);
}

bool KeyboardInputComponnet::IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_keyActionObservers.IsListenerRegistered(listener);
}

bool KeyboardInputComponnet::RegisterTextListener(ITextListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.RegisterListener(listener);
}

bool KeyboardInputComponnet::UnregisterTextListener(ITextListener& listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.UnregisterListener(listener);
}

bool KeyboardInputComponnet::IsTextListenerRegistered(ITextListener& listener) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_textObservers.IsListenerRegistered(listener);
}

bool KeyboardInputComponnet::IsKeyPressed(const KeyCode keyCode) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedKeys.find(keyCode) != m_pressedKeys.cend();
}

const std::set<KeyCode> KeyboardInputComponnet::GetPressedKeys() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_pressedKeys;
}

void KeyboardInputComponnet::operator()(const KeyEvent& keyEvent)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (keyEvent.action == KeyActionType::PRESS) {
        m_pressedKeys.insert(keyEvent.keyCode);
    } else if (keyEvent.action == KeyActionType::RELEASE) {
        m_pressedKeys.erase(keyEvent.keyCode);
    }

    for (auto& listener : m_keyActionObservers.GetObservers()) {
        listener->OnKeyAction(keyEvent);
    }
}

void KeyboardInputComponnet::operator()(const TextEvent& textEvent)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& listener : m_textObservers.GetObservers()) {
        listener->OnText(textEvent);
    }
}
} // namespace PreVEngine