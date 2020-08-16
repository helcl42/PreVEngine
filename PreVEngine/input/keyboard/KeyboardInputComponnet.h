#ifndef __KEYBOARD_INPUT_COMPONENT_H__
#define __KEYBOARD_INPUT_COMPONENT_H__

#include "IKeyboardActionListener.h"
#include "ITextListener.h"
#include "KeyboardEvents.h"

#include "../../common/pattern/Observer.h"
#include "../../event/EventHandler.h"

#include <mutex>

namespace prev {
class KeyboardInputComponnet final {
public:
    KeyboardInputComponnet() = default;

    ~KeyboardInputComponnet() = default;

    KeyboardInputComponnet(const KeyboardInputComponnet& other) = delete;

    KeyboardInputComponnet& operator=(const KeyboardInputComponnet& other) = delete;

    KeyboardInputComponnet(KeyboardInputComponnet&& other) = delete;

    KeyboardInputComponnet& operator=(KeyboardInputComponnet&& other) = delete;

public:
    bool RegisterKeyboardActionListener(IKeyboardActionListener& listener);

    bool UnregisterKeyboardActionListener(IKeyboardActionListener& listener);

    bool IsKeyboardActionListenerRegistered(IKeyboardActionListener& listener) const;

    bool RegisterTextListener(ITextListener& listener);

    bool UnregisterTextListener(ITextListener& listener);

    bool IsTextListenerRegistered(ITextListener& listener) const;

public:
    bool IsKeyPressed(const KeyCode keyCode) const;

    const std::set<KeyCode> GetPressedKeys() const;

public:
    void operator()(const KeyEvent& keyEvent);

    void operator()(const TextEvent& textEvent);

private:
    EventHandler<KeyboardInputComponnet, KeyEvent> m_keyEventsHandler{ *this };

    EventHandler<KeyboardInputComponnet, TextEvent> m_textEventHandler{ *this };

private:
    prev::common::pattern::Observer<IKeyboardActionListener> m_keyActionObservers;

    prev::common::pattern::Observer<ITextListener> m_textObservers;

private:
    mutable std::mutex m_mutex;

    std::set<KeyCode> m_pressedKeys;
};
} // namespace prev

#endif // !__KEYBOARD_INPUT_COMPONENT_H__
